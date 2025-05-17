use crate::connection_info::*;
use std::fmt;

#[repr(C)]
#[derive(Clone, Copy, PartialEq)]
pub enum ConnectionType {
    Usb,
    Serial,
    Tcp,
    Udp,
    Bluetooth,
    File,
}

impl From<&ConnectionInfo> for ConnectionType {
    fn from(connection_info: &ConnectionInfo) -> Self {
        match connection_info {
            ConnectionInfo::UsbConnectionInfo(_) => ConnectionType::Usb,
            ConnectionInfo::SerialConnectionInfo(_) => ConnectionType::Serial,
            ConnectionInfo::TcpConnectionInfo(_) => ConnectionType::Tcp,
            ConnectionInfo::UdpConnectionInfo(_) => ConnectionType::Udp,
            ConnectionInfo::BluetoothConnectionInfo(_) => ConnectionType::Bluetooth,
            ConnectionInfo::FileConnectionInfo(_) => ConnectionType::File,
        }
    }
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
