use crate::command_message::*;
use crate::connection_info::*;
use crate::connections::*;
use crate::data_messages::*;
use crate::decode_error::*;
use crate::decoder::*;
use crate::dispatcher::*;
use crate::ping_response::*;
use crate::statistics::*;
use crossbeam::channel::Sender;
use std::ops::Drop;
use std::sync::{Arc, Mutex};
use std::time::{SystemTime, UNIX_EPOCH};

pub struct Connection {
    dropped: Arc<Mutex<bool>>,
    pub(crate) internal: Arc<Mutex<Box<dyn GenericConnection + Send>>>,
}

impl Connection {
    pub fn new(connection_info: &ConnectionInfo) -> Self {
        let connection = Self {
            dropped: Arc::new(Mutex::new(false)),
            internal: Arc::new(Mutex::new(match connection_info {
                ConnectionInfo::UsbConnectionInfo(connection_info) => Box::new(UsbConnection::new(connection_info)),
                ConnectionInfo::SerialConnectionInfo(connection_info) => Box::new(SerialConnection::new(connection_info)),
                ConnectionInfo::TcpConnectionInfo(connection_info) => Box::new(TcpConnection::new(connection_info)),
                ConnectionInfo::UdpConnectionInfo(connection_info) => Box::new(UdpConnection::new(connection_info)),
                ConnectionInfo::BluetoothConnectionInfo(connection_info) => Box::new(BluetoothConnection::new(connection_info)),
                ConnectionInfo::FileConnectionInfo(connection_info) => Box::new(FileConnection::new(connection_info)),
            })),
        };

        let dropped = connection.dropped.clone();
        let decoder = connection.internal.lock().unwrap().get_decoder();
        let initial_time = SystemTime::now().duration_since(UNIX_EPOCH).unwrap().as_micros();
        let mut previous_statistics: Statistics = Default::default();

        std::thread::spawn(move || loop {
            std::thread::sleep(std::time::Duration::from_secs(1));

            if let Ok(mut decoder) = decoder.lock() {
                decoder.statistics.timestamp = (SystemTime::now().duration_since(UNIX_EPOCH).unwrap().as_micros() - initial_time) as u64;

                let delta_time = (decoder.statistics.timestamp - previous_statistics.timestamp) as f32 / 1E6;
                let delta_data = decoder.statistics.data_total - previous_statistics.data_total;
                let delta_message = decoder.statistics.message_total - previous_statistics.message_total;
                let delta_error = decoder.statistics.error_total - previous_statistics.error_total;

                decoder.statistics.data_rate = (delta_data as f32 / delta_time).round() as u32;
                decoder.statistics.message_rate = (delta_message as f32 / delta_time).round() as u32;
                decoder.statistics.error_rate = (delta_error as f32 / delta_time).round() as u32;

                previous_statistics = decoder.statistics;

                decoder.dispatcher.sender.send(DispatcherData::Statistics(decoder.statistics)).ok();
            }

            if *dropped.lock().unwrap() {
                return;
            }
        });

        connection
    }

    pub fn open(&self) -> std::io::Result<()> {
        self.internal.lock().unwrap().open()
    }

    pub fn open_async(&self, closure: Box<dyn FnOnce(std::io::Result<()>) + Send>) {
        let internal = self.internal.clone();
        let dropped = self.dropped.clone();

        std::thread::spawn(move || {
            let result = internal.lock().unwrap().open();

            if let Ok(dropped) = dropped.lock() {
                if *dropped {
                    return;
                }
                closure(result);
            }
        });
    }

    pub fn close(&self) {
        self.internal.lock().unwrap().close();
    }

    pub fn ping(&self) -> Result<PingResponse, ()> {
        let decoder = self.internal.lock().unwrap().get_decoder();
        let write_sender = self.internal.lock().unwrap().get_write_sender();

        Self::ping_internal(decoder, write_sender)
    }

    pub fn ping_async(&self, closure: Box<dyn FnOnce(Result<PingResponse, ()>) + Send>) {
        let decoder = self.internal.lock().unwrap().get_decoder();
        let write_sender = self.internal.lock().unwrap().get_write_sender();
        let dropped = self.dropped.clone();

        std::thread::spawn(move || {
            let ping_response = Self::ping_internal(decoder, write_sender);

            if let Ok(dropped) = dropped.lock() {
                if *dropped {
                    return;
                }
                closure(ping_response);
            }
        });
    }

    pub(crate) fn ping_internal(decoder: Arc<Mutex<Decoder>>, write_sender: Option<Sender<Vec<u8>>>) -> Result<PingResponse, ()> {
        let responses = Self::send_commands_internal(decoder, write_sender, vec!["{\"ping\":null}"], 4, 200); // 4 retries with 200 ms timeout = 1 second

        if responses.len() == 0 {
            return Err(());
        }

        PingResponse::parse(responses[0].as_str())
    }

    pub fn send_commands(&self, commands: Vec<&str>, retries: u32, timeout: u32) -> Vec<String> {
        let decoder = self.internal.lock().unwrap().get_decoder();
        let write_sender = self.internal.lock().unwrap().get_write_sender();

        Self::send_commands_internal(decoder, write_sender, commands, retries, timeout)
    }

    pub fn send_commands_async(&self, commands: Vec<&str>, retries: u32, timeout: u32, closure: Box<dyn FnOnce(Vec<String>) + Send>) {
        let decoder = self.internal.lock().unwrap().get_decoder();
        let write_sender = self.internal.lock().unwrap().get_write_sender();
        let dropped = self.dropped.clone();
        let commands: Vec<String> = commands.iter().map(|&string| string.to_owned()).collect();

        std::thread::spawn(move || {
            let responses = Self::send_commands_internal(decoder, write_sender, commands.iter().map(|string| string.as_ref()).collect(), retries, timeout);

            if let Ok(dropped) = dropped.lock() {
                if *dropped {
                    return;
                }
                closure(responses);
            }
        });
    }

