use crate::connection::*;
use crate::connections::*;
use std::fmt;
use std::net::Ipv4Addr;
use std::sync::{Arc, Mutex};

#[derive(Clone)]
pub enum ConnectionInfo {
    UsbConnectionInfo(UsbConnectionInfo),
    SerialConnectionInfo(SerialConnectionInfo),
    TcpConnectionInfo(TcpConnectionInfo),
    UdpConnectionInfo(UdpConnectionInfo),
    BluetoothConnectionInfo(BluetoothConnectionInfo),
    FileConnectionInfo(FileConnectionInfo),
    MuxConnectionInfo(MuxConnectionInfo),
}

impl fmt::Display for ConnectionInfo {
    fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::UsbConnectionInfo(connection_info) => connection_info.fmt(formatter),
            Self::SerialConnectionInfo(connection_info) => connection_info.fmt(formatter),
            Self::TcpConnectionInfo(connection_info) => connection_info.fmt(formatter),
            Self::UdpConnectionInfo(connection_info) => connection_info.fmt(formatter),
            Self::BluetoothConnectionInfo(connection_info) => connection_info.fmt(formatter),
            Self::FileConnectionInfo(connection_info) => connection_info.fmt(formatter),
            Self::MuxConnectionInfo(connection_info) => connection_info.fmt(formatter),
        }
    }
}

#[derive(Clone)]
pub struct UsbConnectionInfo {
    pub port_name: String,
}

impl From<SerialConnectionInfo> for UsbConnectionInfo {
    fn from(connection_info: SerialConnectionInfo) -> Self {
        Self {
            port_name: connection_info.port_name,
        }
    }
}

impl fmt::Display for UsbConnectionInfo {
    fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(formatter, "USB {}", self.port_name)
    }
}

#[derive(Clone)]
pub struct SerialConnectionInfo {
    pub port_name: String,
    pub baud_rate: u32,
    pub rts_cts_enabled: bool,
}

impl fmt::Display for SerialConnectionInfo {
    fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        let enabled_disabled = match self.rts_cts_enabled {
            true => "Enabled",
            false => "Disabled",
        };
        write!(formatter, "Serial {}, {}, RTS/CTS {}", self.port_name, self.baud_rate, enabled_disabled)
    }
}

#[derive(Clone)]
pub struct TcpConnectionInfo {
    pub ip_address: Ipv4Addr,
    pub port: u16,
}

impl fmt::Display for TcpConnectionInfo {
    fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(formatter, "TCP {}:{}", self.ip_address, self.port)
    }
}

#[derive(Clone)]
pub struct UdpConnectionInfo {
    pub ip_address: Ipv4Addr,
    pub send_port: u16,
    pub receive_port: u16,
}

impl fmt::Display for UdpConnectionInfo {
    fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(formatter, "UDP {}:{}, {}", self.ip_address, self.send_port, self.receive_port)
    }
}

#[derive(Clone)]
pub struct BluetoothConnectionInfo {
    pub port_name: String,
}
impl From<SerialConnectionInfo> for BluetoothConnectionInfo {
    fn from(connection_info: SerialConnectionInfo) -> Self {
        Self {
            port_name: connection_info.port_name,
        }
    }
}

impl fmt::Display for BluetoothConnectionInfo {
    fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(formatter, "Bluetooth {}", self.port_name)
    }
}

#[derive(Clone)]
pub struct FileConnectionInfo {
    pub file_path: String,
}

impl fmt::Display for FileConnectionInfo {
    fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(formatter, "File {}", self.file_path)
    }
}

#[derive(Clone)]
pub struct MuxConnectionInfo {
    pub channel: u8,
    pub(crate) connection: Arc<Mutex<Box<dyn GenericConnection + Send>>>,
}

impl MuxConnectionInfo {
    pub fn new(channel: u8, connection: &Connection) -> MuxConnectionInfo {
        MuxConnectionInfo {
            channel,
            connection: connection.internal.clone(),
        }
    }
}

impl fmt::Display for MuxConnectionInfo {
    fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(formatter, "Mux 0x{:02X}, {}", self.channel, self.connection.lock().unwrap().get_info().to_string())
    }
}
