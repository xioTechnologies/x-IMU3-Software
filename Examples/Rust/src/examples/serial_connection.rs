use super::connection;
use crate::helpers;
use ximu3::connection_info::*;
use ximu3::port_scanner::*;

pub fn run() {
    if helpers::yes_or_no("Search for connections?") {
        let devices = PortScanner::scan_filter(PortType::Serial);

        let Some(device) = devices.first() else {
            println!("No serial connections available");
            return;
        };

        println!("Found {} {}", device.device_name, device.serial_number);

        let connection_info = &device.connection_info;

        connection::run(connection_info);
    } else {
        let connection_info = &ConnectionInfo::SerialConnectionInfo(SerialConnectionInfo {
            port_name: "COM1".to_owned(),
            baud_rate: 115200,
            rts_cts_enabled: false,
        }); // replace with actual connection info

        connection::run(connection_info);
    }
}
