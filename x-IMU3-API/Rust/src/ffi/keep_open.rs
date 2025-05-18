use std::os::raw::{c_char, c_void};
use crate::connection::*;
use crate::ffi::callback::*;
use crate::ffi::helpers::*;
use crate::keep_open::*;

#[no_mangle]
pub extern "C" fn XIMU3_keep_open_state_to_string(state: ConnectionStatus) -> *const c_char {
    str_to_char_ptr(&state.to_string())
}

#[no_mangle]
pub extern "C" fn XIMU3_keep_open_new(connection: *mut Connection, callback: Callback<ConnectionStatus>, context: *mut c_void) -> *mut KeepOpen<'static> {
    let connection: &Connection = unsafe { &*connection };
    let void_ptr = VoidPtr(context);
    Box::into_raw(Box::new(KeepOpen::new(connection, Box::new(move |state| callback(state, void_ptr.0)))))
}

#[no_mangle]
pub extern "C" fn XIMU3_keep_open_free(keep_open: *mut KeepOpen) {
    unsafe { drop(Box::from_raw(keep_open)) };
}
