use ximu3::connection::*;
use ximu3::connection_info::*;

pub fn run() {

    // Create connection info
    let connection_info = ConnectionInfo::UsbConnectionInfo(UsbConnectionInfo {
        port_name: "COM1".to_string(),
    });

    // Open connection
    let mut connection = Connection::new(connection_info);

    if connection.open().is_err() {
        println!("Unable to open connection");
        return;
    }

    // Ping
    if let Ok(ping_response) = connection.ping() {
        println!("{}, {} - {}", ping_response.interface, ping_response.device_name, ping_response.serial_number);
        // println!("{}", ping_response); // alternative to above
    } else {
        println!("Ping failed");
    }

    // Close connection
    connection.close();
}
