use ximu3::connection::*;
use ximu3::port_scanner::*;
use ximu3::settings::*;

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

    // Backup settings
    let file_path = "C:/Users/Public/x-IMU3 Example Settings.json";

    if let Err(error) = backup(file_path, &connection) {
        println!("Backup failed. {error}.");
    }

    // Restore settings
    if let Err(error) = restore(file_path, &connection) {
        println!("Restore failed. {error}.");
    }

    // Save command
    connection.send_command("{\"save\":null}".into(), DEFAULT_RETRIES, DEFAULT_TIMEOUT);

    // Close connection
    connection.close();
}
