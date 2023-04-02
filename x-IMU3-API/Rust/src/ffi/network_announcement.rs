use std::mem;
use std::net::Ipv4Addr;
use std::os::raw::{c_char, c_void};
use crate::charging_status::*;
use crate::ffi::callback::*;
use crate::ffi::connection_info::*;
use crate::ffi::helpers::*;
use crate::network_announcement::*;
use crate::connection_info::*;

#[repr(C)]
#[derive(Copy, Clone)]
pub struct NetworkAnnouncementMessageC {
    device_name: [c_char; CHAR_ARRAY_SIZE],
    serial_number: [c_char; CHAR_ARRAY_SIZE],
    ip_address: [c_char; CHAR_ARRAY_SIZE],
    tcp_port: u16,
    udp_send: u16,
    udp_receive: u16,
    rssi: i32,
    battery: i32,
    charging_status: ChargingStatus,
}

impl Default for NetworkAnnouncementMessageC {
    fn default() -> NetworkAnnouncementMessageC {
        NetworkAnnouncementMessageC {
            device_name: EMPTY_CHAR_ARRAY,
            serial_number: EMPTY_CHAR_ARRAY,
            ip_address: EMPTY_CHAR_ARRAY,
            tcp_port: 0,
            udp_send: 0,
            udp_receive: 0,
            rssi: 0,
            battery: 0,
            charging_status: ChargingStatus::NotConnected,
        }
    }
}

impl From<&NetworkAnnouncementMessage> for NetworkAnnouncementMessageC {
    fn from(message: &NetworkAnnouncementMessage) -> Self {
        NetworkAnnouncementMessageC {
            device_name: str_to_char_array(&message.device_name),
            serial_number: str_to_char_array(&message.serial_number),
            ip_address: str_to_char_array(&message.ip_address.to_string()),
            tcp_port: message.tcp_port,
            udp_send: message.udp_send,
            udp_receive: message.udp_receive,
            rssi: message.rssi,
            battery: message.battery,
            charging_status: message.charging_status,
        }
    }
}

impl From<NetworkAnnouncementMessageC> for NetworkAnnouncementMessage {
    fn from(message: NetworkAnnouncementMessageC) -> Self {
        NetworkAnnouncementMessage {
            device_name: char_array_to_string(&message.device_name),
            serial_number: char_array_to_string(&message.serial_number),
            ip_address: char_array_to_string(&message.ip_address).parse().ok().unwrap_or_else(|| { Ipv4Addr::new(0, 0, 0, 0) }),
            tcp_port: message.tcp_port,
            udp_send: message.udp_send,
            udp_receive: message.udp_receive,
            rssi: message.rssi,
            battery: message.battery,
            charging_status: message.charging_status,
            expiry: 0,
        }
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_network_announcement_message_to_tcp_connection_info(message: NetworkAnnouncementMessageC) -> TcpConnectionInfoC {
    (&Into::<TcpConnectionInfo>::into(&Into::<NetworkAnnouncementMessage>::into(message))).into()
}

#[no_mangle]
pub extern "C" fn XIMU3_network_announcement_message_to_udp_connection_info(message: NetworkAnnouncementMessageC) -> UdpConnectionInfoC {
    (&Into::<UdpConnectionInfo>::into(&Into::<NetworkAnnouncementMessage>::into(message))).into()
}

#[no_mangle]
pub extern "C" fn XIMU3_network_announcement_message_to_string(message: NetworkAnnouncementMessageC) -> *const c_char {
    str_to_char_ptr!(&NetworkAnnouncementMessage::from(message).to_string())
}

#[repr(C)]
pub struct NetworkAnnouncementMessages {
    array: *mut NetworkAnnouncementMessageC,
    length: u32,
    capacity: u32,
}

impl From<Vec<NetworkAnnouncementMessage>> for NetworkAnnouncementMessages {
    fn from(messages: Vec<NetworkAnnouncementMessage>) -> Self {
        let mut vector: Vec<NetworkAnnouncementMessageC> = messages.iter().map(|message| message.into()).collect();

        let messages = NetworkAnnouncementMessages {
            length: vector.len() as u32,
            array: vector.as_mut_ptr(),
            capacity: vector.capacity() as u32,
        };
        mem::forget(vector);
        messages
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_network_announcement_messages_free(messages: NetworkAnnouncementMessages) {
    unsafe {
        Vec::from_raw_parts(messages.array, messages.length as usize, messages.capacity as usize);
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_network_announcement_new() -> *mut NetworkAnnouncement {
    Box::into_raw(Box::new(NetworkAnnouncement::new()))
}

#[no_mangle]
pub extern "C" fn XIMU3_network_announcement_free(network_announcement: *mut NetworkAnnouncement) {
    unsafe { Box::from_raw(network_announcement) };
}

#[no_mangle]
pub extern "C" fn XIMU3_network_announcement_add_callback(network_announcement: *mut NetworkAnnouncement, callback: Callback<NetworkAnnouncementMessageC>, context: *mut c_void) -> u64 {
    let network_announcement: &mut NetworkAnnouncement = unsafe { &mut *network_announcement };
    let void_ptr = VoidPtr(context);
    network_announcement.add_closure(Box::new(move |message| callback((&message).into(), void_ptr.0)))
}

#[no_mangle]
pub extern "C" fn XIMU3_network_announcement_remove_callback(network_announcement: *mut NetworkAnnouncement, callback_id: u64) {
    let network_announcement: &mut NetworkAnnouncement = unsafe { &mut *network_announcement };
    network_announcement.remove_closure(callback_id);
}

#[no_mangle]
pub extern "C" fn XIMU3_network_announcement_get_messages(network_announcement: *mut NetworkAnnouncement) -> NetworkAnnouncementMessages {
    let network_announcement: &mut NetworkAnnouncement = unsafe { &mut *network_announcement };
    network_announcement.get_messages().into()
}

#[no_mangle]
pub extern "C" fn XIMU3_network_announcement_get_messages_after_short_delay(network_announcement: *mut NetworkAnnouncement) -> NetworkAnnouncementMessages {
    let network_announcement: &mut NetworkAnnouncement = unsafe { &mut *network_announcement };
    network_announcement.get_messages_after_short_delay().into()
}
