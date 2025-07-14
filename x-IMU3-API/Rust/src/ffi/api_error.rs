use crate::api_error::*;
use crate::ffi::helpers::*;
use std::os::raw::c_char;

#[no_mangle]
pub extern "C" fn XIMU3_api_error_to_string(result: ApiError) -> *const c_char {
    str_to_char_ptr(&result.to_string())
} // TODO: rename to XIMU3_result_to_string
