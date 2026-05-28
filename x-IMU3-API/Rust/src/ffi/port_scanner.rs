use crate::ffi::callback::*;
use crate::ffi::device::*;
use crate::ffi::helpers::*;
use crate::port_scanner::*;
use std::os::raw::{c_char, c_void};

#[no_mangle]
pub extern "C" fn XIMU3_port_type_to_string(port_type: PortType) -> *const c_char {
    str_to_char_ptr(&port_type.to_string())
}

#[no_mangle]
pub extern "C" fn XIMU3_port_scanner_new() -> *mut PortScanner {
    Box::into_raw(Box::new(PortScanner::new()))
}

#[no_mangle]
pub extern "C" fn XIMU3_port_scanner_free(port_scanner: *mut PortScanner) {
    unsafe { drop(Box::from_raw(port_scanner)) };
}

#[no_mangle]
pub extern "C" fn XIMU3_port_scanner_add_callback(port_scanner: *mut PortScanner, callback: Callback<Devices>, context: *mut c_void) -> u64 {
    let port_scanner = unsafe { &*port_scanner };
    let void_ptr = VoidPtr(context);
    port_scanner.add_closure(Box::new(move |devices| callback(devices.into(), void_ptr.0)))
}

#[no_mangle]
pub extern "C" fn XIMU3_port_scanner_remove_callback(port_scanner: *mut PortScanner, id: u64) {
    let port_scanner = unsafe { &*port_scanner };
    port_scanner.remove_closure(id);
}

#[no_mangle]
pub extern "C" fn XIMU3_port_scanner_get_devices(port_scanner: *mut PortScanner) -> Devices {
    let port_scanner = unsafe { &*port_scanner };
    port_scanner.get_devices().into()
}

#[no_mangle]
pub extern "C" fn XIMU3_port_scanner_scan() -> Devices {
    PortScanner::scan().into()
}

#[no_mangle]
pub extern "C" fn XIMU3_port_scanner_scan_filter(port_type: PortType) -> Devices {
    PortScanner::scan_filter(port_type).into()
}

#[no_mangle]
pub extern "C" fn XIMU3_port_scanner_get_port_names() -> CharArrays {
    PortScanner::get_port_names().into()
}
