use crate::ffi::helpers::*;
use crate::ping_response::*;
use std::os::raw::c_char;

#[repr(C)]
pub struct PingResponseC {
    pub interface: [c_char; CHAR_ARRAY_SIZE],
    pub model: [c_char; CHAR_ARRAY_SIZE],
    pub serial_number: [c_char; CHAR_ARRAY_SIZE],
    pub device_name: [c_char; CHAR_ARRAY_SIZE],
}

impl From<Option<PingResponse>> for PingResponseC {
    fn from(response: Option<PingResponse>) -> Self {
        match response {
            Some(response) => Self {
                interface: str_to_char_array(&response.interface),
                model: str_to_char_array(&response.model),
                serial_number: str_to_char_array(&response.serial_number),
                device_name: str_to_char_array(&response.device_name),
            },
            None => Self {
                interface: EMPTY_CHAR_ARRAY,
                model: EMPTY_CHAR_ARRAY,
                serial_number: EMPTY_CHAR_ARRAY,
                device_name: EMPTY_CHAR_ARRAY,
            },
        }
    }
}

impl From<PingResponseC> for PingResponse {
    fn from(response: PingResponseC) -> Self {
        Self {
            interface: unsafe { char_array_to_string(&response.interface) },
            model: unsafe { char_array_to_string(&response.model) },
            serial_number: unsafe { char_array_to_string(&response.serial_number) },
            device_name: unsafe { char_array_to_string(&response.device_name) },
        }
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_ping_response_to_string(response: PingResponseC) -> *const c_char {
    str_to_char_ptr(&PingResponse::from(response).to_string())
}
