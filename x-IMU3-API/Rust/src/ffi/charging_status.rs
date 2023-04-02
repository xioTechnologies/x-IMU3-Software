use std::os::raw::c_char;
use crate::charging_status::*;
use crate::ffi::helpers::*;

#[no_mangle]
pub extern "C" fn XIMU3_charging_status_to_string(charging_status: ChargingStatus) -> *const c_char {
    str_to_char_ptr!(&charging_status.to_string())
}
