use crate::command_message::*;
use crate::connection_config::*;
use crate::connections::*;
use crate::data_messages::*;
use crate::dispatcher::*;
use crate::mux_message::*;
use crate::ping_response::*;
use crate::receive_error::*;
use crate::receiver::*;
use crate::statistics::*;
use crossbeam::channel::Sender;
use std::ops::Drop;
use std::sync::{Arc, Mutex};

pub const DEFAULT_RETRIES: u32 = 2;

pub const DEFAULT_TIMEOUT: u32 = 500; // milliseconds

pub struct Connection {
    dropped: Arc<Mutex<bool>>,
    pub(crate) internal: Arc<Mutex<Box<dyn GenericConnection + Send>>>,
}

impl Connection {
    pub fn new(config: &ConnectionConfig) -> Self {
        let connection = Self {
            dropped: Arc::new(Mutex::new(false)),
            internal: Arc::new(Mutex::new(match config {
                ConnectionConfig::UsbConnectionConfig(config) => Box::new(UsbConnection::new(config)),
                ConnectionConfig::SerialConnectionConfig(config) => Box::new(SerialConnection::new(config)),
                ConnectionConfig::TcpConnectionConfig(config) => Box::new(TcpConnection::new(config)),
                ConnectionConfig::UdpConnectionConfig(config) => Box::new(UdpConnection::new(config)),
                ConnectionConfig::BluetoothConnectionConfig(config) => Box::new(BluetoothConnection::new(config)),
                ConnectionConfig::FileConnectionConfig(config) => Box::new(FileConnection::new(config)),
                ConnectionConfig::MuxConnectionConfig(config) => Box::new(MuxConnection::new(config)),
            })),
        };

        let dropped = connection.dropped.clone();
        let receiver = connection.internal.lock().unwrap().get_receiver();
        let start_time = std::time::Instant::now();
        let mut previous_statistics: Statistics = Default::default();

        std::thread::spawn(move || loop {
            std::thread::sleep(std::time::Duration::from_secs(1));

            if let Ok(mut receiver) = receiver.lock() {
                receiver.statistics.timestamp = start_time.elapsed().as_micros() as u64;

                let delta_time = (receiver.statistics.timestamp - previous_statistics.timestamp) as f32 / 1E6;
                let delta_data = receiver.statistics.data_total - previous_statistics.data_total;
                let delta_message = receiver.statistics.message_total - previous_statistics.message_total;
                let delta_error = receiver.statistics.error_total - previous_statistics.error_total;

                receiver.statistics.data_rate = (delta_data as f32 / delta_time).round() as u32;
                receiver.statistics.message_rate = (delta_message as f32 / delta_time).round() as u32;
                receiver.statistics.error_rate = (delta_error as f32 / delta_time).round() as u32;

                previous_statistics = receiver.statistics;

                receiver.dispatcher.sender.send(DispatcherData::Statistics(receiver.statistics)).ok();
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

    pub fn ping(&self) -> Option<PingResponse> {
        let receiver = self.internal.lock().unwrap().get_receiver();
        let write_sender = self.internal.lock().unwrap().get_write_sender();

        Self::ping_internal(receiver, write_sender)
    }

    pub fn ping_async(&self, closure: Box<dyn FnOnce(Option<PingResponse>) + Send>) {
        let receiver = self.internal.lock().unwrap().get_receiver();
        let write_sender = self.internal.lock().unwrap().get_write_sender();
        let dropped = self.dropped.clone();

        std::thread::spawn(move || {
            let response = Self::ping_internal(receiver, write_sender);

            if let Ok(dropped) = dropped.lock() {
                if *dropped {
                    return;
                }
                closure(response);
            }
        });
    }

    pub(crate) fn ping_internal(receiver: Arc<Mutex<Receiver>>, write_sender: Option<Sender<Vec<u8>>>) -> Option<PingResponse> {
        let responses = Self::send_commands_internal(receiver, write_sender, vec!["{\"ping\":null}".into()], 4, 200);

        PingResponse::parse(&responses.first()?.as_ref()?.json)
    }

    pub fn send_command(&self, command: Vec<u8>, retries: u32, timeout: u32) -> Option<CommandMessage> {
        self.send_commands(vec![command], retries, timeout).first()?.clone()
    }

    pub fn send_commands(&self, commands: Vec<Vec<u8>>, retries: u32, timeout: u32) -> Vec<Option<CommandMessage>> {
        let receiver = self.internal.lock().unwrap().get_receiver();
        let write_sender = self.internal.lock().unwrap().get_write_sender();

        Self::send_commands_internal(receiver, write_sender, commands, retries, timeout)
    }

    pub fn send_command_async(&self, command: Vec<u8>, retries: u32, timeout: u32, closure: Box<dyn FnOnce(Option<CommandMessage>) + Send>) {
        self.send_commands_async(vec![command], retries, timeout, Box::new(move |responses: Vec<Option<CommandMessage>>| closure(responses.first().cloned().flatten())));
    }

    pub fn send_commands_async(&self, commands: Vec<Vec<u8>>, retries: u32, timeout: u32, closure: Box<dyn FnOnce(Vec<Option<CommandMessage>>) + Send>) {
        let receiver = self.internal.lock().unwrap().get_receiver();
        let write_sender = self.internal.lock().unwrap().get_write_sender();
        let dropped = self.dropped.clone();

        std::thread::spawn(move || {
            let responses = Self::send_commands_internal(receiver, write_sender, commands, retries, timeout);

            if let Ok(dropped) = dropped.lock() {
                if *dropped {
                    return;
                }
                closure(responses);
            }
        });
    }

    fn send_commands_internal(receiver: Arc<Mutex<Receiver>>, write_sender: Option<Sender<Vec<u8>>>, commands: Vec<Vec<u8>>, retries: u32, timeout: u32) -> Vec<Option<CommandMessage>> {
        struct Transaction {
            command: Option<CommandMessage>,
            response: Option<CommandMessage>,
        }

        let mut transactions: Vec<Transaction> = commands
            .iter()
            .map(|command| Transaction {
                command: CommandMessage::parse(command).map(|message| Some(message)).unwrap_or(None),
                response: None,
            })
            .collect();

        let (response_sender, response_receiver) = crossbeam::channel::unbounded();

        let closure_id = receiver.lock().unwrap().dispatcher.add_command_closure(Box::new(move |command: CommandMessage| {
            response_sender.send(command).ok();
        }));

        'outer: for _ in 0..(1 + retries) {
            for command in transactions.iter().filter_map(|transaction| transaction.command.clone()) {
                if let Some(write_sender) = &write_sender {
                    write_sender.send([command.json.as_slice(), &[b'\n']].concat()).ok();
                }
            }

            let start_time = std::time::Instant::now();

            while start_time.elapsed() < std::time::Duration::from_millis(timeout as u64) {
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

        receiver.lock().unwrap().dispatcher.remove_closure(closure_id);

        transactions.iter().map(|transaction| transaction.response.clone()).collect()
    }

    pub fn get_config(&self) -> ConnectionConfig {
        self.internal.lock().unwrap().get_config()
    }

    pub fn get_statistics(&self) -> Statistics {
        self.internal.lock().unwrap().get_receiver().lock().unwrap().statistics
    }

    pub fn add_receive_error_closure(&self, closure: Box<dyn Fn(ReceiveError) + Send>) -> u64 {
        self.internal.lock().unwrap().get_receiver().lock().unwrap().dispatcher.add_receive_error_closure(closure)
    }

    pub fn add_statistics_closure(&self, closure: Box<dyn Fn(Statistics) + Send>) -> u64 {
        self.internal.lock().unwrap().get_receiver().lock().unwrap().dispatcher.add_statistics_closure(closure)
    }

    pub(crate) fn add_command_closure(&self, closure: Box<dyn Fn(CommandMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_receiver().lock().unwrap().dispatcher.add_command_closure(closure)
    }

    pub(crate) fn add_mux_closure(&self, closure: Box<dyn Fn(MuxMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_receiver().lock().unwrap().dispatcher.add_mux_closure(closure)
    }

    pub(crate) fn add_data_closure(&self, closure: Box<dyn Fn(Box<dyn DataMessage>) + Send>) -> u64 {
        self.internal.lock().unwrap().get_receiver().lock().unwrap().dispatcher.add_data_closure(closure)
    }

    // Start of code block #0 generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py

    pub fn add_inertial_closure(&self, closure: Box<dyn Fn(InertialMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_receiver().lock().unwrap().dispatcher.add_inertial_closure(closure)
    }

    pub fn add_magnetometer_closure(&self, closure: Box<dyn Fn(MagnetometerMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_receiver().lock().unwrap().dispatcher.add_magnetometer_closure(closure)
    }

    pub fn add_quaternion_closure(&self, closure: Box<dyn Fn(QuaternionMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_receiver().lock().unwrap().dispatcher.add_quaternion_closure(closure)
    }

    pub fn add_rotation_matrix_closure(&self, closure: Box<dyn Fn(RotationMatrixMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_receiver().lock().unwrap().dispatcher.add_rotation_matrix_closure(closure)
    }

    pub fn add_euler_angles_closure(&self, closure: Box<dyn Fn(EulerAnglesMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_receiver().lock().unwrap().dispatcher.add_euler_angles_closure(closure)
    }

    pub fn add_linear_acceleration_closure(&self, closure: Box<dyn Fn(LinearAccelerationMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_receiver().lock().unwrap().dispatcher.add_linear_acceleration_closure(closure)
    }

    pub fn add_earth_acceleration_closure(&self, closure: Box<dyn Fn(EarthAccelerationMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_receiver().lock().unwrap().dispatcher.add_earth_acceleration_closure(closure)
    }

    pub fn add_ahrs_status_closure(&self, closure: Box<dyn Fn(AhrsStatusMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_receiver().lock().unwrap().dispatcher.add_ahrs_status_closure(closure)
    }

    pub fn add_high_g_accelerometer_closure(&self, closure: Box<dyn Fn(HighGAccelerometerMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_receiver().lock().unwrap().dispatcher.add_high_g_accelerometer_closure(closure)
    }

    pub fn add_temperature_closure(&self, closure: Box<dyn Fn(TemperatureMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_receiver().lock().unwrap().dispatcher.add_temperature_closure(closure)
    }

    pub fn add_battery_closure(&self, closure: Box<dyn Fn(BatteryMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_receiver().lock().unwrap().dispatcher.add_battery_closure(closure)
    }

    pub fn add_rssi_closure(&self, closure: Box<dyn Fn(RssiMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_receiver().lock().unwrap().dispatcher.add_rssi_closure(closure)
    }

    pub fn add_serial_accessory_closure(&self, closure: Box<dyn Fn(SerialAccessoryMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_receiver().lock().unwrap().dispatcher.add_serial_accessory_closure(closure)
    }

    pub fn add_notification_closure(&self, closure: Box<dyn Fn(NotificationMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_receiver().lock().unwrap().dispatcher.add_notification_closure(closure)
    }

    pub fn add_error_closure(&self, closure: Box<dyn Fn(ErrorMessage) + Send>) -> u64 {
        self.internal.lock().unwrap().get_receiver().lock().unwrap().dispatcher.add_error_closure(closure)
    }
    // End of code block #0 generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py

    pub fn add_end_of_file_closure(&self, closure: Box<dyn Fn() + Send>) -> u64 {
        self.internal.lock().unwrap().get_receiver().lock().unwrap().dispatcher.add_end_of_file_closure(closure)
    }

    pub fn remove_closure(&self, id: u64) {
        self.internal.lock().unwrap().get_receiver().lock().unwrap().dispatcher.remove_closure(id);
    }
}

impl Drop for Connection {
    fn drop(&mut self) {
        *self.dropped.lock().unwrap() = true;
        self.internal.lock().unwrap().get_receiver().lock().unwrap().dispatcher.remove_all_closures();
        self.close(); // call after open_sync complete
    }
}
