use std::net::Ipv4Addr;
use ximu3::connection_info::*;
use ximu3::network_discovery::*;
use crate::helpers;
use super::connection;

pub fn run() {
    let connection_info;

    if helpers::yes_or_no("Search for connections?") {
        println!("Searching for connections");

        let devices = NetworkDiscovery::scan(2000);

        if devices.is_empty() {
            println!("No TCP connections available");
            return;
        }

        let device = devices.first().unwrap();

        println!("Found {} - {}", device.device_name, device.serial_number);

        connection_info = device.tcp_connection_info.clone();
    } else {
        connection_info = TcpConnectionInfo {
            ip_address: Ipv4Addr::new(192, 168, 1, 1),
            port: 7000,
        };
    }

    connection::run(ConnectionInfo::TcpConnectionInfo(connection_info));
}
