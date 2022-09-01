use std::net::Ipv4Addr;
use ximu3::connection_info::*;
use ximu3::network_announcement::*;
use crate::helpers;
use super::connection;

pub fn run() {
    let connection_info;

    if helpers::yes_or_no("Search for connections?") {
        println!("Searching for connections");

        let messages = NetworkAnnouncement::new().get_messages_after_short_delay();

        if messages.is_empty() {
            println!("No UDP connections available");
            return;
        }

        let message = messages.first().unwrap();

        println!("Found {} - {}", message.device_name, message.serial_number);

        connection_info = message.udp_connection_info.clone();
    } else {
        connection_info = UdpConnectionInfo {
            ip_address: Ipv4Addr::new(192, 168, 1, 1),
            send_port: 9000,
            receive_port: 8000,
        };
    }

    connection::run(ConnectionInfo::UdpConnectionInfo(connection_info));
}
