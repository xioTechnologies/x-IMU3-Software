use std::mem;
use std::os::raw::{c_char, c_void};
use crate::charging_status::*;
use crate::ffi::callback::*;
use crate::ffi::connection_info::*;
use crate::ffi::helpers::*;
use crate::network_announcement::*;

#[repr(C)]
#[derive(Copy, Clone)]
pub struct NetworkAnnouncementMessageC {
    device_name: [c_char; CHAR_ARRAY_SIZE],
    serial_number: [c_char; CHAR_ARRAY_SIZE],
    rssi: i32,
    battery: i32,
    status: ChargingStatus,
    tcp_connection_info: TcpConnectionInfoC,
    udp_connection_info: UdpConnectionInfoC,
}

impl Default for NetworkAnnouncementMessageC {
    fn default() -> NetworkAnnouncementMessageC {
        NetworkAnnouncementMessageC {
            device_name: EMPTY_CHAR_ARRAY,
            serial_number: EMPTY_CHAR_ARRAY,
            rssi: 0,
            battery: 0,
            status: ChargingStatus::NotConnected,
            tcp_connection_info: Default::default(),
            udp_connection_info: Default::default(),
        }
    }
}

impl From<&NetworkAnnouncementMessage> for NetworkAnnouncementMessageC {
    fn from(message: &NetworkAnnouncementMessage) -> Self {
        NetworkAnnouncementMessageC {
            device_name: string_to_char_array(message.device_name.clone()),
            serial_number: string_to_char_array(message.serial_number.clone()),
            rssi: message.rssi,
            battery: message.battery,
            status: message.status,
            tcp_connection_info: (&message.tcp_connection_info).into(),
            udp_connection_info: (&message.udp_connection_info).into(),
        }
    }
}

impl From<NetworkAnnouncementMessageC> for NetworkAnnouncementMessage {
    fn from(message: NetworkAnnouncementMessageC) -> Self {
        NetworkAnnouncementMessage {
            device_name: char_array_to_string(&message.device_name),
            serial_number: char_array_to_string(&message.serial_number),
            rssi: message.rssi,
            battery: message.battery,
            status: message.status,
            tcp_connection_info: message.tcp_connection_info.into(),
            udp_connection_info: message.udp_connection_info.into(),
            expiry: 0,
        }
    }
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
pub extern "C" fn XIMU3_network_announcement_message_to_string(message: NetworkAnnouncementMessageC) -> *const c_char {
    string_to_char_ptr!(NetworkAnnouncementMessage::from(message).to_string())
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
