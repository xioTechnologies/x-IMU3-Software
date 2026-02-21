use crate::connection_config::*;
use crate::device::*;
use crate::ffi::connection_config::*;
use crate::ffi::connection_type::*;
use crate::ffi::helpers::*;
use std::mem;
use std::os::raw::c_char;

#[repr(C)]
pub struct DeviceC {
    device_name: [c_char; CHAR_ARRAY_SIZE],
    serial_number: [c_char; CHAR_ARRAY_SIZE],
    connection_type: ConnectionType,
    usb_connection_config: UsbConnectionConfigC,
    serial_connection_config: SerialConnectionConfigC,
    bluetooth_connection_config: BluetoothConnectionConfigC,
    mux_connection_config: *mut MuxConnectionConfig,
}

impl Default for DeviceC {
    fn default() -> Self {
        Self {
            device_name: EMPTY_CHAR_ARRAY,
            serial_number: EMPTY_CHAR_ARRAY,
            connection_type: ConnectionType::Usb,
            usb_connection_config: Default::default(),
            serial_connection_config: Default::default(),
            bluetooth_connection_config: Default::default(),
            mux_connection_config: Default::default(),
        }
    }
}

impl From<&Device> for DeviceC {
    fn from(device: &Device) -> Self {
        let mut device_c: Self = Default::default();

        device_c.device_name = str_to_char_array(&device.device_name);
        device_c.serial_number = str_to_char_array(&device.serial_number);

        match &device.connection_config {
            ConnectionConfig::UsbConnectionConfig(config) => {
                device_c.connection_type = ConnectionType::Usb;
                device_c.usb_connection_config = config.into();
            }
            ConnectionConfig::SerialConnectionConfig(config) => {
                device_c.connection_type = ConnectionType::Serial;
                device_c.serial_connection_config = config.into();
            }
            ConnectionConfig::BluetoothConnectionConfig(config) => {
                device_c.connection_type = ConnectionType::Bluetooth;
                device_c.bluetooth_connection_config = config.into();
            }
            // ConnectionConfig::MuxConnectionConfig(config) => {
            //     device_c.connection_type = ConnectionType::Mux;
            //     device_c.mux_connection_config = config.into();
            // } // TODO
            _ => {}
        }
        device_c
    }
}

impl From<DeviceC> for Device {
    fn from(device: DeviceC) -> Self {
        Self {
            device_name: unsafe { char_array_to_string(&device.device_name) },
            serial_number: unsafe { char_array_to_string(&device.serial_number) },
            connection_config: match device.connection_type {
                ConnectionType::Usb => ConnectionConfig::UsbConnectionConfig(device.usb_connection_config.into()),
                ConnectionType::Serial => ConnectionConfig::SerialConnectionConfig(device.serial_connection_config.into()),
                ConnectionType::Bluetooth => ConnectionConfig::BluetoothConnectionConfig(device.bluetooth_connection_config.into()),
                // ConnectionType::Mux => ConnectionConfig::MuxConnectionConfig(device.mux_connection_config.into()), // TODO
                _ => ConnectionConfig::SerialConnectionConfig(device.serial_connection_config.into()),
            },
        }
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_device_to_string(device: DeviceC) -> *const c_char {
    str_to_char_ptr(&Device::from(device).to_string())
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

        let devices = Self {
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
    // TODO: free each mux connection config
    unsafe {
        let _ = Vec::from_raw_parts(devices.array, devices.length as usize, devices.capacity as usize);
    }
}
