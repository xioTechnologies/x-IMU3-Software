use super::connection;
use crate::helpers;
use ximu3::connection_info::*;
use ximu3::connection_type::*;
use ximu3::port_scanner::*;

pub fn run() {
    if helpers::yes_or_no("Search for connections?") {
        let devices = PortScanner::scan_filter(ConnectionType::Bluetooth);

        if devices.is_empty() {
            println!("No Bluetooth connections available");
            return;
        }

        let device = devices.first().unwrap();

        println!("Found {} {}", device.device_name, device.serial_number);

        let connection_info = &device.connection_info;

        connection::run(connection_info);
    } else {
        let connection_info = &ConnectionInfo::BluetoothConnectionInfo(BluetoothConnectionInfo {
            port_name: "COM1".to_owned(),
        }); // replace with actual connection info

        connection::run(connection_info);
    }
}
