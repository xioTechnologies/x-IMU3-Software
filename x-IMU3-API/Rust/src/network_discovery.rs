use serde::Deserialize;
use serde_json;
use std::fmt;
use std::net::{Ipv4Addr, UdpSocket};
use std::ops::Drop;
use std::sync::{Arc, Mutex};
use std::time::{SystemTime, UNIX_EPOCH};
use crate::charging_status::*;
use crate::connection_info::*;

#[derive(Clone)]
pub struct DiscoveredNetworkDevice {
    pub device_name: String,
    pub serial_number: String,
    pub rssi: i32,
    pub battery: i32,
    pub status: ChargingStatus,
    pub tcp_connection_info: TcpConnectionInfo,
    pub udp_connection_info: UdpConnectionInfo,
    pub(crate) expiry: u128,
}

impl fmt::Display for DiscoveredNetworkDevice {
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        write!(formatter, "{} - {}, RSSI: {}%, Battery: {}%, {}, {}, {}",
               self.device_name,
               self.serial_number,
               self.rssi,
               self.battery,
               self.status,
               self.tcp_connection_info,
               self.udp_connection_info)
    }
}

impl PartialEq for DiscoveredNetworkDevice {
    fn eq(&self, other: &Self) -> bool { // comparison must not include rssi, battery, power, expiry
        self.device_name == other.device_name && self.serial_number == other.serial_number
            && self.tcp_connection_info.to_string() == other.tcp_connection_info.to_string()
            && self.udp_connection_info.to_string() == other.udp_connection_info.to_string()
    }
}

pub struct NetworkDiscovery {
    dropped: Arc<Mutex<bool>>,
    devices: Arc<Mutex<Vec<DiscoveredNetworkDevice>>>,
}

impl NetworkDiscovery {
    pub fn new(closure: Box<dyn Fn(Vec<DiscoveredNetworkDevice>) + Send>) -> NetworkDiscovery {
        let discovery = NetworkDiscovery {
            dropped: Arc::new(Mutex::new(false)),
            devices: Arc::new(Mutex::new(Vec::new())),
        };

        let dropped = discovery.dropped.clone();
        let devices = discovery.devices.clone();

        std::thread::spawn(move || {
            if let Ok(socket) = UdpSocket::bind("0.0.0.0:10000") {
                socket.set_read_timeout(Some(std::time::Duration::from_millis(100))).ok();

                loop {
                    let mut buffer = [0_u8; 1024];

                    let mut closure_pending = false;

                    if let Ok((number_of_bytes, _)) = socket.recv_from(&mut buffer) {
                        if let Ok(new_device) = NetworkDiscovery::parse_json(&buffer[..number_of_bytes]) {
                            if let Ok(mut devices) = devices.lock() {
                                if let Some(index) = devices.iter().position(|device| device == &new_device) {
                                    devices[index] = new_device; // overwrite with new rssi, battery, power, expiry
                                } else {
                                    devices.push(new_device);
                                    closure_pending = true;
                                }
                            }
                        }
                    }

                    let previous_length = devices.lock().unwrap().len();
                    devices.lock().unwrap().retain(|device| device.expiry > SystemTime::now().duration_since(UNIX_EPOCH).unwrap().as_millis());
                    closure_pending |= previous_length != devices.lock().unwrap().len();

                    if let Ok(dropped) = dropped.lock() {
                        if *dropped {
                            return;
                        }
                        if closure_pending == true {
                            let devices = devices.lock().unwrap().clone();
                            closure(devices); // argument must not include lock() because this could cause deadlock
                        }
                    }
                }
            }
        });

        discovery
    }

    fn parse_json(json: &[u8]) -> Result<DiscoveredNetworkDevice, ()> {
        if let Ok(json) = std::str::from_utf8(json) {
            #[derive(Deserialize)]
            struct Object {
                name: String,
                serial: String,
                ip: String,
                port: u16,
                send: u16,
                receive: u16,
                rssi: i32,
                battery: i32,
                status: i32,
            }
            if let Ok(object) = serde_json::from_str::<Object>(json) {
                if let Ok(ip_address) = object.ip.parse::<Ipv4Addr>() {
                    let tcp_connection_info = TcpConnectionInfo {
                        ip_address,
                        port: object.port,
                    };
                    let udp_connection_info = UdpConnectionInfo {
                        ip_address,
                        send_port: object.receive,
                        receive_port: object.send,
                    };
                    let device = DiscoveredNetworkDevice {
                        device_name: object.name,
                        serial_number: object.serial,
                        rssi: object.rssi,
                        battery: object.battery,
                        status: ChargingStatus::from(object.status),
                        tcp_connection_info,
                        udp_connection_info,
                        expiry: SystemTime::now().duration_since(UNIX_EPOCH).unwrap().as_millis() + 2000, // expire after 2 seconds
                    };
                    return Ok(device);
                }
            }
        }
        return Err(());
    }

    pub fn get_devices(&mut self) -> Vec<DiscoveredNetworkDevice> {
        (*self.devices.lock().unwrap()).clone()
    }

    pub fn scan(milliseconds: u32) -> Vec<DiscoveredNetworkDevice> {
        let mut discovery = NetworkDiscovery::new(Box::new(|_| {}));

        std::thread::sleep(std::time::Duration::from_millis(milliseconds as u64));

        discovery.get_devices()
    }
}

impl Drop for NetworkDiscovery {
    fn drop(&mut self) {
        *self.dropped.lock().unwrap() = true;
    }
}
