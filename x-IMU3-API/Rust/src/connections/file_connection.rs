use crate::connection_config::*;
use crate::connection_status::*;
use crate::connections::*;
use crate::dispatcher::*;
use crate::receiver::*;
use crossbeam::channel::Sender;
use std::fs::OpenOptions;
use std::io::Read;
use std::sync::atomic::{AtomicI32, Ordering};
use std::sync::{Arc, Mutex};

pub struct FileConnection {
    config: FileConnectionConfig,
    status: Arc<AtomicI32>,
    receiver: Arc<Mutex<Receiver>>,
    close_sender: Option<Sender<()>>,
}

impl FileConnection {
    pub fn new(config: &FileConnectionConfig) -> Self {
        Self {
            config: config.clone(),
            status: Arc::new(AtomicI32::new(ConnectionStatus::Disconnected as i32)),
            receiver: Arc::new(Mutex::new(Receiver::new())),
            close_sender: None,
        }
    }
}

impl GenericConnection for FileConnection {
    fn open(&mut self) -> std::io::Result<()> {
        let mut file = OpenOptions::new().read(true).open(&self.config.file_path)?;

        let status = self.status.clone();

        let receiver = self.receiver.clone();

        let (close_sender, close_receiver) = crossbeam::channel::bounded(1);

        self.close_sender = Some(close_sender);

        self.status.store(ConnectionStatus::Connected as i32, Ordering::SeqCst);
        self.receiver.lock().unwrap().dispatcher.sender.send(DispatcherData::Status(ConnectionStatus::Connected)).ok();

        std::thread::spawn(move || {
            let mut buffer = [0u8; 2048];

            while close_receiver.try_recv().is_err() {
                if let Ok(number_of_bytes) = file.read(&mut buffer) {
                    if number_of_bytes == 0 {
                        break;
                    }
                    receiver.lock().unwrap().receive_bytes(&buffer[..number_of_bytes]);
                }
            }

            status.store(ConnectionStatus::Disconnected as i32, Ordering::SeqCst);
            receiver.lock().unwrap().dispatcher.sender.send(DispatcherData::Status(ConnectionStatus::Disconnected)).ok();
        });

        Ok(())
    }

    fn close(&self) {
        if let Some(close_sender) = &self.close_sender {
            close_sender.send(()).ok();
        }
    }

    fn get_config(&self) -> ConnectionConfig {
        ConnectionConfig::FileConnectionConfig(self.config.clone())
    }

    fn get_status(&self) -> ConnectionStatus {
        self.status.load(Ordering::SeqCst).into()
    }

    fn get_receiver(&self) -> Arc<Mutex<Receiver>> {
        self.receiver.clone()
    }

    fn get_write_sender(&self) -> Option<Sender<Vec<u8>>> {
        None
    }
}
