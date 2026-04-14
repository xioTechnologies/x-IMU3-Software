use ximu3::device::*;
use ximu3::port_scanner::*;

pub fn run() {
    // Blocking
    let devices = PortScanner::scan();

    print_devices(devices);

    // Non-blocking
    let _port_scanner = PortScanner::new(Box::new(|devices| {
        print_devices(devices);
    }));

    std::thread::sleep(std::time::Duration::from_secs(60));
}

fn print_devices(devices: Vec<Device>) {
    println!("{} device(s) found", devices.len());

    for device in devices {
        println!("{}, {}, {}", device.device_name, device.serial_number, device.connection_config);
        // println!("{device}"); // alternative to above
    }
}
