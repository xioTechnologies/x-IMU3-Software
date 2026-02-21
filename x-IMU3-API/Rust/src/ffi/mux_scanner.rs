use crate::connection::Connection;
use crate::ffi::callback::*;
use crate::ffi::device::*;
use crate::mux_scanner::*;
use std::os::raw::c_void;

#[no_mangle]
pub extern "C" fn XIMU3_mux_scanner_new(connection: *mut Connection, callback: Callback<Devices>, context: *mut c_void) -> *mut MuxScanner {
    let connection = unsafe { &*connection };
    let void_ptr = VoidPtr(context);
    Box::into_raw(Box::new(MuxScanner::new(connection, Box::new(move |devices| callback(devices.into(), void_ptr.0)))))
}

#[no_mangle]
pub extern "C" fn XIMU3_mux_scanner_free(mux_scanner: *mut MuxScanner) {
    unsafe { drop(Box::from_raw(mux_scanner)) };
}

#[no_mangle]
pub extern "C" fn XIMU3_mux_scanner_get_devices(mux_scanner: *mut MuxScanner) -> Devices {
    let mux_scanner = unsafe { &*mux_scanner };
    mux_scanner.get_devices().into()
}

#[no_mangle]
pub extern "C" fn XIMU3_mux_scanner_scan(connection: *mut Connection) -> Devices {
    let connection = unsafe { &*connection };
    MuxScanner::scan(connection).into()
}
