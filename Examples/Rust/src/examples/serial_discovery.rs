use ximu3::serial_discovery::*;
use crate::helpers;

pub fn run() {
    if helpers::yes_or_no("Use async implementation?") {
        let _discovery = SerialDiscovery::new(Box::new(|devices| {
            println!("Devices updated ({} devices available)", devices.len());
            print_devices(devices);
        }));

        helpers::wait(-1);
    } else {
        let devices = SerialDiscovery::scan(2000);

        println!("Discovered {} devices", devices.len());

        print_devices(devices);
    }
}

fn print_devices(devices: Vec<DiscoveredSerialDevice>) {
    for device in devices {
        println!("{} - {}, {}", device.device_name, device.serial_number, device.connection_info);
        // println!("{}", device); // alternative to above
    }
}
