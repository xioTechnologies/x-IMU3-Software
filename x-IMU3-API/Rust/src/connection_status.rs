use std::fmt;

#[repr(C)]
#[derive(Clone, Copy)]
pub enum ConnectionStatus {
    Connecting,
    Connected,
    Disconnected,
    Reconnecting,
}

impl fmt::Display for ConnectionStatus {
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        match *self {
            ConnectionStatus::Connecting => write!(formatter, "Connecting"),
            ConnectionStatus::Connected => write!(formatter, "Connected"),
            ConnectionStatus::Disconnected => write!(formatter, "Disconnected"),
            ConnectionStatus::Reconnecting => write!(formatter, "Reconnecting"),
        }
    }
}
