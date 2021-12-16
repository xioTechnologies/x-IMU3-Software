use std::fmt;
use std::net::Ipv4Addr;

#[derive(Clone)]
pub enum ConnectionInfo {
    UsbConnectionInfo(UsbConnectionInfo),
    SerialConnectionInfo(SerialConnectionInfo),
    TcpConnectionInfo(TcpConnectionInfo),
    UdpConnectionInfo(UdpConnectionInfo),
    BluetoothConnectionInfo(BluetoothConnectionInfo),
    FileConnectionInfo(FileConnectionInfo),
}

impl fmt::Display for ConnectionInfo {
    fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            ConnectionInfo::UsbConnectionInfo(connection_info) => connection_info.fmt(formatter),
            ConnectionInfo::SerialConnectionInfo(connection_info) => connection_info.fmt(formatter),
            ConnectionInfo::TcpConnectionInfo(connection_info) => connection_info.fmt(formatter),
            ConnectionInfo::UdpConnectionInfo(connection_info) => connection_info.fmt(formatter),
            ConnectionInfo::BluetoothConnectionInfo(connection_info) => connection_info.fmt(formatter),
            ConnectionInfo::FileConnectionInfo(connection_info) => connection_info.fmt(formatter),
        }
    }
}

#[derive(Clone)]
pub struct UsbConnectionInfo {
    pub port_name: String,
}

impl fmt::Display for UsbConnectionInfo {
    fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(formatter, "USB - {}", self.port_name)
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
        let enabled_disabled = if self.rts_cts_enabled == true { "Enabled" } else { "Disabled" };
        write!(formatter, "Serial - {}, {}, RTS/CTS {}", self.port_name, self.baud_rate, enabled_disabled)
    }
}

#[derive(Clone)]
pub struct TcpConnectionInfo {
    pub ip_address: Ipv4Addr,
    pub port: u16,
}

impl fmt::Display for TcpConnectionInfo {
    fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(formatter, "TCP - {}:{}", self.ip_address, self.port)
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
        write!(formatter, "UDP - {}:{}, {}", self.ip_address, self.send_port, self.receive_port)
    }
}

#[derive(Clone)]
pub struct BluetoothConnectionInfo {
    pub port_name: String,
}

impl fmt::Display for BluetoothConnectionInfo {
    fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(formatter, "Bluetooth - {}", self.port_name)
    }
}

#[derive(Clone)]
pub struct FileConnectionInfo {
    pub file_path: String,
}

impl fmt::Display for FileConnectionInfo {
    fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(formatter, "File - {}", self.file_path)
    }
}
