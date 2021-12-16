use ximu3::network_discovery::*;
use crate::helpers;

pub fn run() {
    if helpers::yes_or_no("Use async implementation?") {
        let _discovery = NetworkDiscovery::new(Box::new(|devices| {
            println!("Devices updated ({} devices available)", devices.len());
            print_devices(devices);
        }));

        helpers::wait(-1);
    } else {
        let devices = NetworkDiscovery::scan(2000);

        println!("Discovered {} devices", devices.len());

        print_devices(devices);
    }
}

fn print_devices(devices: Vec<DiscoveredNetworkDevice>) {
    for device in devices {
        println!("{} - {}, RSSI: {}%, Battery: {}%, {}, {}, {}",
                 device.device_name,
                 device.serial_number,
                 device.rssi,
                 device.battery,
                 device.status,
                 device.tcp_connection_info,
                 device.udp_connection_info);
        // println!("{}", device); // alternative to above
    }
}
