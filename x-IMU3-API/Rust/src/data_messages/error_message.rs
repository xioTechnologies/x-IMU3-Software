// This file was generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py

use libc::size_t;
use std::fmt;
use std::mem::size_of;
use std::os::raw::c_char;
use crate::data_messages::*;
use crate::decode_error::*;

#[repr(C)]
#[derive(Copy, Clone)]
pub struct ErrorMessage {
    pub timestamp: u64,
    pub char_array: [c_char; DATA_MESSAGE_CHAR_ARRAY_SIZE],
    pub number_of_bytes: size_t,
}

impl ErrorMessage {
    pub fn char_array_as_string(self) -> String {
        char_array_to_string(&self.char_array, self.number_of_bytes)
    }
}

impl DataMessage for ErrorMessage {
    fn get_ascii_id() -> u8 {
        'F' as u8
    }

    fn parse_ascii(message: &str) -> Result<Self, DecodeError> {
        match scan_fmt!( message, "{},{d},{[^\n]}\r\n",  char, u64, String) {
            Ok((_, timestamp, string)) => {
                let (char_array, number_of_bytes) = string_to_char_array(string);
                Ok(ErrorMessage { timestamp, char_array, number_of_bytes })
            }
            Err(_) => Err(DecodeError::UnableToParseAsciiMessage),
        }
    }

    fn parse_binary(message: &[u8]) -> Result<Self, DecodeError> {
        #[repr(C, packed)]
        struct BinaryMessage {
            id: u8,
            timestamp: u64,
        }

        if (message.len() - 1) < size_of::<BinaryMessage>() {
            return Err(DecodeError::InvalidBinaryMessageLength);
        }

        let binary_message = unsafe {
            let ref binary_message = *(message.as_ptr() as *const BinaryMessage);
            binary_message
        };

        let (char_array, number_of_bytes) = slice_to_char_array(&message[size_of::<BinaryMessage>()..(message.len() - 1)]);

        Ok(ErrorMessage { timestamp: binary_message.timestamp, char_array, number_of_bytes })
    }

    fn get_csv_file_name(&self) -> &'static str {
        "ErrorMessage.csv"
    }

    fn get_csv_headings(&self) -> &'static str {
        "Timestamp (us),String\n"
    }

    fn to_csv_row(&self) -> String {
        format!("{},{}\n", self.timestamp, char_array_to_string(&self.char_array, self.number_of_bytes))
    }
}

impl fmt::Display for ErrorMessage {
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        write!(formatter, "{:>8} us \"{}\"", self.timestamp, char_array_to_string(&self.char_array, self.number_of_bytes))
    }
}
