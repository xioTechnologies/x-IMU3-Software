use ximu3::command_message::*;
use ximu3::connection::*;
use ximu3::connection_type::*;
use ximu3::port_scanner::*;
use crate::helpers;

pub fn run() {

    // Search for connection
    println!("Searching for connections");

    let devices = PortScanner::scan_filter(ConnectionType::Usb);

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

    // Example commands
    let commands = vec![
        "{\"device_name\":\"Foobar\"}", // write "Foobar" to device name
        "{\"serial_number\":null}", // read serial number
        "{\"firmware_version\":null}", // read firmware version
        "{\"invalid_key\":null}"]; // invalid key to demonstrate an error response

    // Send commands
    if helpers::yes_or_no("Use async implementation?") {
        connection.send_commands_async(commands, 2, 500, Box::new(|responses| {
            print_responses(responses);
        }));

        std::thread::sleep(std::time::Duration::from_secs(3));
    } else {
        print_responses(connection.send_commands(commands, 2, 500));
    }

    // Close connection
    connection.close();
}

fn print_responses(responses: Vec<String>) {
    println!("{} responses received", responses.len());

    for response in responses {
        let response = CommandMessage::parse(&response);

        if let Some(error) = response.error {
            println!("{}", error);
            return;
        }

        println!("{} : {}", response.key, response.value);
    }
}
