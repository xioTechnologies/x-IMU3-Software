use std::os::raw::c_char;
use crate::ffi::helpers::*;
use crate::ping_response::*;

#[repr(C)]
pub struct PingResponseC {
    pub interface: [c_char; CHAR_ARRAY_SIZE],
    pub device_name: [c_char; CHAR_ARRAY_SIZE],
    pub serial_number: [c_char; CHAR_ARRAY_SIZE],
}

impl Default for PingResponseC {
    fn default() -> PingResponseC {
        PingResponseC {
            interface: EMPTY_CHAR_ARRAY,
            device_name: EMPTY_CHAR_ARRAY,
            serial_number: EMPTY_CHAR_ARRAY,
        }
    }
}

impl From<PingResponse> for PingResponseC {
    fn from(ping_response: PingResponse) -> Self {
        PingResponseC {
            interface: string_to_char_array(ping_response.interface.clone()),
            device_name: string_to_char_array(ping_response.device_name.clone()),
            serial_number: string_to_char_array(ping_response.serial_number.clone()),
        }
    }
}

impl From<PingResponseC> for PingResponse {
    fn from(ping_response: PingResponseC) -> Self {
        PingResponse {
            interface: char_array_to_string(&ping_response.interface),
            device_name: char_array_to_string(&ping_response.device_name),
            serial_number: char_array_to_string(&ping_response.serial_number),
        }
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_ping_response_to_string(ping_response: PingResponseC) -> *const c_char {
    string_to_char_ptr!(PingResponse::from(ping_response).to_string())
}
