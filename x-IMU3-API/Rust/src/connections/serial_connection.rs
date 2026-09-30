use crate::connection_config::*;
use crate::connection_status::*;
use crate::connections::*;
use crate::dispatcher::*;
use crate::receiver::*;
use serialport::FlowControl;
use std::sync::atomic::{AtomicI32, Ordering};
use std::sync::{Arc, Mutex};

pub struct SerialConnection {
    config: SerialConnectionConfig,
    status: Arc<AtomicI32>,
    receiver: Arc<Mutex<Receiver>>,
    close_sender: Option<crossbeam::channel::Sender<()>>,
    write_sender: Option<crossbeam::channel::Sender<Vec<u8>>>,
    thread: Option<std::thread::JoinHandle<()>>,
}

impl SerialConnection {
    pub fn new(config: &SerialConnectionConfig) -> Self {
        Self {
            config: config.clone(),
            status: Arc::new(AtomicI32::new(ConnectionStatus::Disconnected as i32)),
            receiver: Arc::new(Mutex::new(Receiver::new())),
            close_sender: None,
            write_sender: None,
            thread: None,
        }
    }
}

impl GenericConnection for SerialConnection {
    fn open(&mut self) -> crossbeam::channel::Receiver<std::io::Result<()>> {
        let (result_sender, result_receiver) = crossbeam::channel::bounded(1);

        if self.thread.as_ref().is_some_and(|thread| thread.is_finished() == false) {
            result_sender.send(Err(std::io::ErrorKind::AlreadyExists.into())).ok();
            return result_receiver;
        }

        let config = self.config.clone();

        let status = self.status.clone();

        let receiver = self.receiver.clone();

        let (close_sender, close_receiver) = crossbeam::channel::bounded(1);
        let (write_sender, write_receiver) = crossbeam::channel::unbounded();

        self.close_sender = Some(close_sender);
        self.write_sender = Some(write_sender);

        self.thread = Some(std::thread::spawn(move || {
            let connect = || -> std::io::Result<Box<dyn serialport::SerialPort>> {
                let mut serial_port = serialport::new(&config.port_name, config.baud_rate)
                    .flow_control(match config.rts_cts_enabled {
                        true => FlowControl::Hardware,
                        false => FlowControl::None,
                    })
                    .timeout(std::time::Duration::from_millis(1))
                    .open()?;
                serial_port.write_data_terminal_ready(true).ok();
                Ok(serial_port)
            };

            let mut serial_port = match connect() {
                Ok(serial_port) => serial_port,
                Err(error) => {
                    result_sender.send(Err(error)).ok();
                    return;
                }
            };

            status.store(ConnectionStatus::Connected as i32, Ordering::SeqCst);
            receiver.lock().unwrap().dispatcher.sender.send(DispatcherData::Status(ConnectionStatus::Connected)).ok();

            result_sender.send(Ok(())).ok();

            let mut buffer = [0u8; 2048];

            while close_receiver.try_recv().is_err() {
                if let Ok(number_of_bytes) = serial_port.read(buffer.as_mut_slice()) {
                    receiver.lock().unwrap().receive_bytes(&buffer[..number_of_bytes]);
                }

                while let Ok(data) = write_receiver.try_recv() {
                    serial_port.write(&data).ok();
                }
            }

            status.store(ConnectionStatus::Disconnected as i32, Ordering::SeqCst);
            receiver.lock().unwrap().dispatcher.sender.send(DispatcherData::Status(ConnectionStatus::Disconnected)).ok();
        }));

        result_receiver
    }

    fn close(&self) {
        if let Some(close_sender) = &self.close_sender {
            close_sender.try_send(()).ok();
        }
    }

    fn get_config(&self) -> ConnectionConfig {
        ConnectionConfig::SerialConnectionConfig(self.config.clone())
    }

    fn get_status(&self) -> ConnectionStatus {
        self.status.load(Ordering::SeqCst).into()
    }

    fn get_receiver(&self) -> Arc<Mutex<Receiver>> {
        self.receiver.clone()
    }

    fn get_write_sender(&self) -> Option<crossbeam::channel::Sender<Vec<u8>>> {
        self.write_sender.clone()
    }
}
