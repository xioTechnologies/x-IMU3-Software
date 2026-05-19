use crate::connection_status::*;
use crate::ffi::helpers::*;
use std::os::raw::c_char;

#[no_mangle]
pub extern "C" fn XIMU3_connection_status_to_string(status: ConnectionStatus) -> *const c_char {
    str_to_char_ptr(&status.to_string())
}
