use super::connection;
use crate::helpers;
use ximu3::connection_info::*;
use ximu3::port_scanner::*;

pub fn run() {
    if helpers::yes_or_no("Search for connections?") {
        let devices = PortScanner::scan_filter(PortType::Usb);

        let Some(device) = devices.first() else {
            println!("No USB connections available");
            return;
        };

        println!("Found {device}");

        let connection_info = &device.connection_info;

        connection::run(connection_info);
    } else {
        let connection_info = &ConnectionInfo::UsbConnectionInfo(UsbConnectionInfo {
            port_name: "COM1".to_owned(),
        }); // replace with actual connection info

        connection::run(connection_info);
    }
}
