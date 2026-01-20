use crate::connection_config::*;
use crate::ffi::helpers::*;
use std::fmt;
use std::os::raw::c_char;

#[repr(C)]
#[derive(Clone, Copy, PartialEq)]
pub enum ConnectionType {
    Usb,
    Serial,
    Tcp,
    Udp,
    Bluetooth,
    File,
    Mux,
}

impl From<&ConnectionConfig> for ConnectionType {
    fn from(config: &ConnectionConfig) -> Self {
        match config {
            ConnectionConfig::UsbConnectionConfig(_) => Self::Usb,
            ConnectionConfig::SerialConnectionConfig(_) => Self::Serial,
            ConnectionConfig::TcpConnectionConfig(_) => Self::Tcp,
            ConnectionConfig::UdpConnectionConfig(_) => Self::Udp,
            ConnectionConfig::BluetoothConnectionConfig(_) => Self::Bluetooth,
            ConnectionConfig::FileConnectionConfig(_) => Self::File,
            ConnectionConfig::MuxConnectionConfig(_) => Self::Mux,
        }
    }
}

impl fmt::Display for ConnectionType {
    fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::Usb => write!(formatter, "USB"),
            Self::Serial => write!(formatter, "Serial"),
            Self::Tcp => write!(formatter, "TCP"),
            Self::Udp => write!(formatter, "UDP"),
            Self::Bluetooth => write!(formatter, "Bluetooth"),
            Self::File => write!(formatter, "File"),
            Self::Mux => write!(formatter, "Mux"),
        }
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_type_to_string(connection_type: ConnectionType) -> *const c_char {
    str_to_char_ptr(&connection_type.to_string())
}
