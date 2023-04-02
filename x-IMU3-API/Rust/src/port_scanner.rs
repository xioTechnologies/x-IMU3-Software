use crossbeam::channel::Sender;
use std::fmt;
use std::ops::Drop;
use std::sync::{Arc, Mutex};
use crate::connection::*;
use crate::connection_info::*;
use crate::connection_type::*;

#[derive(Clone)]
pub struct Device {
    pub device_name: String,
    pub serial_number: String,
    pub connection_info: ConnectionInfo,
}

impl fmt::Display for Device {
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        write!(formatter, "{}, {}, {}",
               self.device_name,
               self.serial_number,
               self.connection_info.to_string())
    }
}

pub struct PortScanner {
    dropped: Arc<Mutex<bool>>,
    devices: Arc<Mutex<Vec<Device>>>,
}

impl PortScanner {
    pub fn new(closure: Box<dyn Fn(Vec<Device>) + Send>) -> PortScanner {
        let port_scanner = PortScanner {
            dropped: Arc::new(Mutex::new(false)),
            devices: Arc::new(Mutex::new(Vec::new())),
        };

        let dropped = port_scanner.dropped.clone();
        let devices = port_scanner.devices.clone();

        std::thread::spawn(move || {
            let mut handled_port_names = Vec::new();
            let (sender, receiver) = crossbeam::channel::unbounded();

            loop {
                for port_name in PortScanner::get_port_names() {
                    if handled_port_names.contains(&port_name) {
                        continue;
                    }
                    handled_port_names.push(port_name.clone());

                    let dropped = dropped.clone();
                    let devices = devices.clone();
                    let sender = sender.clone();

                    std::thread::spawn(move || loop {
                        if devices.lock().unwrap().iter().any(|device| device.connection_info.to_string().contains(&port_name)) == false
                        {
                            PortScanner::ping_port(&port_name, devices.clone(), sender.clone());
                        } else if PortScanner::get_port_names().contains(&port_name) == false {
                            devices.lock().unwrap().retain(|device| device.connection_info.to_string().contains(&port_name) == false);
                            sender.send(()).ok();
                        }

                        if *dropped.lock().unwrap() {
                            return;
                        }

                        std::thread::sleep(std::time::Duration::from_millis(1000));
                    });
                }

                if let Ok(dropped) = dropped.lock() {
                    if *dropped {
                        return;
                    }
                    if let Ok(()) = receiver.try_recv() {
                        let devices = devices.lock().unwrap().clone();
                        closure(devices); // argument must not include lock() because this could cause deadlock
                    }
                }

                std::thread::sleep(std::time::Duration::from_millis(100));
            }
        });

        port_scanner
    }

    fn ping_port(port_name: &str, devices: Arc<Mutex<Vec<Device>>>, sender: Sender<()>) {
        let connection_info = SerialConnectionInfo {
            port_name: port_name.to_string(),
            baud_rate: 115200,
            rts_cts_enabled: false,
        };

        let mut connection = Connection::new(&ConnectionInfo::SerialConnectionInfo(connection_info.clone()));

        if let Ok(()) = connection.open() {
            if let Ok(ping_response) = connection.ping() {
                let device = Device {
                    device_name: ping_response.device_name,
                    serial_number: ping_response.serial_number,
                    connection_info: match ping_response.interface.as_str() {
                        "USB" => ConnectionInfo::UsbConnectionInfo(UsbConnectionInfo { port_name: connection_info.port_name }),
                        "Serial" => ConnectionInfo::SerialConnectionInfo(connection_info),
                        "Bluetooth" => ConnectionInfo::BluetoothConnectionInfo(BluetoothConnectionInfo { port_name: connection_info.port_name }),
                        _ => ConnectionInfo::SerialConnectionInfo(connection_info),
                    },
                };

                devices.lock().unwrap().push(device);
                sender.send(()).ok();
            }
            connection.close();
        }
    }

    pub fn get_devices(&mut self) -> Vec<Device> {
        (*self.devices.lock().unwrap()).clone()
    }

    pub fn scan() -> Vec<Device> {
        let devices = Arc::new(Mutex::new(Vec::new()));
        let (sender, receiver) = crossbeam::channel::unbounded();

        for port_name in PortScanner::get_port_names() {
            let devices = devices.clone();
            let sender = sender.clone();

            std::thread::spawn(move || {
                PortScanner::ping_port(&port_name, devices, sender);
            });
        }

        drop(sender);
        while let Ok(_) = receiver.recv() {} // wait for all senders to be dropped

        let devices = devices.lock().unwrap().clone();
        devices
    }

    pub fn scan_filter(connection_type: ConnectionType) -> Vec<Device> {
        let mut devices = PortScanner::scan();

        devices.retain(|device| {
            match device.connection_info {
                ConnectionInfo::UsbConnectionInfo(_) => if let ConnectionType::Usb = connection_type { return true; },
                ConnectionInfo::SerialConnectionInfo(_) => if let ConnectionType::Serial = connection_type { return true; },
                ConnectionInfo::BluetoothConnectionInfo(_) => if let ConnectionType::Bluetooth = connection_type { return true; },
                _ => {}
            }
            false
        });
        devices
    }

    pub fn get_port_names() -> Vec<String> {
        if let Ok(serial_port_infos) = serialport::available_ports() {
            let mut port_names: Vec<String> = serial_port_infos.iter().map(|info| info.port_name.to_owned()).collect();

            port_names.retain(|port_name| port_name.contains("/dev/cu") == false);

            return port_names;
        }
        Vec::new()
    }
}

impl Drop for PortScanner {
    fn drop(&mut self) {
        *self.dropped.lock().unwrap() = true;
    }
}
