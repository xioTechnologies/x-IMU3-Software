use crate::connection::*;
use std::fmt;
use std::ops::Drop;
use std::sync::{Arc, Mutex};

#[repr(C)]
#[derive(Clone, Copy, PartialEq)]
pub enum ConnectionStatus {
    Connected,
    Reconnecting,
}

impl fmt::Display for ConnectionStatus {
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        match *self {
            ConnectionStatus::Connected => write!(formatter, "Connected"),
            ConnectionStatus::Reconnecting => write!(formatter, "Reconnecting"),
        }
    }
}

pub struct KeepOpen<'a> {
    dropped: Arc<Mutex<bool>>,
    connection: &'a Connection,
}

impl<'a> KeepOpen<'a> {
    pub fn new(connection: &'a Connection, closure: Box<dyn Fn(ConnectionStatus) + Send>) -> Self {
        let keep_open = Self {
            dropped: Arc::new(Mutex::new(false)),
            connection,
        };

        let dropped = keep_open.dropped.clone();
        let connection = connection.internal.clone();

        std::thread::spawn(move || loop {
            loop {
                if let Ok(dropped) = dropped.lock() {
                    if *dropped {
                        return;
                    }
                    if connection.lock().unwrap().open().is_ok() {
                        break;
                    }
                }
                std::thread::sleep(std::time::Duration::from_millis(100));
            }

            if let Ok(dropped) = dropped.lock() {
                if *dropped {
                    return;
                }
                closure(ConnectionStatus::Connected);
            }

            loop {
                let data_total = connection.lock().unwrap().get_receiver().lock().unwrap().statistics.data_total;

                for _ in 0..10 {
                    std::thread::sleep(std::time::Duration::from_millis(100)); // 10 * 100 ms = 1 second

                    if let Ok(dropped) = dropped.lock() {
                        if *dropped {
                            return;
                        }
                    }
                }

                if connection.lock().unwrap().get_receiver().lock().unwrap().statistics.data_total > data_total {
                    continue;
                }

                let receiver = connection.lock().unwrap().get_receiver();
                let write_sender = connection.lock().unwrap().get_write_sender();

                if let Ok(dropped) = dropped.lock() {
                    if *dropped {
                        return;
                    }
                    Connection::ping_internal(receiver, write_sender).ok();
                }

                if connection.lock().unwrap().get_receiver().lock().unwrap().statistics.data_total > data_total {
                    continue;
                }

                connection.lock().unwrap().close();
                break;
            }

            if let Ok(dropped) = dropped.lock() {
                if *dropped {
                    return;
                }
                closure(ConnectionStatus::Reconnecting);
            }
        });

        keep_open
    }
}

impl Drop for KeepOpen<'_> {
    fn drop(&mut self) {
        *self.dropped.lock().unwrap() = true;
        self.connection.close();
    }
}
