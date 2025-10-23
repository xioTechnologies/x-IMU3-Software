use crate::charging_status::*;
use crate::connection_info::*;
use crate::ffi::callback::*;
use crate::ffi::connection_info::*;
use crate::ffi::helpers::*;
use crate::ffi::result::*;
use crate::network_announcement::*;
use std::mem;
use std::net::Ipv4Addr;
use std::os::raw::{c_char, c_void};

#[repr(C)]
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

impl From<&NetworkAnnouncementMessage> for NetworkAnnouncementMessageC {
    fn from(message: &NetworkAnnouncementMessage) -> Self {
        Self {
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
        Self {
            device_name: unsafe { char_array_to_string(&message.device_name) },
            serial_number: unsafe { char_array_to_string(&message.serial_number) },
            ip_address: unsafe { char_array_to_string(&message.ip_address) }.parse().ok().unwrap_or_else(|| Ipv4Addr::new(0, 0, 0, 0)),
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
    TcpConnectionInfoC::from(&TcpConnectionInfo::from(&NetworkAnnouncementMessage::from(message)))
}

#[no_mangle]
pub extern "C" fn XIMU3_network_announcement_message_to_udp_connection_info(message: NetworkAnnouncementMessageC) -> UdpConnectionInfoC {
    UdpConnectionInfoC::from(&UdpConnectionInfo::from(&NetworkAnnouncementMessage::from(message)))
}

#[no_mangle]
pub extern "C" fn XIMU3_network_announcement_message_to_string(message: NetworkAnnouncementMessageC) -> *const c_char {
    str_to_char_ptr(&NetworkAnnouncementMessage::from(message).to_string())
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

        let messages = Self {
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

pub struct NetworkAnnouncementC {
    internal: std::result::Result<NetworkAnnouncement, std::io::Error>,
}

#[no_mangle]
pub extern "C" fn XIMU3_network_announcement_new() -> *mut NetworkAnnouncementC {
    Box::into_raw(Box::new(NetworkAnnouncementC {
        internal: NetworkAnnouncement::new(),
    }))
}

#[no_mangle]
pub extern "C" fn XIMU3_network_announcement_free(network_announcement: *mut NetworkAnnouncementC) {
    unsafe { drop(Box::from_raw(network_announcement)) };
}

#[no_mangle]
pub extern "C" fn XIMU3_network_announcement_get_result(network_announcement: *mut NetworkAnnouncementC) -> Result {
    let network_announcement: &NetworkAnnouncementC = unsafe { &*network_announcement };
    Result::from(&network_announcement.internal)
}

#[no_mangle]
pub extern "C" fn XIMU3_network_announcement_add_callback(network_announcement: *mut NetworkAnnouncementC, callback: Callback<NetworkAnnouncementMessageC>, context: *mut c_void) -> u64 {
    let network_announcement = unsafe { &*network_announcement };
    let void_ptr = VoidPtr(context);
    if let Ok(network_announcement) = &network_announcement.internal {
        network_announcement.add_closure(Box::new(move |message| callback((&message).into(), void_ptr.0)))
    } else {
        0
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_network_announcement_remove_callback(network_announcement: *mut NetworkAnnouncementC, callback_id: u64) {
    let network_announcement = unsafe { &*network_announcement };
    if let Ok(network_announcement) = &network_announcement.internal {
        network_announcement.remove_closure(callback_id);
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_network_announcement_get_messages(network_announcement: *mut NetworkAnnouncementC) -> NetworkAnnouncementMessages {
    let network_announcement = unsafe { &*network_announcement };
    if let Ok(network_announcement) = &network_announcement.internal {
        network_announcement.get_messages().into()
    } else {
        Vec::new().into()
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_network_announcement_get_messages_after_short_delay(network_announcement: *mut NetworkAnnouncementC) -> NetworkAnnouncementMessages {
    let network_announcement = unsafe { &*network_announcement };
    if let Ok(network_announcement) = &network_announcement.internal {
        network_announcement.get_messages_after_short_delay().into()
    } else {
        Vec::new().into()
    }
}
