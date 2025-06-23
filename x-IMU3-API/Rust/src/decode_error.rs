use std::fmt;

#[repr(C)]
#[derive(Clone, Copy, PartialEq)]
pub enum DecodeError {
    BufferOverrun,
    InvalidMessageIdentifier,
    InvalidUtf8,
    InvalidJson,
    JsonIsNotAnObject,
    JsonObjectIsNotASingleKeyValuePair,
    InvalidMuxMessageLength,
    InvalidEscapeSequence,
    InvalidBinaryMessageLength,
    UnableToParseAsciiMessage,
}

impl fmt::Display for DecodeError {
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        match *self {
            DecodeError::BufferOverrun => write!(formatter, "Buffer overrun"),
            DecodeError::InvalidMessageIdentifier => write!(formatter, "Invalid message identifier"),
            DecodeError::InvalidUtf8 => write!(formatter, "Invalid UTF-8"),
            DecodeError::InvalidJson => write!(formatter, "Invalid JSON"),
            DecodeError::JsonIsNotAnObject => write!(formatter, "JSON is not an object"),
            DecodeError::JsonObjectIsNotASingleKeyValuePair => write!(formatter, "JSON object is not a single key/value pair"),
            DecodeError::InvalidMuxMessageLength => write!(formatter, "Invalid mux message length"),
            DecodeError::InvalidEscapeSequence => write!(formatter, "Invalid escape sequence"),
            DecodeError::InvalidBinaryMessageLength => write!(formatter, "Invalid binary message length"),
            DecodeError::UnableToParseAsciiMessage => write!(formatter, "Unable to parse ASCII message"),
        }
    }
}
