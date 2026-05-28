use crate::connection::*;
use crate::connection_config::*;
use crate::device::*;
use crate::mux_message::*;
use crate::ping_response::*;
use std::ops::Drop;
use std::sync::atomic::{AtomicBool, AtomicU64, Ordering};
use std::sync::{Arc, Mutex};

pub const MAX_NUMBER_OF_MUX_CHANNELS: u32 = 254; // '\n' and '^' are reserved

pub struct MuxScanner<'a> {
    dropped: Arc<Mutex<bool>>,
    connection: &'a Connection,
    mux_closure_id: u64,
    closure_counter: AtomicU64,
    closures: Arc<Mutex<Vec<(Box<dyn Fn(Vec<Device>) + Send>, u64)>>>,
    devices: Arc<Mutex<Vec<Device>>>,
}

impl<'a> MuxScanner<'a> {
    pub fn new(connection: &'a Connection) -> Self {
        let devices = Arc::new(Mutex::new(Vec::new()));
        let updated = Arc::new(AtomicBool::new(false));

        let mux_closure_id = connection.add_mux_closure(Box::new({
            let connection = connection.internal.clone();
            let devices = devices.clone();
            let updated = updated.clone();

            move |message| {
                let Some(device) = Self::parse_mux_message(message, &connection) else {
                    return;
                };

                if Self::update_devices(&devices, &device) {
                    updated.store(true, Ordering::SeqCst);
                }
            }
        }));

        let mux_scanner = Self {
            dropped: Arc::new(Mutex::new(false)),
            connection,
            mux_closure_id,
            closure_counter: AtomicU64::new(0),
            closures: Arc::new(Mutex::new(Vec::new())),
            devices: devices.clone(),
        };

        let dropped = mux_scanner.dropped.clone();
        let closures = mux_scanner.closures.clone();
        let connection = connection.internal.clone();

        std::thread::spawn(move || {
            let mux_connection = Connection::new(&ConnectionConfig::MuxConnectionConfig(MuxConnectionConfig {
                channel: b'^',
                connection: connection.clone(),
            }));

            mux_connection.open().ok();

            let mut next_ping = std::time::Instant::now(); // first ping immediately
            let default_timeout = std::time::Duration::from_millis(DEFAULT_TIMEOUT as u64);

            loop {
                let now = std::time::Instant::now();

                if now >= next_ping {
                    mux_connection.send_command("{\"ping\":null}".into(), 0, 0);
                    next_ping = now + default_timeout;
                }

                if let Ok(dropped) = dropped.lock() {
                    if *dropped {
                        return;
                    }
                    if updated.swap(false, Ordering::SeqCst) {
                        let devices = devices.lock().unwrap().clone();
                        closures.lock().unwrap().iter().for_each(|(closure, _)| closure(devices.clone()));
                    }
                }

                std::thread::sleep(std::time::Duration::from_millis(100));
            }
        });

        mux_scanner
    }

    pub fn add_closure(&self, closure: Box<dyn Fn(Vec<Device>) + Send>) -> u64 {
        let id = self.closure_counter.fetch_add(1, Ordering::SeqCst);
        self.closures.lock().unwrap().push((closure, id));
        id
    }

    pub fn remove_closure(&self, id: u64) {
        self.closures.lock().unwrap().retain(|(_, closure_id)| closure_id != &id);
    }

    pub fn get_devices(&self) -> Vec<Device> {
        (*self.devices.lock().unwrap()).clone()
    }

    pub fn scan(connection: &Connection, number_of_channels: u32, retries: u32, timeout: u32) -> Vec<Device> {
        let devices = Arc::new(Mutex::new(Vec::new()));

        let closure_id = connection.add_mux_closure(Box::new({
            let connection = connection.internal.clone();
            let devices = devices.clone();

            move |message: MuxMessage| {
                if let Some(device) = Self::parse_mux_message(message, &connection) {
                    Self::update_devices(&devices, &device);
                };
            }
        }));

        let mux_connection = Connection::new(&ConnectionConfig::MuxConnectionConfig(MuxConnectionConfig::new(b'^', connection)));

        mux_connection.open().ok();

        'outer: for _ in 0..(1 + retries) {
            mux_connection.send_command("{\"ping\":null}".into(), 0, 0);

            let start_time = std::time::Instant::now();

            while start_time.elapsed() < std::time::Duration::from_millis(timeout as u64) {
                if devices.lock().unwrap().len() >= number_of_channels as usize {
                    break 'outer;
                }

                std::thread::sleep(std::time::Duration::from_millis(1));
            }
        }

        connection.remove_closure(closure_id);

        let mut devices = devices.lock().unwrap();

        devices.sort_by_key(|device| match &device.connection_config {
            ConnectionConfig::MuxConnectionConfig(config) => config.channel,
            _ => unreachable!(),
        });

        devices.clone()
    }

    fn parse_mux_message(message: MuxMessage, connection: &InternalConnection) -> Option<Device> {
        let response = PingResponse::parse(&message.message)?;

        let connection_config = ConnectionConfig::MuxConnectionConfig(MuxConnectionConfig {
            channel: message.channel,
            connection: connection.clone(),
        });

        Some(Device {
            device_name: response.device_name,
            serial_number: response.serial_number,
            connection_config,
        })
    }

    fn update_devices(devices: &Arc<Mutex<Vec<Device>>>, device: &Device) -> bool {
        let mut devices = devices.lock().unwrap();

        if devices.contains(device) {
            return false;
        }

        let channel = match &device.connection_config {
            ConnectionConfig::MuxConnectionConfig(cfg) => cfg.channel,
            _ => unreachable!(),
        };

        devices.retain(|device| match &device.connection_config {
            ConnectionConfig::MuxConnectionConfig(config) => config.channel != channel,
            _ => unreachable!(),
        });

        devices.push(device.clone());

        true
    }
}

impl Drop for MuxScanner<'_> {
    fn drop(&mut self) {
        *self.dropped.lock().unwrap() = true;
        self.connection.remove_closure(self.mux_closure_id);
    }
}
