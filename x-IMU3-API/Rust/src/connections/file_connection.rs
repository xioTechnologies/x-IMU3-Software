use crossbeam::channel::Sender;
use std::fs::OpenOptions;
use std::io::Read;
use std::sync::{Arc, Mutex};
use crate::connection_info::*;
use crate::connections::*;
use crate::decoder::*;

pub struct FileConnection {
    connection_info: FileConnectionInfo,
    decoder: Arc<Mutex<Decoder>>,
    close_sender: Option<Sender<()>>,
    write_sender: Option<Sender<String>>,
}

impl FileConnection {
    pub fn new(connection_info: FileConnectionInfo) -> FileConnection {
        FileConnection {
            connection_info,
            decoder: Arc::new(Mutex::new(Decoder::new())),
            close_sender: None,
            write_sender: None,
        }
    }
}

impl GenericConnection for FileConnection {
    fn open(&mut self) -> std::io::Result<()> {
        let mut file = OpenOptions::new().read(true).open(&self.connection_info.file_path)?;

        let decoder = self.decoder.clone();

        let (close_sender, close_receiver) = crossbeam::channel::bounded(1);
        let (write_sender, write_receiver) = crossbeam::channel::unbounded();

        self.close_sender = Some(close_sender);
        self.write_sender = Some(write_sender);

        std::thread::spawn(move || {
            let mut buffer: Vec<u8> = vec![0; 2048];

            let mut read_enabled = false;

            while let Err(_) = close_receiver.try_recv() {
                if read_enabled {
                    if let Ok(number_of_bytes) = file.read(&mut buffer) {
                        decoder.lock().unwrap().process_received_data(&buffer.as_mut_slice()[..number_of_bytes]);
                    }
                }
                while let Some(_) = write_receiver.try_recv().iter().next() {
                    read_enabled = true;
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
        ConnectionInfo::FileConnectionInfo(self.connection_info.clone())
    }

    fn get_decoder(&mut self) -> Arc<Mutex<Decoder>> {
        self.decoder.clone()
    }

    fn get_write_sender(&mut self) -> Option<Sender<String>> {
        self.write_sender.clone()
    }
}
