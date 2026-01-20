use crate::connection_config::*;
use crate::connections::*;
use crate::dispatcher::*;
use crate::receiver::*;
use crossbeam::channel::Sender;
use std::fs::OpenOptions;
use std::io::Read;
use std::sync::{Arc, Mutex};

pub struct FileConnection {
    config: FileConnectionConfig,
    receiver: Arc<Mutex<Receiver>>,
    close_sender: Option<Sender<()>>,
}

impl FileConnection {
    pub fn new(config: &FileConnectionConfig) -> Self {
        Self {
            config: config.clone(),
            receiver: Arc::new(Mutex::new(Receiver::new())),
            close_sender: None,
        }
    }
}

impl GenericConnection for FileConnection {
    fn open(&mut self) -> std::io::Result<()> {
        let mut file = OpenOptions::new().read(true).open(&self.config.file_path)?;

        let receiver = self.receiver.clone();

        let (close_sender, close_receiver) = crossbeam::channel::bounded(1);

        self.close_sender = Some(close_sender);

        std::thread::spawn(move || {
            let mut buffer = [0u8; 2048];

            while close_receiver.try_recv().is_err() {
                if let Ok(number_of_bytes) = file.read(&mut buffer) {
                    if number_of_bytes == 0 {
                        receiver.lock().unwrap().dispatcher.sender.send(DispatcherData::EndOfFile()).ok();
                        break;
                    }
                    receiver.lock().unwrap().receive_bytes(&buffer[..number_of_bytes]);
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
        ConnectionConfig::FileConnectionConfig(self.config.clone())
    }

    fn get_receiver(&self) -> Arc<Mutex<Receiver>> {
        self.receiver.clone()
    }

    fn get_write_sender(&self) -> Option<Sender<Vec<u8>>> {
        None
    }
}
