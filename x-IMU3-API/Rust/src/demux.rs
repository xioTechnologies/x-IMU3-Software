use crate::connection::*;
use crate::mux_message::*;
use serde_json;
use std::collections::HashMap;
use std::net::{IpAddr, Ipv4Addr, SocketAddr, UdpSocket};
use std::ops::Drop;
use std::sync::{Arc, Mutex};

struct DeviceEmulator {
    udp_socket: UdpSocket,
    udp_socket_address: SocketAddr,
    pub network_announcement: String,
}

impl DeviceEmulator {
    pub fn new(channel: u8) -> std::io::Result<Self> {
        let ip_address = IpAddr::V4(Ipv4Addr::LOCALHOST);

        let tcp_port = 7000 + channel as u16;

        // TODO: TCP

        let udp_receive = 9000 + channel as u16;
        let udp_send = 8000 + channel as u16;

        let udp_socket = UdpSocket::bind(SocketAddr::new("0.0.0.0".parse::<IpAddr>().unwrap(), udp_receive))?;

        udp_socket.set_read_timeout(Some(std::time::Duration::from_millis(1))).ok();

        Ok(Self {
            udp_socket,
            udp_socket_address: SocketAddr::new(ip_address, udp_send),
            network_announcement: serde_json::json!({
                "name": format!("Demux 0x{:02X}", channel),
                "sn": "",
                "ip": ip_address.to_string(),
                "port": tcp_port,
                "send": udp_send,
                "receive": udp_receive,
            })
            .to_string(),
        })
    }

    pub fn udp_receive(&self, buffer: &mut [u8]) -> std::io::Result<(usize, SocketAddr)> {
        self.udp_socket.recv_from(buffer) // TODO: each receive will block for read_timeout
    }

    pub fn udp_send(&self, data: &[u8]) {
        self.udp_socket.send_to(data, self.udp_socket_address).ok();
    }
}

pub struct Demux<'a> {
    dropped: Arc<Mutex<bool>>,
    connection: &'a Connection,
    closure_id: u64,
}

impl<'a> Demux<'a> {
    pub fn new(connection: &'a Connection, channels: Vec<u8>) -> std::io::Result<Self> {
        let devices: HashMap<u8, DeviceEmulator> = channels.into_iter().map(|channel| Ok((channel, DeviceEmulator::new(channel)?))).collect::<std::io::Result<_>>()?;

        let network_announcement_socket = UdpSocket::bind("0.0.0.0:0")?;

        let (closure_sender, closure_receiver) = crossbeam::channel::unbounded();

        let closure_id = connection.add_mux_closure(Box::new(move |mux_message| {
            closure_sender.send(mux_message).ok();
        }));

        let demux = Self {
            dropped: Arc::new(Mutex::new(false)),
            connection,
            closure_id,
        };

        let network_announcement_messages: Vec<String> = devices.values().map(|device| device.network_announcement.clone()).collect();

        let dropped = demux.dropped.clone();

        std::thread::spawn(move || loop {
            for message in &network_announcement_messages {
                network_announcement_socket.send_to(message.as_bytes(), SocketAddr::new(IpAddr::V4(Ipv4Addr::LOCALHOST), 10000)).ok();
            }

            std::thread::sleep(std::time::Duration::from_secs(1));

            if *dropped.lock().unwrap() {
                return;
            }
        });

        let mut buffer = [0u8; 2048];

        let connection = connection.internal.clone();

        let dropped = demux.dropped.clone();

        std::thread::spawn(move || loop {
            while let Ok(mux_message) = closure_receiver.try_recv() {
                if let Some(device) = devices.get(&mux_message.channel) {
                    device.udp_send(mux_message.message.as_slice());
                }
            }

            for (channel, device) in devices.iter() {
                if let Ok((number_of_bytes, _)) = device.udp_receive(&mut buffer) {
                    let mux_message = MuxMessage {
                        channel: *channel,
                        message: buffer[..number_of_bytes].to_vec(),
                    };
                    if let Some(write_sender) = connection.lock().unwrap().get_write_sender() {
                        write_sender.send(mux_message.into_bytes()).ok();
                    }
                }
            }

            if *dropped.lock().unwrap() {
                return;
            }

            // TODO: avoid thread spinning if udp_receive does not block
        });

        Ok(demux)
    }
}

impl Drop for Demux<'_> {
    fn drop(&mut self) {
        *self.dropped.lock().unwrap() = true;
        self.connection.remove_closure(self.closure_id);
    }
}
