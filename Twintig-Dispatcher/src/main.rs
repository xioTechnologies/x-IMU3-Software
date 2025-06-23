#[macro_use]
extern crate colour;

use ximu3::connection::*;
use ximu3::demux::*;
use ximu3::keep_open::*;
use ximu3::port_scanner::*;

fn main() {
    println!("{} v{}", env!("CARGO_PKG_NAME"), env!("CARGO_PKG_VERSION"));

    let mut devices = PortScanner::scan_filter(PortType::Usb);

    devices.retain(|device| device.device_name.contains("Carpus"));

    if devices.is_empty() {
        red_ln!("Carpus not found");
        return;
    }

    let device = devices.first().unwrap();

    println!("Found {} {}", device.device_name, device.serial_number);

    let connection = Connection::new(&device.connection_info);

    let _keep_open = KeepOpen::new(
        &connection,
        Box::new(|status| match status {
            ConnectionStatus::Connected => green_ln!("{status}"),
            ConnectionStatus::Reconnecting => yellow_ln!("{status}"),
        }),
    );

    let demux = Demux::new(&connection, vec![0x41, 0x42, 0x43, 0x44, 0x45]);

    if demux.is_err() {
        red_ln!("Demux failed");
        return;
    }

    loop {
        std::thread::sleep(std::time::Duration::from_secs(1));
    }
}
