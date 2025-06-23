use ximu3::connection::*;
use ximu3::demux::*;
use ximu3::port_scanner::*;

pub fn run() {
    // Search for connection
    let devices = PortScanner::scan_filter(PortType::Usb);

    if devices.is_empty() {
        println!("No USB connections available");
        return;
    }

    let device = devices.first().unwrap();

    println!("Found {} {}", device.device_name, device.serial_number);

    // Open connection
    let connection = Connection::new(&device.connection_info);

    if connection.open().is_err() {
        println!("Unable to open connection");
        return;
    }

    // Demux
    let demux = Demux::new(&connection, vec![0x41, 0x42, 0x43]);

    print_result(&demux);

    if demux.is_ok() {
        std::thread::sleep(std::time::Duration::from_secs(60));
    }

    drop(demux);

    // Close connection
    connection.close();
}

fn print_result<T>(result: &std::io::Result<T>) {
    match result {
        Ok(_) => println!("OK"),
        Err(_) => println!("Error"),
    }
}
