use crate::connection::*;
use crate::connection_info::*;
use crate::ffi::helpers::*;
use std::net::Ipv4Addr;
use std::os::raw::c_char;

#[repr(C)]
pub struct UsbConnectionInfoC {
    pub port_name: [c_char; CHAR_ARRAY_SIZE],
}

impl Default for UsbConnectionInfoC {
    fn default() -> Self {
        Self {
            port_name: EMPTY_CHAR_ARRAY,
        }
    }
}

impl From<&UsbConnectionInfo> for UsbConnectionInfoC {
    fn from(connection_info: &UsbConnectionInfo) -> Self {
        Self {
            port_name: str_to_char_array(&connection_info.port_name),
        }
    }
}

impl From<UsbConnectionInfoC> for UsbConnectionInfo {
    fn from(connection_info: UsbConnectionInfoC) -> Self {
        Self {
            port_name: unsafe { char_array_to_string(&connection_info.port_name) },
        }
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_usb_connection_info_to_string(connection_info: UsbConnectionInfoC) -> *const c_char {
    str_to_char_ptr(&UsbConnectionInfo::from(connection_info).to_string())
}

#[repr(C)]
pub struct SerialConnectionInfoC {
    pub port_name: [c_char; CHAR_ARRAY_SIZE],
    pub baud_rate: u32,
    pub rts_cts_enabled: bool,
}

impl Default for SerialConnectionInfoC {
    fn default() -> Self {
        Self {
            port_name: EMPTY_CHAR_ARRAY,
            baud_rate: 0,
            rts_cts_enabled: false,
        }
    }
}

impl From<&SerialConnectionInfo> for SerialConnectionInfoC {
    fn from(connection_info: &SerialConnectionInfo) -> Self {
        Self {
            port_name: str_to_char_array(&connection_info.port_name),
            baud_rate: connection_info.baud_rate,
            rts_cts_enabled: connection_info.rts_cts_enabled,
        }
    }
}

impl From<SerialConnectionInfoC> for SerialConnectionInfo {
    fn from(connection_info: SerialConnectionInfoC) -> Self {
        Self {
            port_name: unsafe { char_array_to_string(&connection_info.port_name) },
            baud_rate: connection_info.baud_rate,
            rts_cts_enabled: connection_info.rts_cts_enabled,
        }
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_serial_connection_info_to_string(connection_info: SerialConnectionInfoC) -> *const c_char {
    str_to_char_ptr(&SerialConnectionInfo::from(connection_info).to_string())
}

#[repr(C)]
pub struct TcpConnectionInfoC {
    pub ip_address: [c_char; CHAR_ARRAY_SIZE],
    pub port: u16,
}

impl Default for TcpConnectionInfoC {
    fn default() -> Self {
        Self {
            ip_address: EMPTY_CHAR_ARRAY,
            port: 0,
        }
    }
}

impl From<&TcpConnectionInfo> for TcpConnectionInfoC {
    fn from(connection_info: &TcpConnectionInfo) -> Self {
        Self {
            ip_address: str_to_char_array(&connection_info.ip_address.to_string()),
            port: connection_info.port,
        }
    }
}

impl From<TcpConnectionInfoC> for TcpConnectionInfo {
    fn from(connection_info: TcpConnectionInfoC) -> Self {
        Self {
            ip_address: unsafe { char_array_to_string(&connection_info.ip_address) }.parse().ok().unwrap_or_else(|| Ipv4Addr::new(0, 0, 0, 0)),
            port: connection_info.port,
        }
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_tcp_connection_info_to_string(connection_info: TcpConnectionInfoC) -> *const c_char {
    str_to_char_ptr(&TcpConnectionInfo::from(connection_info).to_string())
}

#[repr(C)]
pub struct UdpConnectionInfoC {
    pub ip_address: [c_char; CHAR_ARRAY_SIZE],
    pub send_port: u16,
    pub receive_port: u16,
}

impl Default for UdpConnectionInfoC {
    fn default() -> Self {
        Self {
            ip_address: EMPTY_CHAR_ARRAY,
            send_port: 0,
            receive_port: 0,
        }
    }
}

impl From<&UdpConnectionInfo> for UdpConnectionInfoC {
    fn from(connection_info: &UdpConnectionInfo) -> Self {
        Self {
            ip_address: str_to_char_array(&connection_info.ip_address.to_string()),
            send_port: connection_info.send_port,
            receive_port: connection_info.receive_port,
        }
    }
}

impl From<UdpConnectionInfoC> for UdpConnectionInfo {
    fn from(connection_info: UdpConnectionInfoC) -> Self {
        Self {
            ip_address: unsafe { char_array_to_string(&connection_info.ip_address) }.parse().ok().unwrap_or_else(|| Ipv4Addr::new(0, 0, 0, 0)),
            send_port: connection_info.send_port,
            receive_port: connection_info.receive_port,
        }
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_udp_connection_info_to_string(connection_info: UdpConnectionInfoC) -> *const c_char {
    str_to_char_ptr(&UdpConnectionInfo::from(connection_info).to_string())
}

#[repr(C)]
pub struct BluetoothConnectionInfoC {
    pub port_name: [c_char; CHAR_ARRAY_SIZE],
}

impl Default for BluetoothConnectionInfoC {
    fn default() -> Self {
        Self {
            port_name: EMPTY_CHAR_ARRAY,
        }
    }
}

impl From<&BluetoothConnectionInfo> for BluetoothConnectionInfoC {
    fn from(connection_info: &BluetoothConnectionInfo) -> Self {
        Self {
            port_name: str_to_char_array(&connection_info.port_name),
        }
    }
}

impl From<BluetoothConnectionInfoC> for BluetoothConnectionInfo {
    fn from(connection_info: BluetoothConnectionInfoC) -> Self {
        Self {
            port_name: unsafe { char_array_to_string(&connection_info.port_name) },
        }
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_bluetooth_connection_info_to_string(connection_info: BluetoothConnectionInfoC) -> *const c_char {
    str_to_char_ptr(&BluetoothConnectionInfo::from(connection_info).to_string())
}

#[repr(C)]
pub struct FileConnectionInfoC {
    pub file_path: [c_char; CHAR_ARRAY_SIZE],
}

impl Default for FileConnectionInfoC {
    fn default() -> Self {
        Self {
            file_path: EMPTY_CHAR_ARRAY,
        }
    }
}

impl From<&FileConnectionInfo> for FileConnectionInfoC {
    fn from(connection_info: &FileConnectionInfo) -> Self {
        Self {
            file_path: str_to_char_array(&connection_info.file_path),
        }
    }
}

impl From<FileConnectionInfoC> for FileConnectionInfo {
    fn from(connection_info: FileConnectionInfoC) -> Self {
        Self {
            file_path: unsafe { char_array_to_string(&connection_info.file_path) },
        }
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_file_connection_info_to_string(connection_info: FileConnectionInfoC) -> *const c_char {
    str_to_char_ptr(&FileConnectionInfo::from(connection_info).to_string())
}

#[no_mangle]
pub extern "C" fn XIMU3_mux_connection_info_new(channel: u8, connection: *mut Connection) -> *mut MuxConnectionInfo {
    let connection: &Connection = unsafe { &*connection };
    Box::into_raw(Box::new(MuxConnectionInfo::new(channel, connection)))
}

#[no_mangle]
pub extern "C" fn XIMU3_mux_connection_info_free(connection_info: *mut MuxConnectionInfo) {
    unsafe { drop(Box::from_raw(connection_info)) };
}

#[no_mangle]
pub extern "C" fn XIMU3_mux_connection_info_to_string(connection_info: *mut MuxConnectionInfo) -> *const c_char {
    let connection_info: &MuxConnectionInfo = unsafe { &*connection_info };
    str_to_char_ptr(&connection_info.to_string())
}
