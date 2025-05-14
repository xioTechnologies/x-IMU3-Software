use crate::connection_type::*;
use crate::ffi::helpers::*;
use std::os::raw::c_char;

#[no_mangle]
pub extern "C" fn XIMU3_connection_type_to_string(connection_type: ConnectionType) -> *const c_char {
    str_to_char_ptr(&connection_type.to_string())
}
