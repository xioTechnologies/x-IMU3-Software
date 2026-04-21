use ximu3::command_message::*;
use ximu3::connection::*;
use ximu3::port_scanner::*;

pub fn run() {
    // Search for connection
    let devices = PortScanner::scan_filter(PortType::Usb);

    std::thread::sleep(std::time::Duration::from_secs(1)); // wait for OS to release port

    let Some(device) = devices.first() else {
        println!("No USB connections available");
        return;
    };

    println!("Found {device}");

    // Open connection
    let connection = Connection::new(&device.connection_config);

    if let Err(error) = connection.open() {
        println!("Unable to open {}. {error}.", connection.get_config());
        return;
    }

    // Send accessory data
    let data = b"Any value \x00 to \xFF\n";

    let data_as_json = CommandMessage::bytes_to_json(data);

    let response = connection.send_command(format!("{{\"accessory\":{data_as_json}}}").into(), DEFAULT_RETRIES, DEFAULT_TIMEOUT);

    match response {
        None => println!("No response"),
        Some(response) => {
            if let Some(error) = response.error {
                println!("{error}");
            } else {
                print_data("TX", data);
            }
        }
    }

    // Receive accessory data
    std::thread::sleep(std::time::Duration::from_secs(1));

    if let Some(message) = connection.get_serial_accessory_message(false) {
        print_data("RX", message.char_array_as_bytes());
    }

    // Close connection
    connection.close();
}

fn print_data(direction: &str, data: &[u8]) {
    println!("{direction} {}", data.iter().map(|d| format!("{:02X}", d)).collect::<Vec<_>>().join(" "));
}
