use std::os::raw::c_char;
use crate::ffi::helpers::*;
use crate::statistics::*;

#[no_mangle]
pub extern "C" fn XIMU3_statistics_to_string(statistics: Statistics) -> *const c_char {
    str_to_char_ptr!(&statistics.to_string())
}
