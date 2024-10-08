// This file was generated by Examples/Rust/src/generate_main.py

use crate::examples::*;

mod examples;
mod helpers;

fn main() {
    println!("Select example");
    println!("A. bluetooth_connection.rs");
    println!("B. commands.rs");
    println!("C. data_logger.rs");
    println!("D. file_connection.rs");
    println!("E. file_converter.rs");
    println!("F. get_port_names.rs");
    println!("G. network_announcement.rs");
    println!("H. ping.rs");
    println!("I. port_scanner.rs");
    println!("J. serial_connection.rs");
    println!("K. tcp_connection.rs");
    println!("L. udp_connection.rs");
    println!("M. usb_connection.rs");

    match helpers::get_key() {
        'A' => bluetooth_connection::run(),
        'B' => commands::run(),
        'C' => data_logger::run(),
        'D' => file_connection::run(),
        'E' => file_converter::run(),
        'F' => get_port_names::run(),
        'G' => network_announcement::run(),
        'H' => ping::run(),
        'I' => port_scanner::run(),
        'J' => serial_connection::run(),
        'K' => tcp_connection::run(),
        'L' => udp_connection::run(),
        'M' => usb_connection::run(),
        _ => {}
    }
}
