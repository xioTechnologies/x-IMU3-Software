use super::connection;
use crate::helpers;
use std::net::Ipv4Addr;
use ximu3::connection_info::*;
use ximu3::network_announcement::*;

pub fn run() {
    if helpers::yes_or_no("Search for connections?") {
        let network_announcement = match NetworkAnnouncement::new() {
            Ok(network_announcement) => network_announcement,
            Err(error) => {
                println!("Network announcement failed. {error}.");
                return;
            }
        };

        let messages = network_announcement.get_messages_after_short_delay();

        let Some(message) = messages.first() else {
            println!("No UDP connections available");
            return;
        };

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
