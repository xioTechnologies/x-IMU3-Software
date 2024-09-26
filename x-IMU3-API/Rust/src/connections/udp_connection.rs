use crossbeam::channel::Sender;
use std::net::{IpAddr, SocketAddr, UdpSocket};
use std::sync::{Arc, Mutex};
use crate::connection_info::*;
use crate::connection_status::*;
use crate::connections::*;
use crate::decoder::*;
use crate::dispatcher::*;

pub struct UdpConnection {
    connection_info: UdpConnectionInfo,
    connection_status: Arc<Mutex<ConnectionStatus>>,
    decoder: Arc<Mutex<Decoder>>,
    close_sender: Option<Sender<()>>,
    write_sender: Option<Sender<String>>,
}

impl UdpConnection {
    pub fn new(connection_info: &UdpConnectionInfo) -> UdpConnection {
        UdpConnection {
            connection_info: connection_info.clone(),
            connection_status: Arc::new(Mutex::new(ConnectionStatus::Disconnected)),
            decoder: Arc::new(Mutex::new(Decoder::new())),
            close_sender: None,
            write_sender: None,
        }
    }
}

impl GenericConnection for UdpConnection {
    fn open(&mut self) -> std::io::Result<()> {
        let socket = UdpSocket::bind(SocketAddr::new("0.0.0.0".parse::<IpAddr>().unwrap(), self.connection_info.receive_port))?;

        socket.set_read_timeout(Some(std::time::Duration::from_millis(1))).ok();

        let socket_address = SocketAddr::new(IpAddr::V4(self.connection_info.ip_address), self.connection_info.send_port);

        *self.connection_status.lock().unwrap() = ConnectionStatus::Connected;
        self.decoder.lock().unwrap().dispatcher.sender.send(DispatcherData::ConnectionStatus(ConnectionStatus::Connected)).ok();

        let connection_status = self.connection_status.clone();

        let decoder = self.decoder.clone();

        let (close_sender, close_receiver) = crossbeam::channel::bounded(1);
        let (write_sender, write_receiver) = crossbeam::channel::unbounded();

        self.close_sender = Some(close_sender);
        self.write_sender = Some(write_sender);

        std::thread::spawn(move || {
            let mut buffer: Vec<u8> = vec![0; 2048];

            while let Err(_) = close_receiver.try_recv() {
                if let Ok((number_of_bytes, _)) = socket.recv_from(&mut buffer) {
                    decoder.lock().unwrap().process_bytes(&buffer.as_mut_slice()[..number_of_bytes]);
                }
                while let Some(terminated_json) = write_receiver.try_recv().iter().next() {
                    socket.send_to(terminated_json.as_bytes(), socket_address).ok();
                }
            }

            *connection_status.lock().unwrap() = ConnectionStatus::Disconnected;
            decoder.lock().unwrap().dispatcher.sender.send(DispatcherData::ConnectionStatus(ConnectionStatus::Disconnected)).ok();
        });

        Ok(())
    }

    fn close(&self) {
        if let Some(close_sender) = &self.close_sender {
            close_sender.send(()).ok();
        }
    }

    fn get_info(&self) -> ConnectionInfo {
        ConnectionInfo::UdpConnectionInfo(self.connection_info.clone())
    }

    fn get_status(&self) -> ConnectionStatus {
        *self.connection_status.lock().unwrap()
    }

    fn get_decoder(&self) -> Arc<Mutex<Decoder>> {
        self.decoder.clone()
    }

    fn get_write_sender(&self) -> Option<Sender<String>> {
        self.write_sender.clone()
    }
}
