use ximu3::network_announcement::*;
use crate::helpers;

pub fn run() {
    let mut network_announcement = NetworkAnnouncement::new();

    if helpers::yes_or_no("Use async implementation?") {
        network_announcement.add_closure(Box::new(|message| {
            print_message(message);
        }));

        helpers::wait(-1);
    } else {
        for message in network_announcement.get_messages() {
            print_message(message)
        }
    }
}

fn print_message(message: NetworkAnnouncementMessage) {
    println!("{} - {}, RSSI: {}%, Battery: {}%, {}, {}, {}",
             message.device_name,
             message.serial_number,
             message.rssi,
             message.battery,
             message.status,
             message.tcp_connection_info,
             message.udp_connection_info);
    // println!("{}", message); // alternative to above
}
