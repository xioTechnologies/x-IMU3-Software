use crate::connection_info::*;
use crate::connections::*;
use crate::decoder::*;
use crossbeam::channel::Sender;
use std::net::{IpAddr, SocketAddr, UdpSocket};
use std::sync::{Arc, Mutex};

pub struct UdpConnection {
    connection_info: UdpConnectionInfo,
    decoder: Arc<Mutex<Decoder>>,
    close_sender: Option<Sender<()>>,
    write_sender: Option<Sender<Vec<u8>>>,
}

impl UdpConnection {
    pub fn new(connection_info: &UdpConnectionInfo) -> Self {
        Self {
            connection_info: connection_info.clone(),
            decoder: Arc::new(Mutex::new(Decoder::new())),
            close_sender: None,
            write_sender: None,
        }
    }
}

impl GenericConnection for UdpConnection {
    fn open(&mut self) -> std::io::Result<()> {
        let socket = UdpSocket::bind(SocketAddr::new("0.0.0.0".parse::<IpAddr>().unwrap(), self.connection_info.receive_port))?;

        socket.set_nonblocking(true)?;

        let socket_address = SocketAddr::new(IpAddr::V4(self.connection_info.ip_address), self.connection_info.send_port);

        let decoder = self.decoder.clone();

        let (close_sender, close_receiver) = crossbeam::channel::bounded(1);
        let (write_sender, write_receiver) = crossbeam::channel::unbounded();

        self.close_sender = Some(close_sender);
        self.write_sender = Some(write_sender);

        std::thread::spawn(move || {
            let mut buffer = [0u8; 2048];

            while close_receiver.try_recv().is_err() {
                match socket.recv_from(&mut buffer) {
                    Ok((number_of_bytes, _)) => {
                        decoder.lock().unwrap().process_bytes(&buffer[..number_of_bytes]);
                    }
                    Err(error) if error.kind() == std::io::ErrorKind::WouldBlock => {
                        std::thread::sleep(std::time::Duration::from_millis(1));
                    }
                    _ => {}
                }
                while let Ok(data) = write_receiver.try_recv() {
                    socket.send_to(&data, socket_address).ok();
                }
            }
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

    fn get_decoder(&self) -> Arc<Mutex<Decoder>> {
        self.decoder.clone()
    }

    fn get_write_sender(&self) -> Option<Sender<Vec<u8>>> {
        self.write_sender.clone()
    }
}
