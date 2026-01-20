use crate::helpers;
use ximu3::connection::*;
use ximu3::ping_response::*;
use ximu3::port_scanner::*;

pub fn run() {
    // Search for connection
    let devices = PortScanner::scan_filter(PortType::Usb);

    let Some(device) = devices.first() else {
        println!("No USB connections available");
        return;
    };

    println!("Found {device}");

    // Open connection
    let connection = Connection::new(&device.connection_info);

    if let Err(error) = connection.open() {
        println!("Unable to open {}. {error}.", connection.get_info());
        return;
    }

    // Ping
    if helpers::yes_or_no("Use async implementation?") {
        let closure = Box::new(|ping_response| {
            print_ping_response(ping_response);
        });

        connection.ping_async(closure);

        std::thread::sleep(std::time::Duration::from_secs(3));
    } else {
        print_ping_response(connection.ping());
    }

    // Close connection
    connection.close();
}

fn print_ping_response(ping_response: Option<PingResponse>) {
    match ping_response {
        Some(ping_response) => {
            println!("{}, {}, {}", ping_response.interface, ping_response.device_name, ping_response.serial_number);
            // println!("{ping_response}"); // alternative to above
        }
        None => println!("No response"),
    }
}
