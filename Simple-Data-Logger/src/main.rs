#[macro_use]
extern crate colour;

use ximu3::connection::*;
use ximu3::connection_info::*;
use ximu3::data_logger::*;
use ximu3::network_announcement::*;

pub fn main() {
    if let Err(error) = program() {
        red_ln!("{}", error);
    }

    get_input("Press Enter to exit");
}

pub fn program() -> std::io::Result<()> {
    white_ln!("{} v{}", env!("CARGO_PKG_NAME"), env!("CARGO_PKG_VERSION"));

    let current_directory = get_current_directory()?;

    let network_announcement = NetworkAnnouncement::new()?;

    // Find available connections
    let mut messages;

    grey_ln!("Searching for UDP connections");

    'outer: loop {
        messages = network_announcement.get_messages_after_short_delay();

        if messages.is_empty() {
            continue;
        }

        for message in &messages {
            grey_ln!("{message}");
        }

        grey_ln!("Found {} device(s)", messages.len());

        loop {
            match get_input("Do you want to continue? [Y/N]").to_uppercase().as_str() {
                "Y" => break 'outer,
                "N" => continue 'outer,
                _ => continue,
            }
        }
    }

    // Open connections
    let mut connections = vec![];

    for message in messages {
        let connection = Connection::new(&ConnectionInfo::UdpConnectionInfo((&message).into()));

        connection.open()?;

        connections.push(connection);
    }

    // Start logging
    while get_input("Type 'start' to start logging").to_lowercase() != "start" {}

    let name = chrono::Local::now().format("%Y-%m-%d %H-%M-%S").to_string();

    let data_logger = DataLogger::new(&current_directory, &name, connections.iter().collect())?;

    grey_ln!("Logging to {} in {}", &name, current_directory);

    // Stop logging
    while get_input("Type 'stop' to stop logging").to_lowercase() != "stop" {}

    drop(data_logger);

    // Close all connections
    for connection in connections.iter() {
        connection.close();
    }

    green_ln!("Complete");

    Ok(())
}

pub fn get_input(message: &str) -> String {
    println!("{}", message);

    let mut input = String::new();
    std::io::stdin().read_line(&mut input).ok();

    input.trim().to_string()
}

pub fn get_current_directory() -> std::io::Result<String> {
    let path = std::env::current_exe()?;

    let parent = path.parent().ok_or(std::io::Error::new(std::io::ErrorKind::Other, "path.parent() failed"))?;

    Ok(parent.display().to_string())
}
