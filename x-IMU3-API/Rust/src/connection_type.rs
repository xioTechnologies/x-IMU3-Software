use std::fmt;

#[repr(C)]
#[derive(Copy, Clone, PartialEq)]
pub enum ConnectionType {
    Usb,
    Serial,
    Tcp,
    Udp,
    Bluetooth,
    File,
}

impl fmt::Display for ConnectionType {
    fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            ConnectionType::Usb => write!(formatter, "USB"),
            ConnectionType::Serial => write!(formatter, "Serial"),
            ConnectionType::Tcp => write!(formatter, "TCP"),
            ConnectionType::Udp => write!(formatter, "UDP"),
            ConnectionType::Bluetooth => write!(formatter, "Bluetooth"),
            ConnectionType::File => write!(formatter, "File"),
        }
    }
}