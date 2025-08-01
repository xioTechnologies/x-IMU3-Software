use crate::connection_info::*;
use crate::connections::*;
use crate::decoder::*;
use crossbeam::channel::Sender;
use std::sync::{Arc, Mutex};

pub struct UsbConnection {
    connection_info: UsbConnectionInfo,
    serial_connection: SerialConnection,
}

impl UsbConnection {
    pub fn new(connection_info: &UsbConnectionInfo) -> Self {
        let serial_info = SerialConnectionInfo {
            port_name: connection_info.port_name.clone(),
            baud_rate: 115200,
            rts_cts_enabled: false,
        };

        let serial_connection = SerialConnection::new(&serial_info);

        Self {
            connection_info: connection_info.clone(),
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

    fn get_info(&self) -> ConnectionInfo {
        ConnectionInfo::UsbConnectionInfo(self.connection_info.clone())
    }

    fn get_decoder(&self) -> Arc<Mutex<Decoder>> {
        self.serial_connection.get_decoder()
    }

    fn get_write_sender(&self) -> Option<Sender<Vec<u8>>> {
        self.serial_connection.get_write_sender()
    }
}
