use crossbeam::channel::Sender;
use std::io::{Read, Write};
use std::net::{IpAddr, SocketAddr, TcpStream};
use std::sync::{Arc, Mutex};
use std::time::Duration;
use crate::connection_info::*;
use crate::connections::*;
use crate::decoder::*;

pub struct TcpConnection {
    connection_info: TcpConnectionInfo,
    decoder: Arc<Mutex<Decoder>>,
    close_sender: Option<Sender<()>>,
    write_sender: Option<Sender<String>>,
}

impl TcpConnection {
    pub fn new(connection_info: &TcpConnectionInfo) -> TcpConnection {
        TcpConnection {
            connection_info: connection_info.clone(),
            decoder: Arc::new(Mutex::new(Decoder::new())),
            close_sender: None,
            write_sender: None,
        }
    }
}

impl GenericConnection for TcpConnection {
    fn open(&mut self) -> std::io::Result<()> {
        let mut stream = TcpStream::connect_timeout(&SocketAddr::new(IpAddr::V4(self.connection_info.ip_address), self.connection_info.port), Duration::new(3, 0))?;
        stream.set_read_timeout(Some(std::time::Duration::from_millis(100))).ok();

        let decoder = self.decoder.clone();

        let (close_sender, close_receiver) = crossbeam::channel::bounded(1);
        let (write_sender, write_receiver) = crossbeam::channel::unbounded();

        self.close_sender = Some(close_sender);
        self.write_sender = Some(write_sender);

        std::thread::spawn(move || {
            let mut buffer: Vec<u8> = vec![0; 2048];

            while let Err(_) = close_receiver.try_recv() {
                if let Ok(number_of_bytes) = stream.read(&mut buffer) {
                    decoder.lock().unwrap().process_received_data(&buffer.as_mut_slice()[..number_of_bytes]);
                }
                while let Some(terminated_json) = write_receiver.try_recv().iter().next() {
                    stream.write(terminated_json.as_bytes()).unwrap();
                }
            }
        });

        Ok(())
    }

    fn close(&mut self) {
        if let Some(close_sender) = &self.close_sender {
            close_sender.send(()).ok();
        }
    }

    fn get_info(&mut self) -> ConnectionInfo {
        ConnectionInfo::TcpConnectionInfo(self.connection_info.clone())
    }

    fn get_decoder(&mut self) -> Arc<Mutex<Decoder>> {
        self.decoder.clone()
    }

    fn get_write_sender(&mut self) -> Option<Sender<String>> {
        self.write_sender.clone()
    }
}
