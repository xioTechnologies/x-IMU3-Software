use crate::connection_config::*;
use crate::connection_status::*;
use crate::connections::*;
use crate::dispatcher::*;
use crate::mux_message::*;
use crate::receiver::*;
use std::sync::atomic::{AtomicI32, Ordering};
use std::sync::{Arc, Mutex};

pub struct MuxConnection {
    config: MuxConnectionConfig,
    status: Arc<AtomicI32>,
    receiver: Arc<Mutex<Receiver>>,
    close_sender: Option<crossbeam::channel::Sender<()>>,
    write_sender: Option<crossbeam::channel::Sender<Vec<u8>>>,
    thread: Option<std::thread::JoinHandle<()>>,
}

impl MuxConnection {
    pub fn new(config: &MuxConnectionConfig) -> Self {
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

impl GenericConnection for MuxConnection {
    fn open(&mut self) -> crossbeam::channel::Receiver<std::io::Result<()>> {
        let (result_sender, result_receiver) = crossbeam::channel::bounded(1);

        if self.thread.as_ref().is_some_and(|thread| thread.is_finished() == false) {
            result_sender.send(Err(std::io::ErrorKind::AlreadyExists.into())).ok();
            return result_receiver;
        }

        let channel = self.config.channel;
        let connection = self.config.connection.clone();

        let status = self.status.clone();

        let receiver = self.receiver.clone();

        let (close_sender, close_receiver) = crossbeam::channel::bounded(1);
        let (write_sender, write_receiver) = crossbeam::channel::unbounded();

        self.close_sender = Some(close_sender);
        self.write_sender = Some(write_sender);

        self.thread = Some(std::thread::spawn(move || {
            let closure_id = {
                let receiver = receiver.clone();

                connection.lock().unwrap().get_receiver().lock().unwrap().dispatcher.add_mux_closure(Box::new(move |message| {
                    if message.channel == channel {
                        receiver.lock().unwrap().receive_bytes(message.message.as_slice());
                    }
                }))
            };

            status.store(ConnectionStatus::Connected as i32, Ordering::SeqCst);
            receiver.lock().unwrap().dispatcher.sender.send(DispatcherData::Status(ConnectionStatus::Connected)).ok();

            result_sender.send(Ok(())).ok();

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
        ConnectionConfig::MuxConnectionConfig(self.config.clone())
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
