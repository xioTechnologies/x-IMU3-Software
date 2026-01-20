use super::connection;
use crate::helpers;
use ximu3::connection_config::*;
use ximu3::port_scanner::*;

pub fn run() {
    if helpers::yes_or_no("Search for connections?") {
        let devices = PortScanner::scan_filter(PortType::Serial);

        let Some(device) = devices.first() else {
            println!("No serial connections available");
            return;
        };

        println!("Found {device}");

        let config = &device.connection_config;

        connection::run(config);
    } else {
        let config = &ConnectionConfig::SerialConnectionConfig(SerialConnectionConfig {
            port_name: "COM1".to_owned(),
            baud_rate: 115200,
            rts_cts_enabled: false,
        }); // replace with actual connection config

        connection::run(config);
    }
}
