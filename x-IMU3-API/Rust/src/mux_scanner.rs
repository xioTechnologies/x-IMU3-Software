use crate::connection::*;
use crate::connection_config::*;
use crate::device::*;
use crate::mux_message::*;
use crate::ping_response::*;
use std::ops::Drop;
use std::sync::{Arc, Mutex};
pub struct MuxScanner {
    dropped: Arc<Mutex<bool>>,
    devices: Arc<Mutex<Vec<Device>>>,
}

impl MuxScanner {
    pub fn new(connection: &Connection, closure: Box<dyn Fn(Vec<Device>) + Send>) -> Self {
        let mux_scanner = Self {
            dropped: Arc::new(Mutex::new(false)),
            devices: Arc::new(Mutex::new(Vec::new())),
        };

        let dropped = mux_scanner.dropped.clone();
        let devices = mux_scanner.devices.clone();

        std::thread::spawn(move || {
            let (sender, receiver) = crossbeam::channel::unbounded();

            loop {
                sender.send(()).ok(); // trigger closure if devices updated

                if let Ok(dropped) = dropped.lock() {
                    if *dropped {
                        return;
                    }
                    if let Ok(_) = receiver.try_recv() {
                        let devices = devices.lock().unwrap().clone();
                        closure(devices); // argument must not include lock() because this could cause deadlock
                    }
                }

                std::thread::sleep(std::time::Duration::from_millis(100));
            }
        });

        mux_scanner
    }

    pub fn get_devices(&self) -> Vec<Device> {
        (*self.devices.lock().unwrap()).clone()
    }

    pub fn scan(connection: &Connection) -> Vec<Device> {
        let devices = Arc::new(Mutex::new(Vec::new()));

        let closure_id = connection.add_mux_closure(Box::new({
            let devices = devices.clone();
            let connection = connection.internal.clone(); // TODO: use vector of channels instead

            move |message: MuxMessage| {
                let response = match PingResponse::parse(&message.message) {
                    Some(response) => response,
                    None => return,
                };

                let connection_config = ConnectionConfig::MuxConnectionConfig(MuxConnectionConfig {
                    channel: message.channel,
                    connection: connection.clone(),
                });

                // TODO: don't push is already present

                devices.lock().unwrap().push(Device {
                    device_name: response.device_name,
                    serial_number: response.serial_number,
                    connection_config,
                });
            }
        }));

        let mux_connection = Connection::new(&ConnectionConfig::MuxConnectionConfig(MuxConnectionConfig::new(b'^', connection)));

        if mux_connection.open().is_err() {
            return vec![];
        }

        mux_connection.ping();

        connection.remove_closure(closure_id);

        let devices = devices.lock().unwrap().clone();

        // TODO: sort devices by channel

        devices
    }
}

impl Drop for MuxScanner {
    fn drop(&mut self) {
        *self.dropped.lock().unwrap() = true;
    }
}
