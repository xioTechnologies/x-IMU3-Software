use crossbeam::channel::Sender;
use std::net::{IpAddr, SocketAddr, UdpSocket};
use std::sync::{Arc, Mutex};
use crate::connection_info::*;
use crate::connections::*;
use crate::decoder::*;

pub struct UdpConnection {
    connection_info: UdpConnectionInfo,
    decoder: Arc<Mutex<Decoder>>,
    close_sender: Option<Sender<()>>,
    write_sender: Option<Sender<String>>,
}

impl UdpConnection {
    pub fn new(connection_info: &UdpConnectionInfo) -> UdpConnection {
        UdpConnection {
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

        socket.set_read_timeout(Some(std::time::Duration::from_millis(1))).ok();

        let socket_address = SocketAddr::new(IpAddr::V4(self.connection_info.ip_address), self.connection_info.send_port);

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

    fn get_write_sender(&self) -> Option<Sender<String>> {
        self.write_sender.clone()
    }
}
