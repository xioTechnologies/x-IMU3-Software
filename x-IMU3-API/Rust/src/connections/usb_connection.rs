use crate::connection_config::*;
use crate::connections::*;
use crate::receiver::*;
use crossbeam::channel::Sender;
use std::sync::{Arc, Mutex};

pub struct UsbConnection {
    config: UsbConnectionConfig,
    serial_connection: SerialConnection,
}

impl UsbConnection {
    pub fn new(config: &UsbConnectionConfig) -> Self {
        let serial_config = SerialConnectionConfig {
            port_name: config.port_name.clone(),
            baud_rate: 115200,
            rts_cts_enabled: false,
        };

        let serial_connection = SerialConnection::new(&serial_config);

        Self {
            config: config.clone(),
            serial_connection,
        }
    }
}

impl GenericConnection for UsbConnection {
    fn open(&mut self) -> std::io::Result<()> {
        self.serial_connection.open()
    }

    fn close(&self) {
        self.serial_connection.close();
    }

    fn get_config(&self) -> ConnectionConfig {
        ConnectionConfig::UsbConnectionConfig(self.config.clone())
    }

    fn get_receiver(&self) -> Arc<Mutex<Receiver>> {
        self.serial_connection.get_receiver()
    }

    fn get_write_sender(&self) -> Option<Sender<Vec<u8>>> {
        self.serial_connection.get_write_sender()
    }
}
