use ximu3::network_announcement::*;
use crate::helpers;

pub fn run() {
    let mut network_announcement = NetworkAnnouncement::new();

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
    // println!("{}", message); // alternative to above
}
