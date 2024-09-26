use crossbeam::channel::Sender;
use serialport::FlowControl;
use std::sync::{Arc, Mutex};
use std::time::Duration;
use crate::connection_info::*;
use crate::connection_status::*;
use crate::connections::*;
use crate::decoder::*;
use crate::dispatcher::*;

pub struct SerialConnection {
    connection_info: SerialConnectionInfo,
    connection_status: Arc<Mutex<ConnectionStatus>>,
    decoder: Arc<Mutex<Decoder>>,
    close_sender: Option<Sender<()>>,
    write_sender: Option<Sender<String>>,
}

impl SerialConnection {
    pub fn new(connection_info: &SerialConnectionInfo) -> SerialConnection {
        SerialConnection {
            connection_info: connection_info.clone(),
            connection_status: Arc::new(Mutex::new(ConnectionStatus::Disconnected)),
            decoder: Arc::new(Mutex::new(Decoder::new())),
            close_sender: None,
            write_sender: None,
        }
    }
}

impl GenericConnection for SerialConnection {
    fn open(&mut self) -> std::io::Result<()> {
        let mut serial_port = serialport::new(&self.connection_info.port_name, self.connection_info.baud_rate)
            .flow_control(if self.connection_info.rts_cts_enabled { FlowControl::Hardware } else { FlowControl::None })
            .timeout(Duration::from_millis(1))
            .open()?;

        serial_port.write_data_terminal_ready(true).ok();

        *self.connection_status.lock().unwrap() = ConnectionStatus::Connected;
        self.decoder.lock().unwrap().dispatcher.sender.send(DispatcherData::ConnectionStatus(ConnectionStatus::Connected)).ok();

        let connection_status = self.connection_status.clone();

        let decoder = self.decoder.clone();

        let (close_sender, close_receiver) = crossbeam::channel::bounded(1);
        let (write_sender, write_receiver) = crossbeam::channel::unbounded();

        self.close_sender = Some(close_sender);
        self.write_sender = Some(write_sender);

        std::thread::spawn(move || {
            let mut buffer: Vec<u8> = vec![0; 2048];

            while let Err(_) = close_receiver.try_recv() {
                if let Ok(number_of_bytes) = serial_port.read(buffer.as_mut_slice()) {
                    decoder.lock().unwrap().process_bytes(&buffer.as_mut_slice()[..number_of_bytes]);
                }
                while let Some(terminated_json) = write_receiver.try_recv().iter().next() {
                    serial_port.write(terminated_json.as_bytes()).ok();
                }
            }

            *connection_status.lock().unwrap() = ConnectionStatus::Disconnected;
            decoder.lock().unwrap().dispatcher.sender.send(DispatcherData::ConnectionStatus(ConnectionStatus::Disconnected)).ok();
        });

        Ok(())
    }

    fn close(&self) {
        if let Some(close_sender) = &self.close_sender {
            close_sender.send(()).ok();
        }
    }

    fn get_info(&self) -> ConnectionInfo {
        ConnectionInfo::SerialConnectionInfo(self.connection_info.clone())
    }

    fn get_status(&self) -> ConnectionStatus {
        *self.connection_status.lock().unwrap()
    }

    fn get_decoder(&self) -> Arc<Mutex<Decoder>> {
        self.decoder.clone()
    }

    fn get_write_sender(&self) -> Option<Sender<String>> {
        self.write_sender.clone()
    }
}
