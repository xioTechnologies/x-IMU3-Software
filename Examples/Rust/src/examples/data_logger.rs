use ximu3::connection::*;
use ximu3::connection_type::*;
use ximu3::data_logger::*;
use ximu3::port_scanner::*;
use crate::helpers;

pub fn run() {

    // Open all USB connections
    let mut connections = vec![];

    for device in PortScanner::scan_filter(ConnectionType::Usb) {
        println!("{}", device);

        let mut connection = Connection::new(&device.connection_info);

        if connection.open().is_ok() {
            connections.push(connection);
        } else {
            println!("Unable to open connection");
        }
    }

    // Log data
    let directory = "C:/";
    let name = "Data Logger Example";

    if helpers::yes_or_no("Use async implementation?") {
        let _data_logger = DataLogger::new(directory, name, connections.iter_mut().collect(), Box::new(|result| {
            print_result(result);
        }));

        std::thread::sleep(std::time::Duration::from_secs(3));
    } else {
        print_result(DataLogger::log(directory, name, connections.iter_mut().collect(), 3));
    }

    // Close all connections
    for connection in connections.iter_mut() {
        connection.close();
    }
}

fn print_result(result: Result<(), ()>) {
    match result {
        Ok(()) => println!("Ok"),
        Err(()) => println!("Error"),
    }
}
