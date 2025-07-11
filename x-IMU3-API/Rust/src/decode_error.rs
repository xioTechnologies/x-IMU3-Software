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
    InvalidEscapeSequence,
    InvalidBinaryMessageLength,
    UnableToParseAsciiMessage,
}

impl fmt::Display for DecodeError {
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        match *self {
            Self::BufferOverrun => write!(formatter, "Buffer overrun"),
            Self::InvalidMessageIdentifier => write!(formatter, "Invalid message identifier"),
            Self::InvalidUtf8 => write!(formatter, "Invalid UTF-8"),
            Self::InvalidJson => write!(formatter, "Invalid JSON"),
            Self::JsonIsNotAnObject => write!(formatter, "JSON is not an object"),
            Self::JsonObjectIsNotASingleKeyValuePair => write!(formatter, "JSON object is not a single key/value pair"),
            Self::InvalidEscapeSequence => write!(formatter, "Invalid escape sequence"),
            Self::InvalidBinaryMessageLength => write!(formatter, "Invalid binary message length"),
            Self::UnableToParseAsciiMessage => write!(formatter, "Unable to parse ASCII message"),
        }
    }
}
