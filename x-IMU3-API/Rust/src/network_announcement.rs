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
pub struct NetworkAnnouncementMessage {
    pub device_name: String,
    pub serial_number: String,
    pub rssi: i32,
    pub battery: i32,
    pub status: ChargingStatus,
    pub tcp_connection_info: TcpConnectionInfo,
    pub udp_connection_info: UdpConnectionInfo,
    pub(crate) expiry: u128,
}

impl fmt::Display for NetworkAnnouncementMessage {
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

impl PartialEq for NetworkAnnouncementMessage {
    fn eq(&self, other: &Self) -> bool { // comparison must not include rssi, battery, power, expiry
        self.device_name == other.device_name && self.serial_number == other.serial_number
            && self.tcp_connection_info.to_string() == other.tcp_connection_info.to_string()
            && self.udp_connection_info.to_string() == other.udp_connection_info.to_string()
    }
}

pub struct NetworkAnnouncement {
    dropped: Arc<Mutex<bool>>,
    closure_counter: u64,
    closures: Arc<Mutex<Vec<(Box<dyn Fn(NetworkAnnouncementMessage) + Send>, u64)>>>,
    messages: Arc<Mutex<Vec<NetworkAnnouncementMessage>>>,
}

impl NetworkAnnouncement {
    pub fn new() -> NetworkAnnouncement {
        let network_announcement = NetworkAnnouncement {
            dropped: Arc::new(Mutex::new(false)),
            closure_counter: 0,
            closures: Arc::new(Mutex::new(Vec::new())),
            messages: Arc::new(Mutex::new(Vec::new())),
        };

        let dropped = network_announcement.dropped.clone();
        let closures = network_announcement.closures.clone();
        let messages = network_announcement.messages.clone();

        std::thread::spawn(move || {
            if let Ok(socket) = UdpSocket::bind("0.0.0.0:10000") {
                socket.set_read_timeout(Some(std::time::Duration::from_millis(100))).ok();

                loop {
                    let mut buffer = [0_u8; 1024];

                    let mut message = None;

                    if let Ok((number_of_bytes, _)) = socket.recv_from(&mut buffer) {
                        message = NetworkAnnouncement::parse_json(&buffer[..number_of_bytes]);
                    }

                    if let Some(message) = message.clone() {
                        if let Ok(mut messages) = messages.lock() {
                            if let Some(index) = messages.iter().position(|element| element == &message) {
                                messages[index] = message; // overwrite with new rssi, battery, power, expiry
                            } else {
                                messages.push(message);
                            }
                        }
                    }

                    messages.lock().unwrap().retain(|device| device.expiry > SystemTime::now().duration_since(UNIX_EPOCH).unwrap().as_millis());

                    if let Ok(dropped) = dropped.lock() {
                        if *dropped {
                            return;
                        }
                        if let Some(message) = message {
                            closures.lock().unwrap().iter().for_each(|(closure, _)| closure(message.clone()));
                        }
                    }
                }
            }
        });

        network_announcement
    }

    fn parse_json(json: &[u8]) -> Option<NetworkAnnouncementMessage> {
        if let Ok(json) = std::str::from_utf8(json) {
            #[derive(Deserialize)]
            struct Object {
                name: String,
                sn: String,
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
                    let device = NetworkAnnouncementMessage {
                        device_name: object.name,
                        serial_number: object.sn,
                        rssi: object.rssi,
                        battery: object.battery,
                        status: ChargingStatus::from(object.status),
                        tcp_connection_info,
                        udp_connection_info,
                        expiry: SystemTime::now().duration_since(UNIX_EPOCH).unwrap().as_millis() + 2000, // expire after 2 seconds
                    };
                    return Some(device);
                }
            }
        }
        None
    }

    pub fn add_closure(&mut self, closure: Box<dyn Fn(NetworkAnnouncementMessage) + Send>) -> u64 {
        let id = self.closure_counter;
        self.closure_counter += 1;
        self.closures.lock().unwrap().push((closure, id));
        id
    }

    pub fn remove_closure(&mut self, closure_id: u64) {
        self.closures.lock().unwrap().retain(|(_, id)| id != &closure_id);
    }

    pub fn get_messages(&mut self) -> Vec<NetworkAnnouncementMessage> {
        (*self.messages.lock().unwrap()).clone()
    }

    pub fn get_messages_after_short_delay(&mut self) -> Vec<NetworkAnnouncementMessage> {
        std::thread::sleep(std::time::Duration::from_secs(2));
        self.get_messages()
    }
}

impl Drop for NetworkAnnouncement {
    fn drop(&mut self) {
        *self.dropped.lock().unwrap() = true;
    }
}
