use crate::helpers;
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

        connection.send_commands_async(commands, DEFAULT_RETRIES, DEFAULT_TIMEOUT, closure);

        std::thread::sleep(std::time::Duration::from_secs(3));
    } else {
        print_responses(connection.send_commands(commands, DEFAULT_RETRIES, DEFAULT_TIMEOUT));
    }

    // Close connection
    connection.close();
}

fn print_responses(responses: Vec<Option<CommandMessage>>) {
    for response in responses {
        let Some(response) = response else {
            println!("No response");
            continue;
        };

        if let Some(error) = response.error {
            println!("{error}");
            continue;
        }

        println!("{} : {}", String::from_utf8_lossy(&response.key), String::from_utf8_lossy(&response.value));
    }
}
