use crate::connection_config::*;
use crate::connections::*;
use crate::mux_message::*;
use crate::receiver::*;
use crossbeam::channel::Sender;
use std::sync::{Arc, Mutex};

pub struct MuxConnection {
    config: MuxConnectionConfig,
    receiver: Arc<Mutex<Receiver>>,
    close_sender: Option<Sender<()>>,
    write_sender: Option<Sender<Vec<u8>>>,
}

impl MuxConnection {
    pub fn new(config: &MuxConnectionConfig) -> Self {
        Self {
            config: config.clone(),
            receiver: Arc::new(Mutex::new(Receiver::new())),
            close_sender: None,
            write_sender: None,
        }
    }
}

impl GenericConnection for MuxConnection {
    fn open(&mut self) -> std::io::Result<()> {
        let channel = self.config.channel;
        let connection = self.config.connection.clone();

        let receiver = self.receiver.clone();

        let (close_sender, close_receiver) = crossbeam::channel::bounded(1);
        let (write_sender, write_receiver) = crossbeam::channel::unbounded();

        self.close_sender = Some(close_sender);
        self.write_sender = Some(write_sender);

        std::thread::spawn(move || {
            let closure_id = connection.lock().unwrap().get_receiver().lock().unwrap().dispatcher.add_mux_closure(Box::new(move |message| {
                if message.channel == channel {
                    receiver.lock().unwrap().receive_bytes(message.message.as_slice());
                }
            }));

            while close_receiver.try_recv().is_err() {
                while let Ok(data) = write_receiver.try_recv() {
                    let message = MuxMessage {
                        channel,
                        message: data,
                    };
                    if let Some(connection_write_sender) = &connection.lock().unwrap().get_write_sender() {
                        connection_write_sender.send(message.into_bytes()).ok();
                    }
                }
                std::thread::sleep(std::time::Duration::from_millis(1));
            }

            connection.lock().unwrap().get_receiver().lock().unwrap().dispatcher.remove_closure(closure_id);
        });

        Ok(())
    }

    fn close(&self) {
        if let Some(close_sender) = &self.close_sender {
            close_sender.send(()).ok();
        }
    }

    fn get_config(&self) -> ConnectionConfig {
        ConnectionConfig::MuxConnectionConfig(self.config.clone())
    }

    fn get_receiver(&self) -> Arc<Mutex<Receiver>> {
        self.receiver.clone()
    }

    fn get_write_sender(&self) -> Option<Sender<Vec<u8>>> {
        self.write_sender.clone()
    }
}
