use super::connection;
use crate::helpers;
use std::net::Ipv4Addr;
use ximu3::connection_info::*;
use ximu3::network_announcement::*;

pub fn run() {
    if helpers::yes_or_no("Search for connections?") {
        let network_announcement = NetworkAnnouncement::new();

        if network_announcement.is_err() {
            println!("Unable to open network announcement socket");
            return;
        }

        let messages = network_announcement.unwrap().get_messages_after_short_delay();

        if messages.is_empty() {
            println!("No UDP connections available");
            return;
        }

        let message = messages.first().unwrap();

        println!("Found {} {}", message.device_name, message.serial_number);

        let connection_info = &ConnectionInfo::UdpConnectionInfo(message.into());

        connection::run(connection_info);
    } else {
        let connection_info = &ConnectionInfo::UdpConnectionInfo(UdpConnectionInfo {
            ip_address: Ipv4Addr::new(192, 168, 1, 1),
            send_port: 9000,
            receive_port: 8000,
        }); // replace with actual connection info

        connection::run(connection_info);
    }
}
