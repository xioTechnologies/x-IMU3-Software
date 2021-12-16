use std::mem;
use std::os::raw::{c_char, c_void};
use crate::charging_status::*;
use crate::ffi::callback::*;
use crate::ffi::connection_info::*;
use crate::ffi::helpers::*;
use crate::network_discovery::*;

#[repr(C)]
#[derive(Copy, Clone)]
pub struct DiscoveredNetworkDeviceC {
    device_name: [c_char; CHAR_ARRAY_SIZE],
    serial_number: [c_char; CHAR_ARRAY_SIZE],
    rssi: i32,
    battery: i32,
    status: ChargingStatus,
    tcp_connection_info: TcpConnectionInfoC,
    udp_connection_info: UdpConnectionInfoC,
}

impl Default for DiscoveredNetworkDeviceC {
    fn default() -> DiscoveredNetworkDeviceC {
        DiscoveredNetworkDeviceC {
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

impl From<&DiscoveredNetworkDevice> for DiscoveredNetworkDeviceC {
    fn from(device: &DiscoveredNetworkDevice) -> Self {
        DiscoveredNetworkDeviceC {
            device_name: string_to_char_array(device.device_name.clone()),
            serial_number: string_to_char_array(device.serial_number.clone()),
            rssi: device.rssi,
            battery: device.battery,
            status: device.status,
            tcp_connection_info: (&device.tcp_connection_info).into(),
            udp_connection_info: (&device.udp_connection_info).into(),
        }
    }
}

impl From<DiscoveredNetworkDeviceC> for DiscoveredNetworkDevice {
    fn from(device: DiscoveredNetworkDeviceC) -> Self {
        DiscoveredNetworkDevice {
            device_name: char_array_to_string(&device.device_name),
            serial_number: char_array_to_string(&device.serial_number),
            rssi: device.rssi,
            battery: device.battery,
            status: device.status,
            tcp_connection_info: device.tcp_connection_info.into(),
            udp_connection_info: device.udp_connection_info.into(),
            expiry: 0,
        }
    }
}

#[repr(C)]
pub struct DiscoveredNetworkDevices {
    array: *const DiscoveredNetworkDeviceC,
    length: u32,
}

impl From<Vec<DiscoveredNetworkDevice>> for DiscoveredNetworkDevices {
    fn from(devices: Vec<DiscoveredNetworkDevice>) -> Self {
        let vector: Vec<DiscoveredNetworkDeviceC> = devices.iter().map(|device| device.into()).collect();

        let devices = DiscoveredNetworkDevices {
            length: vector.len() as u32,
            array: vector.as_ptr(),
        };
        mem::forget(vector);
        devices
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_discovered_network_devices_free(devices: DiscoveredNetworkDevices) {
    if devices.length > 0 {
        drop(devices.array); // TODO: confirm that memory is actually released
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_discovered_network_device_to_string(device: DiscoveredNetworkDeviceC) -> *const c_char {
    string_to_char_ptr!(DiscoveredNetworkDevice::from(device).to_string())
}

#[no_mangle]
pub extern "C" fn XIMU3_network_discovery_new(callback: Callback<DiscoveredNetworkDevices>, context: *mut c_void) -> *mut NetworkDiscovery {
    let void_ptr = VoidPtr(context);
    Box::into_raw(Box::new(NetworkDiscovery::new(Box::new(move |devices| callback(devices.into(), void_ptr.0)))))
}

#[no_mangle]
pub extern "C" fn XIMU3_network_discovery_free(discovery: *mut NetworkDiscovery) {
    unsafe { Box::from_raw(discovery) };
}

#[no_mangle]
pub extern "C" fn XIMU3_network_discovery_get_devices(discovery: *mut NetworkDiscovery) -> DiscoveredNetworkDevices {
    let discovery: &mut NetworkDiscovery = unsafe { &mut *discovery };
    discovery.get_devices().into()
}

#[no_mangle]
pub extern "C" fn XIMU3_network_discovery_scan(milliseconds: u32) -> DiscoveredNetworkDevices {
    NetworkDiscovery::scan(milliseconds).into()
}
