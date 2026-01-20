use crate::connection_config::*;
use crate::connections::*;
use crate::receiver::*;
use crossbeam::channel::Sender;
use std::io::{Read, Write};
use std::net::{IpAddr, SocketAddr, TcpStream};
use std::sync::{Arc, Mutex};
use std::time::Duration;

pub struct TcpConnection {
    config: TcpConnectionConfig,
    receiver: Arc<Mutex<Receiver>>,
    close_sender: Option<Sender<()>>,
    write_sender: Option<Sender<Vec<u8>>>,
}

impl TcpConnection {
    pub fn new(config: &TcpConnectionConfig) -> Self {
        Self {
            config: config.clone(),
            receiver: Arc::new(Mutex::new(Receiver::new())),
            close_sender: None,
            write_sender: None,
        }
    }
}

impl GenericConnection for TcpConnection {
    fn open(&mut self) -> std::io::Result<()> {
        let mut stream = TcpStream::connect_timeout(&SocketAddr::new(IpAddr::V4(self.config.ip_address), self.config.port), Duration::new(3, 0))?;

        stream.set_nonblocking(true)?;

        let receiver = self.receiver.clone();

        let (close_sender, close_receiver) = crossbeam::channel::bounded(1);
        let (write_sender, write_receiver) = crossbeam::channel::unbounded();

        self.close_sender = Some(close_sender);
        self.write_sender = Some(write_sender);

        std::thread::spawn(move || {
            let mut buffer = [0u8; 2048];

            while close_receiver.try_recv().is_err() {
                match stream.read(&mut buffer) {
                    Ok(number_of_bytes) => {
                        receiver.lock().unwrap().receive_bytes(&buffer[..number_of_bytes]);
                    }
                    Err(_) => {
                        std::thread::sleep(std::time::Duration::from_millis(1));
                    }
                }
                while let Ok(data) = write_receiver.try_recv() {
                    stream.write(&data).ok();
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

    fn get_config(&self) -> ConnectionConfig {
        ConnectionConfig::TcpConnectionConfig(self.config.clone())
    }

    fn get_receiver(&self) -> Arc<Mutex<Receiver>> {
        self.receiver.clone()
    }

    fn get_write_sender(&self) -> Option<Sender<Vec<u8>>> {
        self.write_sender.clone()
    }
}
