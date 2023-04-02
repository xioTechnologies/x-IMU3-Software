use crossbeam::channel::Sender;
use std::ops::Drop;
use std::sync::{Arc, Mutex};
use std::time::{SystemTime, UNIX_EPOCH};
use crate::command_message::*;
use crate::connection_info::*;
use crate::connections::*;
use crate::data_messages::*;
use crate::decode_error::*;
use crate::decoder::*;
use crate::dispatcher::*;
use crate::ping_response::*;
use crate::statistics::*;

pub struct Connection {
    dropped: Arc<Mutex<bool>>,
    internal: Arc<Mutex<Box<dyn GenericConnection + Send>>>,
}

impl Connection {
    pub fn new(connection_info: &ConnectionInfo) -> Connection {
        let internal: Box<dyn GenericConnection + Send>;

        match connection_info {
            ConnectionInfo::UsbConnectionInfo(connection_info) => internal = Box::new(UsbConnection::new(connection_info)),
            ConnectionInfo::SerialConnectionInfo(connection_info) => internal = Box::new(SerialConnection::new(connection_info)),
            ConnectionInfo::TcpConnectionInfo(connection_info) => internal = Box::new(TcpConnection::new(connection_info)),
            ConnectionInfo::UdpConnectionInfo(connection_info) => internal = Box::new(UdpConnection::new(connection_info)),
            ConnectionInfo::BluetoothConnectionInfo(connection_info) => internal = Box::new(BluetoothConnection::new(connection_info)),
            ConnectionInfo::FileConnectionInfo(connection_info) => internal = Box::new(FileConnection::new(connection_info)),
        }

        let connection = Connection {
            dropped: Arc::new(Mutex::new(false)),
            internal: Arc::new(Mutex::new(internal)),
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

    pub fn open(&mut self) -> std::io::Result<()> {
        self.internal.lock().unwrap().open()
    }

    pub fn open_async(&mut self, closure: Box<dyn FnOnce(std::io::Result<()>) + Send>) {
        let internal = self.internal.clone();
        let dropped = self.dropped.clone();

        std::thread::spawn(move || {
            let result = internal.lock().unwrap().open();

            if let Ok(dropped) = dropped.lock() {
                if *dropped {
                    return;
                }

                if result.is_err() {
                    closure(result); // argument must not include lock() because this could cause deadlock
                    return;
                }

                closure(Ok(()));
            }
        });
    }

    pub fn close(&mut self) {
        self.internal.lock().unwrap().close();
    }

    pub fn ping(&mut self) -> Result<PingResponse, ()> {
        let response = self.send_commands(vec!["{\"ping\":null}"], 4, 200); // 4 retries with 200 ms timeout = 1 second

        if response.len() == 0 {
            return Err(());
        }

        PingResponse::parse_json(response[0].as_str())
    }

    pub fn send_commands(&mut self, commands: Vec<&str>, retries: u32, timeout: u32) -> Vec<String> {
        let decoder = self.internal.lock().unwrap().get_decoder();
        let write_sender = self.internal.lock().unwrap().get_write_sender();

        Self::send_commands_internal(decoder, write_sender, commands, retries, timeout) // argument must not include lock() because this could cause deadlock
    }

    pub fn send_commands_async(&mut self, commands: Vec<&str>, retries: u32, timeout: u32, closure: Box<dyn FnOnce(Vec<String>) + Send>) {
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

    fn send_commands_internal(decoder: Arc<Mutex<Decoder>>, write_sender: Option<Sender<String>>, commands: Vec<&str>, retries: u32, timeout: u32) -> Vec<String> {
        struct Transaction {
            command: Option<CommandMessage>,
            response: String,
        }

        let mut transactions: Vec<Transaction> = commands.iter().map(|&command| {
            if let Ok(command) = CommandMessage::parse_json(command) {
                Transaction { command: Some(command), response: "".to_owned() }
            } else {
                Transaction { command: None, response: "".to_owned() }
            }
        }).collect();

        let (response_sender, response_receiver) = crossbeam::channel::unbounded();

        let closure_id = decoder.lock().unwrap().dispatcher.add_command_closure(Box::new(move |command: CommandMessage| {
            response_sender.send(command).ok();
        }));

        for _ in 0..(1 + retries) {
            for transaction in transactions.iter().filter(|&transaction| transaction.command.is_some()) {
                if let Some(write_sender) = write_sender.clone() {
                    write_sender.send(transaction.command.as_ref().unwrap().terminated_json.clone()).ok();
                }
            }

            let end_time = SystemTime::now().duration_since(UNIX_EPOCH).unwrap().as_millis() + timeout as u128;

            while SystemTime::now().duration_since(UNIX_EPOCH).unwrap().as_millis() < end_time {
                if let Ok(response) = response_receiver.try_recv() {
                    for index in 0..transactions.len() {
                        if transactions[index].command.is_some() && response.key == transactions[index].command.as_ref().unwrap().key {
                            transactions[index] = Transaction { command: None, response: response.json.clone() };
                        }
                    }
                }

                if transactions.iter().all(|transaction| transaction.command.as_ref().is_none()) {
                    decoder.lock().unwrap().dispatcher.remove_closure(closure_id);
                    transactions.retain(|transaction| transaction.response.is_empty() == false);
                    return transactions.iter().map(|transaction| transaction.response.clone()).collect();
                }

                std::thread::sleep(std::time::Duration::from_millis(1));
            }
        }

        decoder.lock().unwrap().dispatcher.remove_closure(closure_id);
        transactions.retain(|transaction| transaction.response.is_empty() == false);
        return transactions.iter().map(|transaction| transaction.response.clone()).collect();
    }

    pub fn get_info(&mut self) -> ConnectionInfo {
        self.internal.lock().unwrap().get_info()
    }

    pub fn get_statistics(&mut self) -> Statistics {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().statistics
    }

    pub fn add_decode_error_closure(&mut self, closure: Box<dyn Fn(DecodeError) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_decode_error_closure(closure)
    }

    pub fn add_statistics_closure(&mut self, closure: Box<dyn Fn(Statistics) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_statistics_closure(closure)
    }

    pub(crate) fn add_command_closure(&mut self, closure: Box<dyn Fn(CommandMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_command_closure(closure)
    }

    pub(crate) fn add_data_closure(&mut self, closure: Box<dyn Fn(Box<dyn DataMessage>) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_data_closure(closure)
    }

    // Start of code block #0 generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py

    pub fn add_inertial_closure(&mut self, closure: Box<dyn Fn(InertialMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_inertial_closure(closure)
    }

    pub fn add_magnetometer_closure(&mut self, closure: Box<dyn Fn(MagnetometerMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_magnetometer_closure(closure)
    }

    pub fn add_quaternion_closure(&mut self, closure: Box<dyn Fn(QuaternionMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_quaternion_closure(closure)
    }

    pub fn add_rotation_matrix_closure(&mut self, closure: Box<dyn Fn(RotationMatrixMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_rotation_matrix_closure(closure)
    }

    pub fn add_euler_angles_closure(&mut self, closure: Box<dyn Fn(EulerAnglesMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_euler_angles_closure(closure)
    }

    pub fn add_linear_acceleration_closure(&mut self, closure: Box<dyn Fn(LinearAccelerationMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_linear_acceleration_closure(closure)
    }

    pub fn add_earth_acceleration_closure(&mut self, closure: Box<dyn Fn(EarthAccelerationMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_earth_acceleration_closure(closure)
    }

    pub fn add_high_g_accelerometer_closure(&mut self, closure: Box<dyn Fn(HighGAccelerometerMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_high_g_accelerometer_closure(closure)
    }

    pub fn add_temperature_closure(&mut self, closure: Box<dyn Fn(TemperatureMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_temperature_closure(closure)
    }

    pub fn add_battery_closure(&mut self, closure: Box<dyn Fn(BatteryMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_battery_closure(closure)
    }

    pub fn add_rssi_closure(&mut self, closure: Box<dyn Fn(RssiMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_rssi_closure(closure)
    }

    pub fn add_serial_accessory_closure(&mut self, closure: Box<dyn Fn(SerialAccessoryMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_serial_accessory_closure(closure)
    }

    pub fn add_notification_closure(&mut self, closure: Box<dyn Fn(NotificationMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_notification_closure(closure)
    }

    pub fn add_error_closure(&mut self, closure: Box<dyn Fn(ErrorMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_error_closure(closure)
    }
    // End of code block #0 generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py

    pub fn remove_closure(&mut self, id: u64) {
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
