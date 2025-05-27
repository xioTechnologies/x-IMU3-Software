use ximu3::connection::*;
use ximu3::connection_type::*;
use ximu3::decode_error::*;
use ximu3::mux_message::*;
use ximu3::port_scanner::*;
use ximu3::statistics::*;

fn main() {
    let devices = PortScanner::scan_filter(ConnectionType::Usb);

    if devices.is_empty() {
        println!("No USB connections available");
        return;
    }

    let device = devices.first().unwrap();

    println!("Found {} {}", device.device_name, device.serial_number);

    let connection_info = &device.connection_info;

    let connection = Connection::new(&connection_info);

    connection.add_decode_error_closure(Box::new(decode_error_closure));
    connection.add_statistics_closure(Box::new(statistics_closure));
    connection.add_mux_closure(Box::new(mux_closure));

    // Open connection
    std::thread::sleep(std::time::Duration::from_secs(1));

    if connection.open().is_err() {
        println!("Unable to open {}", connection_info.to_string());
        return;
    }

    // Send command to strobe LED
    connection.send_commands(vec!["{\"haptic\":12}"], 2, 500);

    // Close connection
    std::thread::sleep(std::time::Duration::from_secs(60));
    connection.close();
}
macro_rules! timestamp_fmt {
    () => {
        "{:>8} us"
    };
}

macro_rules! int_fmt {
    () => {
        " {:>8}"
    };
}

pub fn decode_error_closure(error: DecodeError) {
    println!("{}", error);
}

pub fn statistics_closure(statistics: Statistics) {
    #[rustfmt::skip]
    println!(concat!(timestamp_fmt!(), int_fmt!(), " bytes", int_fmt!(), " bytes/s", int_fmt!(), " messages", int_fmt!(), " messages/s", int_fmt!(), " errors", int_fmt!(), " errors/s"),
             statistics.timestamp,
             statistics.data_total,
             statistics.data_rate,
             statistics.message_total,
             statistics.message_rate,
             statistics.error_total,
             statistics.error_rate);
    // println!("{}", statistics); // alternative to above
}

pub fn mux_closure(message: MuxMessage) {
    println!("{}", message);
}
