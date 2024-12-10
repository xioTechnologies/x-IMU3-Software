// This file was generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py

use std::fmt;
use std::mem::size_of;
use crate::data_messages::*;
use crate::decode_error::*;

#[repr(C)]
#[derive(Clone, Copy)]
pub struct TemperatureMessage {
    pub timestamp: u64,
    pub temperature: f32,
}

impl DataMessage for TemperatureMessage {
    fn get_ascii_id() -> u8 {
        'T' as u8
    }

    fn parse_ascii(message: &str) -> Result<Self, DecodeError> {
        match scan_fmt!( message, "{},{d},{f}\n",  char, u64, f32) {
            Ok((_, timestamp, temperature)) => Ok(TemperatureMessage { timestamp, temperature }),
            Err(_) => Err(DecodeError::UnableToParseAsciiMessage),
        }
    }

    fn parse_binary(message: &[u8]) -> Result<Self, DecodeError> {
        #[repr(C, packed)]
        struct BinaryMessage {
            id: u8,
            timestamp: u64,
            temperature: f32,
            termination: u8,
        }

        if message.len() != size_of::<BinaryMessage>() {
            return Err(DecodeError::InvalidBinaryMessageLength);
        }

        let binary_message = unsafe {
            let ref binary_message = *(message.as_ptr() as *const BinaryMessage);
            binary_message
        };

        Ok(TemperatureMessage { timestamp: binary_message.timestamp, temperature: binary_message.temperature })
    }

    fn get_csv_file_name(&self) -> &'static str {
        "Temperature.csv"
    }

    fn get_csv_headings(&self) -> &'static str {
        "Timestamp (us),Temperature (degC)\n"
    }

    fn to_csv_row(&self) -> String {
        format!("{},{:.6}\n", self.timestamp, self.temperature)
    }
}

impl fmt::Display for TemperatureMessage {
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        write!(formatter, "{:>8} us {:>8.3} degC", self.timestamp, self.temperature)
    }
}
