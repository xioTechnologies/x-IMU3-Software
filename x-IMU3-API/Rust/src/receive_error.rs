use std::fmt;

#[repr(C)]
#[derive(Clone, Copy, PartialEq)]
pub enum ReceiveError {
    BufferOverrun,
    InvalidMessageIdentifier,
    InvalidJson,
    InvalidMuxMessageLength,
    InvalidEscapeSequence,
    InvalidBinaryMessageLength,
    UnableToParseAsciiMessage,
    UnknownError,
}

impl fmt::Display for ReceiveError {
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        match *self {
            Self::BufferOverrun => write!(formatter, "Buffer overrun"),
            Self::InvalidMessageIdentifier => write!(formatter, "Invalid message identifier"),
            Self::InvalidJson => write!(formatter, "Invalid JSON"),
            Self::InvalidMuxMessageLength => write!(formatter, "Invalid mux message length"),
            Self::InvalidEscapeSequence => write!(formatter, "Invalid escape sequence"),
            Self::InvalidBinaryMessageLength => write!(formatter, "Invalid binary message length"),
            Self::UnableToParseAsciiMessage => write!(formatter, "Unable to parse ASCII message"),
            Self::UnknownError => write!(formatter, "Unknown error"),
        }
    }
}
