use crate::decode_error::*;
use std::fmt;

#[derive(Clone)]
pub struct MuxMessage {
    pub channel: u8,
    pub message: Vec<u8>,
}

impl MuxMessage {
    pub fn parse(bytes: &[u8]) -> Result<MuxMessage, DecodeError> {
        #[repr(C, packed)]
        struct Header {
            _id: u8,
            channel: u8,
        }

        let min_message_size = size_of::<Header>() + 1; // include termination byte

        if bytes.len() < min_message_size {
            return Err(DecodeError::InvalidMuxMessageLength);
        }

        let channel = unsafe {
            let ref header = *(bytes.as_ptr() as *const Header);
            header.channel
        };

        Ok(Self {
            channel,
            message: (&bytes[size_of::<Header>()..bytes.len()]).to_vec(),
        })
    }

    pub fn into_bytes(self) -> Vec<u8> {
        let mut bytes = vec![b'^', self.channel];
        bytes.extend(self.message);
        bytes
    }
}

impl fmt::Display for MuxMessage {
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        write!(formatter, "0x{:02X} {} bytes", self.channel, self.message.len())
    }
}
