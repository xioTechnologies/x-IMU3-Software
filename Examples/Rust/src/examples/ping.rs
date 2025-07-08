use crate::helpers;
use ximu3::connection::*;
use ximu3::ping_response::*;
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

fn print_ping_response(ping_response: Result<PingResponse, ()>) {
    if let Ok(ping_response) = ping_response {
        println!("{}, {}, {}", ping_response.interface, ping_response.device_name, ping_response.serial_number);
        // println!("{ping_response}"); // alternative to above
    } else {
        println!("No response");
    }
}
