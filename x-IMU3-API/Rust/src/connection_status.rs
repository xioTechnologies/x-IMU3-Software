use std::fmt;

#[repr(C)]
#[derive(Clone, Copy)]
pub enum ConnectionStatus {
    Disconnected,
    Connected,
}

impl fmt::Display for ConnectionStatus {
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        match *self {
            ConnectionStatus::Disconnected => write!(formatter, "Disconnected"),
            ConnectionStatus::Connected => write!(formatter, "Connected"),
        }
    }
}
