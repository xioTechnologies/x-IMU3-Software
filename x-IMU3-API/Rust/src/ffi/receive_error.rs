use crate::ffi::helpers::*;
use crate::receive_error::*;
use std::os::raw::c_char;

#[no_mangle]
pub extern "C" fn XIMU3_receive_error_to_string(receive_error: ReceiveError) -> *const c_char {
    str_to_char_ptr(&receive_error.to_string())
}
