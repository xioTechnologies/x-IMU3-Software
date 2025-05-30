use crate::charging_status::*;
use crate::ffi::helpers::*;
use std::os::raw::c_char;

#[no_mangle]
pub extern "C" fn XIMU3_charging_status_from_float(charging_status: f32) -> ChargingStatus {
    ChargingStatus::from(charging_status)
}

#[no_mangle]
pub extern "C" fn XIMU3_charging_status_to_string(charging_status: ChargingStatus) -> *const c_char {
    str_to_char_ptr(&charging_status.to_string())
}
