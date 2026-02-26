use ximu3::connection::*;
use ximu3::device::*;
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

    // Mux scanner (blocking)
    let mux_devices = MuxScanner::scan(&usb_connection, MAX_NUMBER_OF_MUX_CHANNELS, DEFAULT_RETRIES, DEFAULT_TIMEOUT);

    print_devices(mux_devices);

    // Mux scanner (non-blocking)
    let _mux_scanner = MuxScanner::new(
        &usb_connection,
        Box::new(|mux_devices| {
            print_devices(mux_devices);
        }),
    );

    std::thread::sleep(std::time::Duration::from_secs(60));

    // Close connection
    usb_connection.close();
}

fn print_devices(devices: Vec<Device>) {
    println!("{} device(s) found", devices.len());

    for device in devices {
        println!("{}, {}, {}", device.device_name, device.serial_number, device.connection_config);
        // println!("{device}"); // alternative to above
    }
}
