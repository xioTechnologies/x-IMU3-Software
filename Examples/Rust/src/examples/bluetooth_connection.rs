use super::connection;
use crate::helpers;
use ximu3::connection_config::*;
use ximu3::port_scanner::*;

pub fn run() {
    if helpers::yes_or_no("Search for connections?") {
        let devices = PortScanner::scan_filter(PortType::Bluetooth);

        let Some(device) = devices.first() else {
            println!("No Bluetooth connections available");
            return;
        };

        println!("Found {device}");

        let config = &device.connection_config;

        connection::run(config);
    } else {
        let config = &ConnectionConfig::BluetoothConnectionConfig(BluetoothConnectionConfig {
            port_name: "COM1".to_owned(),
        }); // replace with actual connection config

        connection::run(config);
    }
}
