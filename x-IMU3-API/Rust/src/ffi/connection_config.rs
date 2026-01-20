use crate::connection::*;
use crate::connection_config::*;
use crate::ffi::helpers::*;
use std::net::Ipv4Addr;
use std::os::raw::c_char;

#[repr(C)]
pub struct UsbConnectionConfigC {
    pub port_name: [c_char; CHAR_ARRAY_SIZE],
}

impl Default for UsbConnectionConfigC {
    fn default() -> Self {
        Self {
            port_name: EMPTY_CHAR_ARRAY,
        }
    }
}

impl From<&UsbConnectionConfig> for UsbConnectionConfigC {
    fn from(config: &UsbConnectionConfig) -> Self {
        Self {
            port_name: str_to_char_array(&config.port_name),
        }
    }
}

impl From<UsbConnectionConfigC> for UsbConnectionConfig {
    fn from(config: UsbConnectionConfigC) -> Self {
        Self {
            port_name: unsafe { char_array_to_string(&config.port_name) },
        }
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_usb_connection_config_to_string(config: UsbConnectionConfigC) -> *const c_char {
    str_to_char_ptr(&UsbConnectionConfig::from(config).to_string())
}

#[repr(C)]
pub struct SerialConnectionConfigC {
    pub port_name: [c_char; CHAR_ARRAY_SIZE],
    pub baud_rate: u32,
    pub rts_cts_enabled: bool,
}

impl Default for SerialConnectionConfigC {
    fn default() -> Self {
        Self {
            port_name: EMPTY_CHAR_ARRAY,
            baud_rate: 0,
            rts_cts_enabled: false,
        }
    }
}

impl From<&SerialConnectionConfig> for SerialConnectionConfigC {
    fn from(config: &SerialConnectionConfig) -> Self {
        Self {
            port_name: str_to_char_array(&config.port_name),
            baud_rate: config.baud_rate,
            rts_cts_enabled: config.rts_cts_enabled,
        }
    }
}

impl From<SerialConnectionConfigC> for SerialConnectionConfig {
    fn from(config: SerialConnectionConfigC) -> Self {
        Self {
            port_name: unsafe { char_array_to_string(&config.port_name) },
            baud_rate: config.baud_rate,
            rts_cts_enabled: config.rts_cts_enabled,
        }
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_serial_connection_config_to_string(config: SerialConnectionConfigC) -> *const c_char {
    str_to_char_ptr(&SerialConnectionConfig::from(config).to_string())
}

#[repr(C)]
pub struct TcpConnectionConfigC {
    pub ip_address: [c_char; CHAR_ARRAY_SIZE],
    pub port: u16,
}

impl Default for TcpConnectionConfigC {
    fn default() -> Self {
        Self {
            ip_address: EMPTY_CHAR_ARRAY,
            port: 0,
        }
    }
}

impl From<&TcpConnectionConfig> for TcpConnectionConfigC {
    fn from(config: &TcpConnectionConfig) -> Self {
        Self {
            ip_address: str_to_char_array(&config.ip_address.to_string()),
            port: config.port,
        }
    }
}

impl From<TcpConnectionConfigC> for TcpConnectionConfig {
    fn from(config: TcpConnectionConfigC) -> Self {
        Self {
            ip_address: unsafe { char_array_to_string(&config.ip_address) }.parse().ok().unwrap_or_else(|| Ipv4Addr::new(0, 0, 0, 0)),
            port: config.port,
        }
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_tcp_connection_config_to_string(config: TcpConnectionConfigC) -> *const c_char {
    str_to_char_ptr(&TcpConnectionConfig::from(config).to_string())
}

#[repr(C)]
pub struct UdpConnectionConfigC {
    pub ip_address: [c_char; CHAR_ARRAY_SIZE],
    pub send_port: u16,
    pub receive_port: u16,
}

impl Default for UdpConnectionConfigC {
    fn default() -> Self {
        Self {
            ip_address: EMPTY_CHAR_ARRAY,
            send_port: 0,
            receive_port: 0,
        }
    }
}

impl From<&UdpConnectionConfig> for UdpConnectionConfigC {
    fn from(config: &UdpConnectionConfig) -> Self {
        Self {
            ip_address: str_to_char_array(&config.ip_address.to_string()),
            send_port: config.send_port,
            receive_port: config.receive_port,
        }
    }
}

impl From<UdpConnectionConfigC> for UdpConnectionConfig {
    fn from(config: UdpConnectionConfigC) -> Self {
        Self {
            ip_address: unsafe { char_array_to_string(&config.ip_address) }.parse().ok().unwrap_or_else(|| Ipv4Addr::new(0, 0, 0, 0)),
            send_port: config.send_port,
            receive_port: config.receive_port,
        }
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_udp_connection_config_to_string(config: UdpConnectionConfigC) -> *const c_char {
    str_to_char_ptr(&UdpConnectionConfig::from(config).to_string())
}

#[repr(C)]
pub struct BluetoothConnectionConfigC {
    pub port_name: [c_char; CHAR_ARRAY_SIZE],
}

impl Default for BluetoothConnectionConfigC {
    fn default() -> Self {
        Self {
            port_name: EMPTY_CHAR_ARRAY,
        }
    }
}

impl From<&BluetoothConnectionConfig> for BluetoothConnectionConfigC {
    fn from(config: &BluetoothConnectionConfig) -> Self {
        Self {
            port_name: str_to_char_array(&config.port_name),
        }
    }
}

impl From<BluetoothConnectionConfigC> for BluetoothConnectionConfig {
    fn from(config: BluetoothConnectionConfigC) -> Self {
        Self {
            port_name: unsafe { char_array_to_string(&config.port_name) },
        }
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_bluetooth_connection_config_to_string(config: BluetoothConnectionConfigC) -> *const c_char {
    str_to_char_ptr(&BluetoothConnectionConfig::from(config).to_string())
}

#[repr(C)]
pub struct FileConnectionConfigC {
    pub file_path: [c_char; CHAR_ARRAY_SIZE],
}

impl Default for FileConnectionConfigC {
    fn default() -> Self {
        Self {
            file_path: EMPTY_CHAR_ARRAY,
        }
    }
}

impl From<&FileConnectionConfig> for FileConnectionConfigC {
    fn from(config: &FileConnectionConfig) -> Self {
        Self {
            file_path: str_to_char_array(&config.file_path),
        }
    }
}

impl From<FileConnectionConfigC> for FileConnectionConfig {
    fn from(config: FileConnectionConfigC) -> Self {
        Self {
            file_path: unsafe { char_array_to_string(&config.file_path) },
        }
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_file_connection_config_to_string(config: FileConnectionConfigC) -> *const c_char {
    str_to_char_ptr(&FileConnectionConfig::from(config).to_string())
}

#[no_mangle]
pub extern "C" fn XIMU3_mux_connection_config_new(channel: u8, connection: *mut Connection) -> *mut MuxConnectionConfig {
    let connection: &Connection = unsafe { &*connection };
    Box::into_raw(Box::new(MuxConnectionConfig::new(channel, connection)))
}

#[no_mangle]
pub extern "C" fn XIMU3_mux_connection_config_free(config: *mut MuxConnectionConfig) {
    unsafe { drop(Box::from_raw(config)) };
}

#[no_mangle]
pub extern "C" fn XIMU3_mux_connection_config_to_string(config: *mut MuxConnectionConfig) -> *const c_char {
    let config: &MuxConnectionConfig = unsafe { &*config };
    str_to_char_ptr(&config.to_string())
}
