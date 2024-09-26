use std::os::raw::c_char;
use crate::connection_status::*;
use crate::ffi::helpers::*;

#[no_mangle]
pub extern "C" fn XIMU3_connection_status_to_string(connection_status: ConnectionStatus) -> *const c_char {
    str_to_char_ptr!(&connection_status.to_string())
}
