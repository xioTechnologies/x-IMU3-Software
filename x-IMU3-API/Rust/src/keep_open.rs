use crate::connection::*;
use crate::connection_status::*;
use std::ops::Drop;
use std::sync::{Arc, Mutex};

pub struct KeepOpen {
    dropped: Arc<Mutex<bool>>,
    connection: InternalConnection,
}

impl KeepOpen {
    pub fn new(connection: &Connection) -> Self {
        let keep_open = Self {
            dropped: Arc::new(Mutex::new(false)),
            connection: connection.internal.clone(),
        };

        let dropped = keep_open.dropped.clone();
        let connection = keep_open.connection.clone();

        std::thread::spawn(move || loop {
            loop {
                if let Ok(dropped) = dropped.lock() {
                    if *dropped {
                        return;
                    }
                    let mut connection = connection.lock().unwrap();
                    if connection.get_status() == ConnectionStatus::Connected || connection.open().is_ok() {
                        break;
                    }
                }
                std::thread::sleep(std::time::Duration::from_millis(100));
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

                if let Ok(dropped) = dropped.lock() {
                    if *dropped {
                        return;
                    }
                    Connection::ping_internal(&connection);
                }

                if connection.lock().unwrap().get_receiver().lock().unwrap().statistics.data_total > data_total {
                    continue;
                }

                connection.lock().unwrap().close();
                break;
            }
        });

        keep_open
    }
}

impl Drop for KeepOpen {
    fn drop(&mut self) {
        *self.dropped.lock().unwrap() = true;
        self.connection.lock().unwrap().close();
    }
}