    fn send_commands_internal(decoder: Arc<Mutex<Decoder>>, write_sender: Option<Sender<Vec<u8>>>, commands: Vec<&str>, retries: u32, timeout: u32) -> Vec<String> {
        struct Transaction {
            command: Option<CommandMessage>,
            response: Option<CommandMessage>,
        }

        let mut transactions: Vec<Transaction> = commands
            .iter()
            .map(|&command| {
                if let Ok(command) = CommandMessage::parse_json(command) {
                    Transaction {
                        command: Some(command),
                        response: None,
                    }
                } else {
                    Transaction {
                        command: None,
                        response: None,
                    }
                }
            })
            .collect();

        let (response_sender, response_receiver) = crossbeam::channel::unbounded();

        let closure_id = decoder.lock().unwrap().dispatcher.add_command_closure(Box::new(move |command: CommandMessage| {
            response_sender.send(command).ok();
        }));

        'outer: for _ in 0..(1 + retries) {
            for command in transactions.iter().filter_map(|transaction| transaction.command.clone()) {
                if let Some(write_sender) = &write_sender {
                    write_sender.send(command.terminated_json).ok();
                }
            }

            let end_time = SystemTime::now().duration_since(UNIX_EPOCH).unwrap().as_millis() + timeout as u128;

            while SystemTime::now().duration_since(UNIX_EPOCH).unwrap().as_millis() < end_time {
                if let Ok(response) = response_receiver.try_recv() {
                    for transaction in transactions.iter_mut() {
                        if let Some(command) = transaction.command.as_ref() {
                            if response.key == command.key {
                                *transaction = Transaction {
                                    command: None,
                                    response: Some(response.clone()),
                                };
                            }
                        }
                    }
                }

                if transactions.iter().all(|transaction| transaction.command.as_ref().is_none()) {
                    break 'outer;
                }

                std::thread::sleep(std::time::Duration::from_millis(1));
            }
        }

        decoder.lock().unwrap().dispatcher.remove_closure(closure_id);

        let responses: Vec<_> = transactions.iter().filter_map(|transaction| transaction.response.clone()).collect();
        responses.iter().map(|response| response.json.clone()).collect()
    }

    pub fn get_info(&self) -> ConnectionInfo {
        self.internal.lock().unwrap().get_info()
    }

    pub fn get_statistics(&self) -> Statistics {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().statistics
    }

    pub fn add_decode_error_closure(&self, closure: Box<dyn Fn(DecodeError) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_decode_error_closure(closure)
    }

    pub fn add_statistics_closure(&self, closure: Box<dyn Fn(Statistics) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_statistics_closure(closure)
    }

    pub(crate) fn add_command_closure(&self, closure: Box<dyn Fn(CommandMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_command_closure(closure)
    }

    pub(crate) fn add_data_closure(&self, closure: Box<dyn Fn(Box<dyn DataMessage>) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_data_closure(closure)
    }

    // Start of code block #0 generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py

    pub fn add_inertial_closure(&self, closure: Box<dyn Fn(InertialMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_inertial_closure(closure)
    }

    pub fn add_magnetometer_closure(&self, closure: Box<dyn Fn(MagnetometerMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_magnetometer_closure(closure)
    }

    pub fn add_quaternion_closure(&self, closure: Box<dyn Fn(QuaternionMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_quaternion_closure(closure)
    }

    pub fn add_rotation_matrix_closure(&self, closure: Box<dyn Fn(RotationMatrixMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_rotation_matrix_closure(closure)
    }

    pub fn add_euler_angles_closure(&self, closure: Box<dyn Fn(EulerAnglesMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_euler_angles_closure(closure)
    }

    pub fn add_linear_acceleration_closure(&self, closure: Box<dyn Fn(LinearAccelerationMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_linear_acceleration_closure(closure)
    }

    pub fn add_earth_acceleration_closure(&self, closure: Box<dyn Fn(EarthAccelerationMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_earth_acceleration_closure(closure)
    }

    pub fn add_ahrs_status_closure(&self, closure: Box<dyn Fn(AhrsStatusMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_ahrs_status_closure(closure)
    }

    pub fn add_high_g_accelerometer_closure(&self, closure: Box<dyn Fn(HighGAccelerometerMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_high_g_accelerometer_closure(closure)
    }

    pub fn add_temperature_closure(&self, closure: Box<dyn Fn(TemperatureMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_temperature_closure(closure)
    }

    pub fn add_battery_closure(&self, closure: Box<dyn Fn(BatteryMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_battery_closure(closure)
    }

    pub fn add_rssi_closure(&self, closure: Box<dyn Fn(RssiMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_rssi_closure(closure)
    }

    pub fn add_serial_accessory_closure(&self, closure: Box<dyn Fn(SerialAccessoryMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_serial_accessory_closure(closure)
    }

    pub fn add_notification_closure(&self, closure: Box<dyn Fn(NotificationMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_notification_closure(closure)
    }

    pub fn add_error_closure(&self, closure: Box<dyn Fn(ErrorMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_error_closure(closure)
    }
    // End of code block #0 generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py

    pub fn add_end_of_file_closure(&self, closure: Box<dyn Fn() + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_end_of_file_closure(closure)
    }

    pub fn remove_closure(&self, id: u64) {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.remove_closure(id);
    }
}

impl Drop for Connection {
    fn drop(&mut self) {
        *self.dropped.lock().unwrap() = true;
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.remove_all_closures();
        self.close(); // call after open_sync complete
    }
}
