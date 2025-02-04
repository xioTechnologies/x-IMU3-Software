use ximu3::port_scanner::*;
use crate::helpers;

pub fn run() {
    if helpers::yes_or_no("Use async implementation?") {
        let _port_scanner = PortScanner::new(Box::new(|devices| {
            println!("Devices updated ({} devices available)", devices.len());

            print_devices(devices);
        }));

        std::thread::sleep(std::time::Duration::from_secs(60));
    } else {
        let devices = PortScanner::scan();

        println!("Found {} devices", devices.len());

        print_devices(devices);
    }
}

fn print_devices(devices: Vec<Device>) {
    for device in devices {
        println!("{}, {}, {}", device.device_name, device.serial_number, device.connection_info);
        // println!("{}", device); // alternative to above
    }
}
