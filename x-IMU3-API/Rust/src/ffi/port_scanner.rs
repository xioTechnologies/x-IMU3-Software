use std::mem;
use std::os::raw::{c_char, c_void};
use crate::connection_info::*;
use crate::connection_type::*;
use crate::ffi::callback::*;
use crate::ffi::connection_info::*;
use crate::ffi::helpers::*;
use crate::port_scanner::*;

#[repr(C)]
#[derive(Copy, Clone)]
pub struct DeviceC {
    device_name: [c_char; CHAR_ARRAY_SIZE],
    serial_number: [c_char; CHAR_ARRAY_SIZE],
    connection_type: ConnectionType,
    usb_connection_info: UsbConnectionInfoC,
    serial_connection_info: SerialConnectionInfoC,
    bluetooth_connection_info: BluetoothConnectionInfoC,
}

impl Default for DeviceC {
    fn default() -> DeviceC {
        DeviceC {
            device_name: EMPTY_CHAR_ARRAY,
            serial_number: EMPTY_CHAR_ARRAY,
            connection_type: ConnectionType::Usb,
            usb_connection_info: Default::default(),
            serial_connection_info: Default::default(),
            bluetooth_connection_info: Default::default(),
        }
    }
}

impl From<&Device> for DeviceC {
    fn from(device: &Device) -> Self {
        let mut device_c: DeviceC = Default::default();

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

impl From<DeviceC> for Device {
    fn from(device: DeviceC) -> Self {
        Device {
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
pub struct Devices {
    array: *mut DeviceC,
    length: u32,
    capacity: u32,
}

impl From<Vec<Device>> for Devices {
    fn from(devices: Vec<Device>) -> Self {
        let mut vector: Vec<DeviceC> = devices.iter().map(|device| device.into()).collect();

        let devices = Devices {
            array: vector.as_mut_ptr(),
            length: vector.len() as u32,
            capacity: vector.capacity() as u32,
        };
        mem::forget(vector);
        devices
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_devices_free(devices: Devices) {
    unsafe {
        Vec::from_raw_parts(devices.array, devices.length as usize, devices.capacity as usize);
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_device_to_string(device: DeviceC) -> *const c_char {
    string_to_char_ptr!(Device::from(device).to_string())
}

#[no_mangle]
pub extern "C" fn XIMU3_port_scanner_new(callback: Callback<Devices>, context: *mut c_void) -> *mut PortScanner {
    let void_ptr = VoidPtr(context);
    Box::into_raw(Box::new(PortScanner::new(Box::new(move |devices| callback(devices.into(), void_ptr.0)))))
}

#[no_mangle]
pub extern "C" fn XIMU3_port_scanner_free(port_scanner: *mut PortScanner) {
    unsafe { Box::from_raw(port_scanner) };
}

#[no_mangle]
pub extern "C" fn XIMU3_port_scanner_get_devices(port_scanner: *mut PortScanner) -> Devices {
    let port_scanner: &mut PortScanner = unsafe { &mut *port_scanner };
    port_scanner.get_devices().into()
}

#[no_mangle]
pub extern "C" fn XIMU3_port_scanner_scan() -> Devices {
    PortScanner::scan().into()
}

#[no_mangle]
pub extern "C" fn XIMU3_port_scanner_scan_filter(connection_type: ConnectionType) -> Devices {
    PortScanner::scan_filter(connection_type).into()
}

#[no_mangle]
pub extern "C" fn XIMU3_port_scanner_get_port_names() -> CharArrays {
    PortScanner::get_port_names().into()
}
