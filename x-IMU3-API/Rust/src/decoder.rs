use crate::command_message::*;
use crate::data_messages::*;
use crate::decode_error::*;
use crate::dispatcher::*;
use crate::statistics::*;

const BUFFER_SIZE: usize = 4096;

pub struct Decoder {
    buffer: [u8; BUFFER_SIZE],
    buffer_index: usize,
    pub statistics: Statistics,
    pub dispatcher: Dispatcher,
}

impl Decoder {
    pub fn new() -> Decoder {
        Decoder {
            buffer: [0; BUFFER_SIZE],
            buffer_index: 0,
            statistics: Statistics::new(),
            dispatcher: Dispatcher::new(),
        }
    }

    pub fn process_received_data(&mut self, data: &[u8]) {
        self.statistics.data_total += data.len() as u64;

        for byte in data {
            self.buffer[self.buffer_index] = *byte;

            self.buffer_index += 1;
            if self.buffer_index >= self.buffer.len() {
                self.statistics.error_total += 1;
                self.dispatcher.incoming_sender.send(DispatcherData::DecodeError(DecodeError::BufferOverrun)).ok();
                self.buffer_index = 0;
                continue;
            }

            if *byte == '\n' as u8 {
                match self.process_message() {
                    Ok(()) => self.statistics.message_total += 1,
                    Err(decode_error) => {
                        self.statistics.error_total += 1;
                        self.dispatcher.incoming_sender.send(DispatcherData::DecodeError(decode_error)).ok();
                    }
                }
                self.buffer_index = 0;
            }
        }
    }

    fn process_message(&mut self) -> Result<(), DecodeError> {
        if self.buffer[0] == ('{' as u8) {
            self.process_command_message()
        } else {
            self.process_data_message()
        }
    }

    fn process_command_message(&mut self) -> Result<(), DecodeError> {
        let command = CommandMessage::parse_bytes(&self.buffer[..self.buffer_index])?;
        self.dispatcher.incoming_sender.send(DispatcherData::Command(command)).ok();
        Ok(())
    }

    fn process_data_message(&mut self) -> Result<(), DecodeError> {
        let message = Decoder::undo_byte_stuffing(&mut self.buffer[..self.buffer_index])?;

        macro_rules! parse {
            ($data_message:ident, $dispatcher_data:ident) => {{
                match $data_message::parse(message) {
                    Ok(message) => {
                        self.dispatcher.incoming_sender.send(DispatcherData::$dispatcher_data(message)).ok();
                        return Ok(());
                    }
                    Err(error) => {
                        if error != DecodeError::InvalidMessageIdentifier {
                            return Err(error);
                        }
                    }
                }
            }}
        }

        // Start of code block #0 generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py
        parse!(InertialMessage, Inertial);
        parse!(MagnetometerMessage, Magnetometer);
        parse!(QuaternionMessage, Quaternion);
        parse!(RotationMatrixMessage, RotationMatrix);
        parse!(EulerAnglesMessage, EulerAngles);
        parse!(LinearAccelerationMessage, LinearAcceleration);
        parse!(EarthAccelerationMessage, EarthAcceleration);
        parse!(HighGAccelerometerMessage, HighGAccelerometer);
        parse!(TemperatureMessage, Temperature);
        parse!(BatteryMessage, Battery);
        parse!(RssiMessage, Rssi);
        parse!(SerialAccessoryMessage, SerialAccessory);
        parse!(NotificationMessage, Notification);
        parse!(ErrorMessage, Error);
        // End of code block #0 generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py

        Err(DecodeError::InvalidMessageIdentifier)
    }

    fn undo_byte_stuffing(message: &mut [u8]) -> Result<&[u8], DecodeError> {
        const BYTE_STUFFING_END: u8 = 0x0A;
        const BYTE_STUFFING_ESC: u8 = 0xDB;
        const BYTE_STUFFING_ESC_END: u8 = 0xDC;
        const BYTE_STUFFING_ESC_ESC: u8 = 0xDD;

        let mut source_index = 0;
        let mut destination_index = 0;

        while source_index < message.len() {
            if message[source_index] == BYTE_STUFFING_ESC {
                source_index += 1;

                if source_index >= message.len() {
                    return Err(DecodeError::InvalidEscapeSequence);
                }

                match message[source_index] {
                    BYTE_STUFFING_ESC_END => message[destination_index] = BYTE_STUFFING_END,
                    BYTE_STUFFING_ESC_ESC => message[destination_index] = BYTE_STUFFING_ESC,
                    _ => return Err(DecodeError::InvalidEscapeSequence),
                }
            } else {
                message[destination_index] = message[source_index];
            }
            source_index += 1;
            destination_index += 1;
        }
        Ok(&message[..destination_index])
    }
}
