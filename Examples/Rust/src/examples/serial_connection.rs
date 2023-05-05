use ximu3::connection_info::*;
use ximu3::connection_type::*;
use ximu3::port_scanner::*;
use crate::helpers;
use super::connection;

pub fn run() {
    let connection_info;

    if helpers::ask_question("Search for connections?") {
        println!("Searching for connections");

        let devices = PortScanner::scan_filter(ConnectionType::Serial);

        if devices.is_empty() {
            println!("No serial connections available");
            return;
        }

        let device = devices.first().unwrap();

        println!("Found {} - {}", device.device_name, device.serial_number);

        connection_info = device.connection_info.clone();
    } else {
        connection_info = ConnectionInfo::SerialConnectionInfo(SerialConnectionInfo {
            port_name: "COM1".to_owned(),
            baud_rate: 115200,
            rts_cts_enabled: false,
        });
    }

    connection::run(connection_info);
}
