use crossbeam::channel::Sender;
use std::sync::{Arc, Mutex};
use crate::connection_info::*;
use crate::connections::*;
use crate::decoder::*;

pub struct BluetoothConnection {
    connection_info: BluetoothConnectionInfo,
    serial_connection: SerialConnection,
}

impl BluetoothConnection {
    pub fn new(connection_info: &BluetoothConnectionInfo) -> BluetoothConnection {
        let serial_info = SerialConnectionInfo { port_name: connection_info.port_name.clone(), baud_rate: 115200, rts_cts_enabled: false };
        let serial_connection = SerialConnection::new(&serial_info);

        BluetoothConnection {
            connection_info: connection_info.clone(),
            serial_connection,
        }
    }
}

impl GenericConnection for BluetoothConnection {
    fn open(&mut self) -> std::io::Result<()> {
        self.serial_connection.open()
    }

    fn close(&self) {
        self.serial_connection.close();
    }

    fn get_info(&self) -> ConnectionInfo {
        ConnectionInfo::BluetoothConnectionInfo(self.connection_info.clone())
    }

    fn get_decoder(&self) -> Arc<Mutex<Decoder>> {
        self.serial_connection.get_decoder()
    }

    fn get_write_sender(&self) -> Option<Sender<String>> {
        self.serial_connection.get_write_sender()
    }
}
