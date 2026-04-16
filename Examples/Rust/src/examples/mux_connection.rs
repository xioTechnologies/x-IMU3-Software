use super::connection;
use crate::helpers;
use ximu3::connection::*;
use ximu3::connection_config::*;
use ximu3::mux_scanner::*;
use ximu3::port_scanner::*;

pub fn run() {
    // Search for connection
    let usb_devices = PortScanner::scan_filter(PortType::Usb);

    std::thread::sleep(std::time::Duration::from_secs(1)); // wait for OS to release port

    let Some(usb_device) = usb_devices.first() else {
        println!("No USB connections available");
        return;
    };

    println!("Found {usb_device}");

    // Open connection
    let usb_connection = Connection::new(&usb_device.connection_config);

    if let Err(error) = usb_connection.open() {
        println!("Unable to open {}. {error}.", usb_connection.get_config());
        return;
    }

    // Mux connection
    if helpers::yes_or_no("Connect automatically?") {
        let mux_devices = MuxScanner::scan(&usb_connection, MAX_NUMBER_OF_MUX_CHANNELS, DEFAULT_RETRIES, DEFAULT_TIMEOUT);

        let Some(mux_device) = mux_devices.first() else {
            println!("No mux connections available");
            return;
        };

        println!("Found {mux_device}");

        let config = &mux_device.connection_config;

        connection::run(config);
    } else {
        let config = &ConnectionConfig::MuxConnectionConfig(MuxConnectionConfig::new(0x41, &usb_connection)); // replace with actual connection config

        connection::run(config);
    }

    // Close connection
    usb_connection.close();
}
