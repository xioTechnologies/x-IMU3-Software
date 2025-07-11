use crate::connection_info::*;
use crate::connections::*;
use crate::decoder::*;
use crossbeam::channel::Sender;
use serialport::FlowControl;
use std::sync::{Arc, Mutex};
use std::time::Duration;

pub struct SerialConnection {
    connection_info: SerialConnectionInfo,
    decoder: Arc<Mutex<Decoder>>,
    close_sender: Option<Sender<()>>,
    write_sender: Option<Sender<Vec<u8>>>,
}

impl SerialConnection {
    pub fn new(connection_info: &SerialConnectionInfo) -> Self {
        Self {
            connection_info: connection_info.clone(),
            decoder: Arc::new(Mutex::new(Decoder::new())),
            close_sender: None,
            write_sender: None,
        }
    }
}

impl GenericConnection for SerialConnection {
    fn open(&mut self) -> std::io::Result<()> {
        let mut serial_port = serialport::new(&self.connection_info.port_name, self.connection_info.baud_rate)
            .flow_control(match self.connection_info.rts_cts_enabled {
                true => FlowControl::Hardware,
                false => FlowControl::None,
            })
            .timeout(Duration::from_millis(1))
            .open()?;

        serial_port.write_data_terminal_ready(true).ok();

        let decoder = self.decoder.clone();

        let (close_sender, close_receiver) = crossbeam::channel::bounded(1);
        let (write_sender, write_receiver) = crossbeam::channel::unbounded();

        self.close_sender = Some(close_sender);
        self.write_sender = Some(write_sender);

        std::thread::spawn(move || {
            let mut buffer = [0u8; 2048];

            while close_receiver.try_recv().is_err() {
                if let Ok(number_of_bytes) = serial_port.read(buffer.as_mut_slice()) {
                    decoder.lock().unwrap().process_bytes(&buffer[..number_of_bytes]);
                }
                while let Ok(data) = write_receiver.try_recv() {
                    serial_port.write(&data).ok();
                }
            }
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

    fn get_decoder(&self) -> Arc<Mutex<Decoder>> {
        self.decoder.clone()
    }

    fn get_write_sender(&self) -> Option<Sender<Vec<u8>>> {
        self.write_sender.clone()
    }
}
