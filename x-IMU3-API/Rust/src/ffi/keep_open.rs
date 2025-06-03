use crate::connection::*;
use crate::ffi::callback::*;
use crate::ffi::helpers::*;
use crate::keep_open::*;
use std::os::raw::{c_char, c_void};

#[no_mangle]
pub extern "C" fn XIMU3_connection_status_to_string(status: ConnectionStatus) -> *const c_char {
    str_to_char_ptr(&status.to_string())
}

#[no_mangle]
pub extern "C" fn XIMU3_keep_open_new(connection: *mut Connection, callback: Callback<ConnectionStatus>, context: *mut c_void) -> *mut KeepOpen<'static> {
    let connection: &Connection = unsafe { &*connection };
    let void_ptr = VoidPtr(context);
    Box::into_raw(Box::new(KeepOpen::new(connection, Box::new(move |status| callback(status, void_ptr.0)))))
}

#[no_mangle]
pub extern "C" fn XIMU3_keep_open_free(keep_open: *mut KeepOpen) {
    unsafe { drop(Box::from_raw(keep_open)) };
}
