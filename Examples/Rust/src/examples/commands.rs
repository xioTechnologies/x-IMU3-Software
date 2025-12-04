use crate::helpers;
use ximu3::command_message::*;
use ximu3::connection::*;
use ximu3::port_scanner::*;

pub fn run() {
    // Search for connection
    let devices = PortScanner::scan_filter(PortType::Usb);

    let Some(device) = devices.first() else {
        println!("No USB connections available");
        return;
    };

    println!("Found {} {}", device.device_name, device.serial_number);

    // Open connection
    let connection = Connection::new(&device.connection_info);

    if let Err(error) = connection.open() {
        println!("Unable to open connection. {error}");
        return;
    }

    // Example commands
    let commands = vec![
        "{\"device_name\":\"Foobar\"}".into(), // write "Foobar" to device name
        "{\"serial_number\":null}".into(),     // read serial number
        "{\"firmware_version\":null}".into(),  // read firmware version
        "{\"invalid_key\":null}".into(),       // invalid key to demonstrate an error response
    ];

    // Send commands
    if helpers::yes_or_no("Use async implementation?") {
        let closure = Box::new(|responses| {
            print_responses(responses);
        });

        connection.send_commands_async(commands, 2, 500, closure);

        std::thread::sleep(std::time::Duration::from_secs(3));
    } else {
        print_responses(connection.send_commands(commands, 2, 500));
    }

    // Close connection
    connection.close();
}

fn print_responses(responses: Vec<Vec<u8>>) {
    println!("{} responses", responses.len());

    for response in responses {
        let response = CommandMessage::parse(&response);

        if let Some(error) = response.error {
            println!("{error}");
            continue;
        }

        println!("{} : {}", String::from_utf8_lossy(&response.key), String::from_utf8_lossy(&response.value));
    }
}
