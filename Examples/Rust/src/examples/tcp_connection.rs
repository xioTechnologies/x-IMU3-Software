use std::net::Ipv4Addr;
use ximu3::connection_info::*;
use ximu3::network_announcement::*;
use crate::helpers;
use super::connection;

pub fn run() {
    let connection_info;

    if helpers::ask_question("Search for connections?") {
        println!("Searching for connections");

        let network_announcement = NetworkAnnouncement::new();

        if network_announcement.is_err() {
            println!("Unable to open network announcement socket");
            return;
        }

        let messages = network_announcement.unwrap().get_messages_after_short_delay();

        if messages.is_empty() {
            println!("No TCP connections available");
            return;
        }

        let message = messages.first().unwrap();

        println!("Found {} {}", message.device_name, message.serial_number);

        connection_info = message.into();
    } else {
        connection_info = TcpConnectionInfo {
            ip_address: Ipv4Addr::new(192, 168, 1, 1),
            port: 7000,
        };
    }

    connection::run(ConnectionInfo::TcpConnectionInfo(connection_info));
}
