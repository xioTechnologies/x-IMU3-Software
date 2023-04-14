use crossbeam::channel::Sender;
use std::fs::OpenOptions;
use std::io::Read;
use std::sync::{Arc, Mutex};
use crate::connection_info::*;
use crate::connections::*;
use crate::decoder::*;
use crate::dispatcher::*;

pub struct FileConnection {
    connection_info: FileConnectionInfo,
    decoder: Arc<Mutex<Decoder>>,
    close_sender: Option<Sender<()>>,
}

impl FileConnection {
    pub fn new(connection_info: &FileConnectionInfo) -> FileConnection {
        FileConnection {
            connection_info: connection_info.clone(),
            decoder: Arc::new(Mutex::new(Decoder::new())),
            close_sender: None,
        }
    }
}

impl GenericConnection for FileConnection {
    fn open(&mut self) -> std::io::Result<()> {
        let mut file = OpenOptions::new().read(true).open(&self.connection_info.file_path)?;

        let decoder = self.decoder.clone();

        let (close_sender, close_receiver) = crossbeam::channel::bounded(1);

        self.close_sender = Some(close_sender);

        std::thread::spawn(move || {
            let mut buffer: Vec<u8> = vec![0; 2048];

            while let Err(_) = close_receiver.try_recv() {
                if let Ok(number_of_bytes) = file.read(&mut buffer) {
                    if number_of_bytes == 0 {
                        decoder.lock().unwrap().dispatcher.sender.send(DispatcherData::EndOfFile()).ok();
                        break;
                    }
                    decoder.lock().unwrap().process_bytes(&buffer.as_mut_slice()[..number_of_bytes]);
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
        ConnectionInfo::FileConnectionInfo(self.connection_info.clone())
    }

    fn get_decoder(&self) -> Arc<Mutex<Decoder>> {
        self.decoder.clone()
    }

    fn get_write_sender(&self) -> Option<Sender<String>> {
        None
    }
}
