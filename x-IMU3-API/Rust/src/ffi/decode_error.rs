use std::os::raw::c_char;
use crate::decode_error::*;
use crate::ffi::helpers::*;

#[no_mangle]
pub extern "C" fn XIMU3_decode_error_to_string(decode_error: DecodeError) -> *const c_char {
    string_to_char_ptr!(decode_error.to_string())
}
