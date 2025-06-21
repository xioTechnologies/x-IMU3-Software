use crate::connection_info::*;
use crate::connections::*;
use crate::decoder::*;
use crate::mux_message::*;
use crossbeam::channel::Sender;
use std::sync::{Arc, Mutex};

pub struct MuxConnection {
    connection_info: MuxConnectionInfo,
    decoder: Arc<Mutex<Decoder>>,
    close_sender: Option<Sender<()>>,
    write_sender: Option<Sender<Vec<u8>>>,
}

impl MuxConnection {
    pub fn new(connection_info: &MuxConnectionInfo) -> MuxConnection {
        MuxConnection {
            connection_info: connection_info.clone(),
            decoder: Arc::new(Mutex::new(Decoder::new())),
            close_sender: None,
            write_sender: None,
        }
    }
}

impl GenericConnection for MuxConnection {
    fn open(&mut self) -> std::io::Result<()> {
        let channel = self.connection_info.channel;
        let mux_decoder = self.connection_info.connection.lock().unwrap().get_decoder();
        let mux_write_sender = self.connection_info.connection.lock().unwrap().get_write_sender();

        let decoder = self.decoder.clone();

        let (close_sender, close_receiver) = crossbeam::channel::bounded(1);
        let (write_sender, write_receiver) = crossbeam::channel::unbounded();

        self.close_sender = Some(close_sender);
        self.write_sender = Some(write_sender);

        std::thread::spawn(move || {
            let closure_id = mux_decoder.lock().unwrap().dispatcher.add_mux_closure(Box::new(move |message| {
                decoder.lock().unwrap().process_bytes(message.into_bytes().as_mut_slice());
            }));

            while close_receiver.try_recv().is_err() {
                while let Ok(data) = write_receiver.try_recv() {
                    let message = MuxMessage {
                        channel,
                        message: data,
                    };
                    if let Some(mux_write_sender) = &mux_write_sender {
                        mux_write_sender.send(message.into_bytes()).ok();
                    }
                }
            }

            mux_decoder.lock().unwrap().dispatcher.remove_closure(closure_id);
        });

        Ok(())
    }

    fn close(&self) {
        if let Some(close_sender) = &self.close_sender {
            close_sender.send(()).ok();
        }
    }

    fn get_info(&self) -> ConnectionInfo {
        ConnectionInfo::MuxConnectionInfo(self.connection_info.clone())
    }

    fn get_decoder(&self) -> Arc<Mutex<Decoder>> {
        self.decoder.clone()
    }

    fn get_write_sender(&self) -> Option<Sender<Vec<u8>>> {
        self.write_sender.clone()
    }
}
