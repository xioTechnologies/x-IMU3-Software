use std::str;
use crate::decode_error::*;

pub trait DataMessage {
    fn get_ascii_id() -> u8 where Self: Sized;

    fn get_binary_id() -> u8 where Self: Sized {
        0x80 + Self::get_ascii_id()
    }

    fn parse(message: &[u8]) -> Result<Self, DecodeError> where Self: Sized {
        if message[0] == Self::get_ascii_id() {
            if let Ok(message_str) = str::from_utf8(message) {
                return Self::parse_ascii(message_str);
            }
            return Err(DecodeError::UnableToParseAsciiMessage);
        }
        if message[0] == Self::get_binary_id() {
            return Self::parse_binary(message);
        }
        Err(DecodeError::InvalidMessageIdentifier)
    }

    fn parse_ascii(message: &str) -> Result<Self, DecodeError> where Self: Sized;
    fn parse_binary(message: &[u8]) -> Result<Self, DecodeError> where Self: Sized;
    fn get_csv_file_name(&self) -> &'static str;
    fn get_csv_headings(&self) -> &'static str;
    fn to_csv_row(&self) -> String;
    fn to_ascii(&self) -> String;
}
