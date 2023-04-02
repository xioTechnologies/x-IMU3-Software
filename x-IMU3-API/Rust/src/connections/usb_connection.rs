use crossbeam::channel::Sender;
use std::sync::{Arc, Mutex};
use crate::connection_info::*;
use crate::connections::*;
use crate::decoder::*;

pub struct UsbConnection {
    connection_info: UsbConnectionInfo,
    serial_connection: SerialConnection,
}

impl UsbConnection {
    pub fn new(connection_info: &UsbConnectionInfo) -> UsbConnection {
        let serial_info = SerialConnectionInfo { port_name: connection_info.port_name.clone(), baud_rate: 115200, rts_cts_enabled: false };
        let serial_connection = SerialConnection::new(&serial_info);

        UsbConnection {
            connection_info: connection_info.clone(),
            serial_connection,
        }
    }
}

impl GenericConnection for UsbConnection {
    fn open(&mut self) -> std::io::Result<()> {
        self.serial_connection.open()
    }

    fn close(&mut self) {
        self.serial_connection.close();
    }

    fn get_info(&mut self) -> ConnectionInfo {
        ConnectionInfo::UsbConnectionInfo(self.connection_info.clone())
    }

    fn get_decoder(&mut self) -> Arc<Mutex<Decoder>> {
        self.serial_connection.get_decoder()
    }

    fn get_write_sender(&mut self) -> Option<Sender<String>> {
        self.serial_connection.get_write_sender()
    }
}
