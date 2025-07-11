use crate::connection_info::*;
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
}

impl From<&ConnectionInfo> for ConnectionType {
    fn from(connection_info: &ConnectionInfo) -> Self {
        match connection_info {
            ConnectionInfo::UsbConnectionInfo(_) => Self::Usb,
            ConnectionInfo::SerialConnectionInfo(_) => Self::Serial,
            ConnectionInfo::TcpConnectionInfo(_) => Self::Tcp,
            ConnectionInfo::UdpConnectionInfo(_) => Self::Udp,
            ConnectionInfo::BluetoothConnectionInfo(_) => Self::Bluetooth,
            ConnectionInfo::FileConnectionInfo(_) => Self::File,
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
        }
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_type_to_string(connection_type: ConnectionType) -> *const c_char {
    str_to_char_ptr(&connection_type.to_string())
}
