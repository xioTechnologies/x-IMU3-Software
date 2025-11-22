use crate::helpers;
use ximu3::network_announcement::*;

pub fn run() {
    let network_announcement = match NetworkAnnouncement::new() {
        Ok(network_announcement) => network_announcement,
        Err(error) => {
            println!("Network announcement failed. {error}");
            return;
        }
    };

    if helpers::yes_or_no("Use async implementation?") {
        network_announcement.add_closure(Box::new(|message| {
            print_message(message);
        }));

        std::thread::sleep(std::time::Duration::from_secs(60));
    } else {
        for message in network_announcement.get_messages() {
            print_message(message)
        }
    }
}

fn print_message(message: NetworkAnnouncementMessage) {
    #[rustfmt::skip]
    println!("{}, {}, {}, {}, {}, {}, {}%, {}%, {}",
             message.device_name,
             message.serial_number,
             message.ip_address,
             message.tcp_port,
             message.udp_send,
             message.udp_receive,
             message.rssi,
             message.battery,
             message.charging_status);
    // println!("{message}"); // alternative to above
}
