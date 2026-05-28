use crate::connection::Connection;
use crate::ffi::callback::*;
use crate::ffi::device::*;
use crate::mux_scanner::*;
use std::os::raw::c_void;

#[no_mangle]
pub extern "C" fn XIMU3_mux_scanner_new(connection: *mut Connection) -> *mut MuxScanner<'static> {
    let connection = unsafe { &*connection };
    Box::into_raw(Box::new(MuxScanner::new(connection)))
}

#[no_mangle]
pub extern "C" fn XIMU3_mux_scanner_free(mux_scanner: *mut MuxScanner) {
    unsafe { drop(Box::from_raw(mux_scanner)) };
}

#[no_mangle]
pub extern "C" fn XIMU3_mux_scanner_add_callback(mux_scanner: *mut MuxScanner, callback: Callback<Devices>, context: *mut c_void) -> u64 {
    let mux_scanner = unsafe { &*mux_scanner };
    let void_ptr = VoidPtr(context);
    mux_scanner.add_closure(Box::new(move |devices| callback(devices.into(), void_ptr.0)))
}

#[no_mangle]
pub extern "C" fn XIMU3_mux_scanner_remove_callback(mux_scanner: *mut MuxScanner, id: u64) {
    let mux_scanner = unsafe { &*mux_scanner };
    mux_scanner.remove_closure(id);
}

#[no_mangle]
pub extern "C" fn XIMU3_mux_scanner_get_devices(mux_scanner: *mut MuxScanner) -> Devices {
    let mux_scanner = unsafe { &*mux_scanner };
    mux_scanner.get_devices().into()
}

#[no_mangle]
pub extern "C" fn XIMU3_mux_scanner_scan(connection: *mut Connection, number_of_channels: u32, retries: u32, timeout: u32) -> Devices {
    let connection = unsafe { &*connection };
    MuxScanner::scan(connection, number_of_channels, retries, timeout).into()
}
