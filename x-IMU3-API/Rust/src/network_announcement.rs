use crate::charging_status::*;
use crate::connection_info::*;
use serde::Deserialize;
use serde_json;
use std::fmt;
use std::net::{Ipv4Addr, UdpSocket};
use std::ops::Drop;
use std::sync::atomic::{AtomicU64, Ordering};
use std::sync::{Arc, Mutex};

#[derive(Clone)]
pub struct NetworkAnnouncementMessage {
    pub device_name: String,
    pub serial_number: String,
    pub ip_address: Ipv4Addr,
    pub tcp_port: u16,
    pub udp_send: u16,
    pub udp_receive: u16,
    pub rssi: i32,
    pub battery: i32,
    pub charging_status: ChargingStatus,
    pub(crate) time: std::time::Instant,
}

impl From<&NetworkAnnouncementMessage> for TcpConnectionInfo {
    fn from(message: &NetworkAnnouncementMessage) -> Self {
        TcpConnectionInfo {
            ip_address: message.ip_address,
            port: message.tcp_port,
        }
    }
}

impl From<&NetworkAnnouncementMessage> for UdpConnectionInfo {
    fn from(message: &NetworkAnnouncementMessage) -> Self {
        UdpConnectionInfo {
            ip_address: message.ip_address,
            send_port: message.udp_receive, // swap send and receive ports
            receive_port: message.udp_send,
        }
    }
}

impl fmt::Display for NetworkAnnouncementMessage {
    #[rustfmt::skip]
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        write!(
            formatter,
            "{}, {}, {}, {}, {}, {}, {}%, {}%, {}",
            self.device_name,
            self.serial_number,
            self.ip_address,
            self.tcp_port,
            self.udp_send,
            self.udp_receive,
            self.rssi,
            self.battery,
            self.charging_status
        )
    }
}

impl PartialEq for NetworkAnnouncementMessage {
    #[rustfmt::skip]
    fn eq(&self, other: &Self) -> bool { // comparison must not include rssi, battery, charging_status, expiry
        self.device_name == other.device_name
            && self.serial_number == other.serial_number
            && self.ip_address == other.ip_address
            && self.tcp_port == other.tcp_port
            && self.udp_send == other.udp_send
            && self.udp_receive == other.udp_receive
    }
}

pub struct NetworkAnnouncement {
    dropped: Arc<Mutex<bool>>,
    closure_counter: AtomicU64,
    closures: Arc<Mutex<Vec<(Box<dyn Fn(NetworkAnnouncementMessage) + Send>, u64)>>>,
    messages: Arc<Mutex<Vec<NetworkAnnouncementMessage>>>,
}

impl NetworkAnnouncement {
    pub fn new() -> std::io::Result<Self> {
        let socket = UdpSocket::bind("0.0.0.0:10000")?;

        socket.set_nonblocking(true)?;

        let network_announcement = Self {
            dropped: Arc::new(Mutex::new(false)),
            closure_counter: AtomicU64::new(0),
            closures: Arc::new(Mutex::new(Vec::new())),
            messages: Arc::new(Mutex::new(Vec::new())),
        };

        let dropped = network_announcement.dropped.clone();
        let closures = network_announcement.closures.clone();
        let messages = network_announcement.messages.clone();

        std::thread::spawn(move || {
            loop {
                let mut buffer = [0_u8; 1024];

                let message = {
                    match socket.recv_from(&mut buffer) {
                        Ok((number_of_bytes, _)) => Self::parse(&buffer[..number_of_bytes]),
                        Err(_) => {
                            std::thread::sleep(std::time::Duration::from_millis(1));
                            None
                        }
                    }
                };

                if let Some(message) = &message {
                    if let Ok(mut messages) = messages.lock() {
                        if let Some(index) = messages.iter().position(|element| element == message) {
                            messages[index] = message.clone(); // overwrite with new rssi, battery, power, expiry
                        } else {
                            messages.push(message.clone());
                        }
                    }
                }

                messages.lock().unwrap().retain(|device| device.time.elapsed() < std::time::Duration::from_secs(2)); // discard messages older than 2 seconds

                if let Ok(dropped) = dropped.lock() {
                    if *dropped {
                        return;
                    }
                    if let Some(message) = message {
                        closures.lock().unwrap().iter().for_each(|(closure, _)| closure(message.clone()));
                    }
                }
            }
        });

        Ok(network_announcement)
    }

    fn parse(json: &[u8]) -> Option<NetworkAnnouncementMessage> {
        let json = std::str::from_utf8(json).ok()?;

        #[derive(Deserialize)]
        struct Object {
            name: String,
            sn: String,
            ip: String,
            port: Option<u16>,
            send: Option<u16>,
            receive: Option<u16>,
            rssi: Option<i32>,
            battery: Option<i32>,
            status: Option<i32>,
        }

        let object: Object = serde_json::from_str(json).ok()?;
        let ip_address = object.ip.parse::<Ipv4Addr>().ok()?;

        Some(NetworkAnnouncementMessage {
            device_name: object.name,
            serial_number: object.sn,
            ip_address,
            tcp_port: object.port.unwrap_or(0),
            udp_send: object.send.unwrap_or(0),
            udp_receive: object.receive.unwrap_or(0),
            rssi: object.rssi.unwrap_or(-1),
            battery: object.battery.unwrap_or(-1),
            charging_status: ChargingStatus::from(object.status.unwrap_or(-1)),
            time: std::time::Instant::now(),
        })
    }

    pub fn add_closure(&self, closure: Box<dyn Fn(NetworkAnnouncementMessage) + Send>) -> u64 {
        let id = self.closure_counter.fetch_add(1, Ordering::SeqCst);
        self.closures.lock().unwrap().push((closure, id));
        id
    }

    pub fn remove_closure(&self, closure_id: u64) {
        self.closures.lock().unwrap().retain(|(_, id)| id != &closure_id);
    }

    pub fn get_messages(&self) -> Vec<NetworkAnnouncementMessage> {
        (*self.messages.lock().unwrap()).clone()
    }

    pub fn get_messages_after_short_delay(&self) -> Vec<NetworkAnnouncementMessage> {
        std::thread::sleep(std::time::Duration::from_secs(2));
        self.get_messages()
    }
}

impl Drop for NetworkAnnouncement {
    fn drop(&mut self) {
        *self.dropped.lock().unwrap() = true;
    }
}
