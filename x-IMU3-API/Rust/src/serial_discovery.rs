use crossbeam::channel::Sender;
use std::fmt;
use std::ops::Drop;
use std::sync::{Arc, Mutex};
use crate::connection::*;
use crate::connection_info::*;
use crate::connection_type::*;

#[derive(Clone)]
pub struct DiscoveredSerialDevice {
    pub device_name: String,
    pub serial_number: String,
    pub connection_info: ConnectionInfo,
}

impl fmt::Display for DiscoveredSerialDevice {
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        write!(formatter, "{} - {}, {}",
               self.device_name,
               self.serial_number,
               self.connection_info.to_string())
    }
}

pub struct SerialDiscovery {
    dropped: Arc<Mutex<bool>>,
    devices: Arc<Mutex<Vec<DiscoveredSerialDevice>>>,
}

impl SerialDiscovery {
    pub fn new(closure: Box<dyn Fn(Vec<DiscoveredSerialDevice>) + Send>) -> SerialDiscovery {
        let discovery = SerialDiscovery {
            dropped: Arc::new(Mutex::new(false)),
            devices: Arc::new(Mutex::new(Vec::new())),
        };

        let dropped = discovery.dropped.clone();
        let devices = discovery.devices.clone();

        let mut active_ports = Vec::new();
        let (sender, receiver) = crossbeam::channel::unbounded();

        std::thread::spawn(move || loop {
            for available_port in SerialDiscovery::get_available_ports() {
                if active_ports.contains(&available_port) == false {
                    active_ports.push(available_port.clone());

                    SerialDiscovery::ping_port(available_port, devices.clone(), dropped.clone(), sender.clone());
                }
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
        });

        discovery
    }

    fn ping_port(port_name: String, devices: Arc<Mutex<Vec<DiscoveredSerialDevice>>>, dropped: Arc<Mutex<bool>>, sender: Sender<()>) {
        std::thread::spawn(move || loop {
            if devices.lock().unwrap().iter().any(|device| device.connection_info.to_string().contains(port_name.as_str())) == false
            {
                let connection_info = SerialConnectionInfo {
                    port_name: port_name.clone(),
                    baud_rate: 115200,
                    rts_cts_enabled: false,
                };

                let mut connection = Connection::new(ConnectionInfo::SerialConnectionInfo(connection_info.clone()));

                if let Ok(()) = connection.open() {
                    if let Ok(ping_response) = connection.ping() {
                        let device = DiscoveredSerialDevice {
                            device_name: ping_response.device_name,
                            serial_number: ping_response.serial_number,
                            connection_info: match ping_response.interface.as_str() {
                                "USB" => ConnectionInfo::UsbConnectionInfo(UsbConnectionInfo { port_name: connection_info.port_name }),
                                "Serial" => ConnectionInfo::SerialConnectionInfo(connection_info.clone()),
                                "Bluetooth" => ConnectionInfo::BluetoothConnectionInfo(BluetoothConnectionInfo { port_name: connection_info.port_name }),
                                _ => ConnectionInfo::SerialConnectionInfo(connection_info),
                            },
                        };

                        devices.lock().unwrap().push(device);
                        sender.send(()).ok();
                    }
                    connection.close();
                }
            } else if SerialDiscovery::get_available_ports().contains(&port_name) == false {
                devices.lock().unwrap().retain(|device| device.connection_info.to_string().contains(port_name.as_str()) == false);
                sender.send(()).ok();
            }

            std::thread::sleep(std::time::Duration::from_millis(1000));

            if *dropped.lock().unwrap() {
                return;
            }
        });
    }

    pub fn get_devices(&mut self) -> Vec<DiscoveredSerialDevice> {
        (*self.devices.lock().unwrap()).clone()
    }

    pub fn scan(milliseconds: u32) -> Vec<DiscoveredSerialDevice> {
        let mut discovery = SerialDiscovery::new(Box::new(|_| {}));

        std::thread::sleep(std::time::Duration::from_millis(milliseconds as u64));

        discovery.get_devices()
    }

    pub fn scan_filter(milliseconds: u32, connection_type: ConnectionType) -> Vec<DiscoveredSerialDevice> {
        let mut devices = SerialDiscovery::scan(milliseconds);

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

    pub fn get_available_ports() -> Vec<String> {
        if let Ok(serial_port_infos) = serialport::available_ports() {
            return serial_port_infos.iter().map(|info| info.port_name.to_owned()).collect();
        }
        Vec::new()
    }
}

impl Drop for SerialDiscovery {
    fn drop(&mut self) {
        *self.dropped.lock().unwrap() = true;
    }
}
