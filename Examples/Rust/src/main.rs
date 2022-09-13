// This file was generated by Examples/Rust/src/generate_main.py

use crate::examples::*;

mod examples;
mod helpers;

fn main() {
    println!("Select example");
    println!("A. bluetooth_connection.rs");
    println!("B. commands.rs");
    println!("C. data_logger.rs");
    println!("D. file_converter.rs");
    println!("E. get_port_names.rs");
    println!("F. network_announcement.rs");
    println!("G. open_and_ping.rs");
    println!("H. port_scanner.rs");
    println!("I. serial_connection.rs");
    println!("J. tcp_connection.rs");
    println!("K. udp_connection.rs");
    println!("L. usb_connection.rs");

    match helpers::get_key() {
        'A' => bluetooth_connection::run(),
        'B' => commands::run(),
        'C' => data_logger::run(),
        'D' => file_converter::run(),
        'E' => get_port_names::run(),
        'F' => network_announcement::run(),
        'G' => open_and_ping::run(),
        'H' => port_scanner::run(),
        'I' => serial_connection::run(),
        'J' => tcp_connection::run(),
        'K' => udp_connection::run(),
        'L' => usb_connection::run(),
        _ => {}
    }
}
