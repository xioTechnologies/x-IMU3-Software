use ximu3::connection::*;
use ximu3::data_messages::SerialAccessoryMessage;
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

    if let Err(error) = connection.open() {
        println!("Unable to open connection. {error}");
        return;
    }

    // Loopback
    connection.add_serial_accessory_closure(Box::new(|message: SerialAccessoryMessage| {
        print!("RX ");
        // print_bytes(slice_from_raw_parts(message.char_array, message.number_of_bytes));
        println!();
    }));

    // TODO

    // Close connection
    connection.close();
}

fn print_bytes(bytes: &[u8]) {
    println!("{}", bytes.iter().map(|b| format!("{:02X}", b)).collect::<Vec<_>>().join(" "));
}
