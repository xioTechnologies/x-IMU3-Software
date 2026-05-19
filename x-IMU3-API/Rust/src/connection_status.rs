use std::fmt;

#[repr(C)]
#[derive(Clone, Copy, PartialEq)]
pub enum ConnectionStatus {
    Connected,
    Disconnected,
}

impl From<i32> for ConnectionStatus {
    fn from(value: i32) -> Self {
        match value {
            0 => Self::Connected,
            _ => Self::Disconnected,
        }
    }
}

impl fmt::Display for ConnectionStatus {
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        match *self {
            ConnectionStatus::Connected => write!(formatter, "Connected"),
            ConnectionStatus::Disconnected => write!(formatter, "Disconnected"),
        }
    }
}
