use std::mem;
use std::os::raw::{c_char, c_void};
use crate::connection_info::*;
use crate::connection_type::*;
use crate::ffi::callback::*;
use crate::ffi::connection_info::*;
use crate::ffi::helpers::*;
use crate::serial_discovery::*;

#[repr(C)]
#[derive(Copy, Clone)]
pub struct DiscoveredSerialDeviceC {
    device_name: [c_char; CHAR_ARRAY_SIZE],
    serial_number: [c_char; CHAR_ARRAY_SIZE],
    connection_type: ConnectionType,
    usb_connection_info: UsbConnectionInfoC,
    serial_connection_info: SerialConnectionInfoC,
    bluetooth_connection_info: BluetoothConnectionInfoC,
}

impl Default for DiscoveredSerialDeviceC {
    fn default() -> DiscoveredSerialDeviceC {
        DiscoveredSerialDeviceC {
            device_name: EMPTY_CHAR_ARRAY,
            serial_number: EMPTY_CHAR_ARRAY,
            connection_type: ConnectionType::Usb,
            usb_connection_info: Default::default(),
            serial_connection_info: Default::default(),
            bluetooth_connection_info: Default::default(),
        }
    }
}

impl From<&DiscoveredSerialDevice> for DiscoveredSerialDeviceC {
    fn from(device: &DiscoveredSerialDevice) -> Self {
        let mut device_c: DiscoveredSerialDeviceC = Default::default();

        device_c.device_name = string_to_char_array(device.device_name.clone());
        device_c.serial_number = string_to_char_array(device.serial_number.clone());

        match &device.connection_info {
            ConnectionInfo::UsbConnectionInfo(connection_info) => {
                device_c.connection_type = ConnectionType::Usb;
                device_c.usb_connection_info = connection_info.into();
            }
            ConnectionInfo::SerialConnectionInfo(connection_info) => {
                device_c.connection_type = ConnectionType::Serial;
                device_c.serial_connection_info = connection_info.into();
            }
            ConnectionInfo::BluetoothConnectionInfo(connection_info) => {
                device_c.connection_type = ConnectionType::Bluetooth;
                device_c.bluetooth_connection_info = connection_info.into();
            }
            _ => {}
        }
        device_c
    }
}

impl From<DiscoveredSerialDeviceC> for DiscoveredSerialDevice {
    fn from(device: DiscoveredSerialDeviceC) -> Self {
        DiscoveredSerialDevice {
            device_name: char_array_to_string(&device.device_name),
            serial_number: char_array_to_string(&device.serial_number),
            connection_info: match device.connection_type {
                ConnectionType::Usb => ConnectionInfo::UsbConnectionInfo(device.usb_connection_info.into()),
                ConnectionType::Serial => ConnectionInfo::SerialConnectionInfo(device.serial_connection_info.into()),
                ConnectionType::Bluetooth => ConnectionInfo::BluetoothConnectionInfo(device.bluetooth_connection_info.into()),
                _ => ConnectionInfo::SerialConnectionInfo(device.serial_connection_info.into()),
            },
        }
    }
}

#[repr(C)]
pub struct DiscoveredSerialDevices {
    array: *const DiscoveredSerialDeviceC,
    length: u32,
}

impl From<Vec<DiscoveredSerialDevice>> for DiscoveredSerialDevices {
    fn from(devices: Vec<DiscoveredSerialDevice>) -> Self {
        let vector: Vec<DiscoveredSerialDeviceC> = devices.iter().map(|device| device.into()).collect();

        let devices = DiscoveredSerialDevices {
            length: vector.len() as u32,
            array: vector.as_ptr(),
        };
        mem::forget(vector);
        devices
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_discovered_serial_devices_free(devices: DiscoveredSerialDevices) {
    if devices.length > 0 {
        drop(devices.array); // TODO: confirm that memory is actually released
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_discovered_serial_device_to_string(device: DiscoveredSerialDeviceC) -> *const c_char {
    string_to_char_ptr!(DiscoveredSerialDevice::from(device).to_string())
}

#[no_mangle]
pub extern "C" fn XIMU3_serial_discovery_new(callback: Callback<DiscoveredSerialDevices>, context: *mut c_void) -> *mut SerialDiscovery {
    let void_ptr = VoidPtr(context);
    Box::into_raw(Box::new(SerialDiscovery::new(Box::new(move |devices| callback(devices.into(), void_ptr.0)))))
}

#[no_mangle]
pub extern "C" fn XIMU3_serial_discovery_free(discovery: *mut SerialDiscovery) {
    unsafe { Box::from_raw(discovery) };
}

#[no_mangle]
pub extern "C" fn XIMU3_serial_discovery_get_devices(discovery: *mut SerialDiscovery) -> DiscoveredSerialDevices {
    let discovery: &mut SerialDiscovery = unsafe { &mut *discovery };
    discovery.get_devices().into()
}

#[no_mangle]
pub extern "C" fn XIMU3_serial_discovery_scan(milliseconds: u32) -> DiscoveredSerialDevices {
    SerialDiscovery::scan(milliseconds).into()
}

#[no_mangle]
pub extern "C" fn XIMU3_serial_discovery_scan_filter(milliseconds: u32, connection_type: ConnectionType) -> DiscoveredSerialDevices {
    SerialDiscovery::scan_filter(milliseconds, connection_type).into()
}

#[no_mangle]
pub extern "C" fn XIMU3_serial_discovery_get_available_ports() -> CharArrays {
    SerialDiscovery::get_available_ports().into()
}
