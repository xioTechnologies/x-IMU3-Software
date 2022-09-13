use ximu3::connection_info::*;
use ximu3::connection_type::*;
use ximu3::port_scanner::*;
use crate::helpers;
use super::connection;

pub fn run() {
    let connection_info;

    if helpers::yes_or_no("Search for connections?") {
        println!("Searching for connections");

        let devices = PortScanner::scan_filter(ConnectionType::Bluetooth);

        if devices.is_empty() {
            println!("No Bluetooth connections available");
            return;
        }

        let device = devices.first().unwrap();

        println!("Found {} - {}", device.device_name, device.serial_number);

        connection_info = device.connection_info.clone();
    } else {
        connection_info = ConnectionInfo::BluetoothConnectionInfo(BluetoothConnectionInfo {
            port_name: "COM1".to_owned(),
        });
    }

    connection::run(connection_info);
}
