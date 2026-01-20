use crate::connection::*;
use crate::connections::*;
use std::fmt;
use std::net::Ipv4Addr;
use std::sync::{Arc, Mutex};

#[derive(Clone)]
pub enum ConnectionConfig {
    UsbConnectionConfig(UsbConnectionConfig),
    SerialConnectionConfig(SerialConnectionConfig),
    TcpConnectionConfig(TcpConnectionConfig),
    UdpConnectionConfig(UdpConnectionConfig),
    BluetoothConnectionConfig(BluetoothConnectionConfig),
    FileConnectionConfig(FileConnectionConfig),
    MuxConnectionConfig(MuxConnectionConfig),
}

impl fmt::Display for ConnectionConfig {
    fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::UsbConnectionConfig(config) => config.fmt(formatter),
            Self::SerialConnectionConfig(config) => config.fmt(formatter),
            Self::TcpConnectionConfig(config) => config.fmt(formatter),
            Self::UdpConnectionConfig(config) => config.fmt(formatter),
            Self::BluetoothConnectionConfig(config) => config.fmt(formatter),
            Self::FileConnectionConfig(config) => config.fmt(formatter),
            Self::MuxConnectionConfig(config) => config.fmt(formatter),
        }
    }
}

#[derive(Clone)]
pub struct UsbConnectionConfig {
    pub port_name: String,
}

impl From<SerialConnectionConfig> for UsbConnectionConfig {
    fn from(config: SerialConnectionConfig) -> Self {
        Self {
            port_name: config.port_name,
        }
    }
}

impl fmt::Display for UsbConnectionConfig {
    fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(formatter, "USB {}", self.port_name)
    }
}

#[derive(Clone)]
pub struct SerialConnectionConfig {
    pub port_name: String,
    pub baud_rate: u32,
    pub rts_cts_enabled: bool,
}

impl fmt::Display for SerialConnectionConfig {
    fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        let enabled_disabled = match self.rts_cts_enabled {
            true => "Enabled",
            false => "Disabled",
        };
        write!(formatter, "Serial {}, {}, RTS/CTS {}", self.port_name, self.baud_rate, enabled_disabled)
    }
}

#[derive(Clone)]
pub struct TcpConnectionConfig {
    pub ip_address: Ipv4Addr,
    pub port: u16,
}

impl fmt::Display for TcpConnectionConfig {
    fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(formatter, "TCP {}:{}", self.ip_address, self.port)
    }
}

#[derive(Clone)]
pub struct UdpConnectionConfig {
    pub ip_address: Ipv4Addr,
    pub send_port: u16,
    pub receive_port: u16,
}

impl fmt::Display for UdpConnectionConfig {
    fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(formatter, "UDP {}:{}, {}", self.ip_address, self.send_port, self.receive_port)
    }
}

#[derive(Clone)]
pub struct BluetoothConnectionConfig {
    pub port_name: String,
}

impl From<SerialConnectionConfig> for BluetoothConnectionConfig {
    fn from(config: SerialConnectionConfig) -> Self {
        Self {
            port_name: config.port_name,
        }
    }
}

impl fmt::Display for BluetoothConnectionConfig {
    fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(formatter, "Bluetooth {}", self.port_name)
    }
}

#[derive(Clone)]
pub struct FileConnectionConfig {
    pub file_path: String,
}

impl fmt::Display for FileConnectionConfig {
    fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(formatter, "File {}", self.file_path)
    }
}

#[derive(Clone)]
pub struct MuxConnectionConfig {
    pub channel: u8,
    pub(crate) connection: Arc<Mutex<Box<dyn GenericConnection + Send>>>,
}

impl MuxConnectionConfig {
    pub fn new(channel: u8, connection: &Connection) -> MuxConnectionConfig {
        MuxConnectionConfig {
            channel,
            connection: connection.internal.clone(),
        }
    }
}

impl fmt::Display for MuxConnectionConfig {
    fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(formatter, "Mux 0x{:02X}, {}", self.channel, self.connection.lock().unwrap().get_config().to_string())
    }
}
