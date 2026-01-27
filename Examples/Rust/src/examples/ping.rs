use crate::helpers;
use ximu3::connection::*;
use ximu3::ping_response::*;
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

    // Ping
    if helpers::yes_or_no("Use async implementation?") {
        let closure = Box::new(|response| {
            print_response(response);
        });

        connection.ping_async(closure);

        std::thread::sleep(std::time::Duration::from_secs(3));
    } else {
        print_response(connection.ping());
    }

    // Close connection
    connection.close();
}

fn print_response(response: Option<PingResponse>) {
    match response {
        Some(response) => {
            println!("{}, {}, {}", response.interface, response.device_name, response.serial_number);
            // println!("{response}"); // alternative to above
        }
        None => println!("No response"),
    }
}
