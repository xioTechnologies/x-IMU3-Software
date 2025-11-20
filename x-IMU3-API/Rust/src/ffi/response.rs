use crate::ffi::helpers::*;
use crate::response::*;
use std::mem;
use std::os::raw::c_char;

#[repr(C)]
pub struct ResponseC {
    pub json: [c_char; CHAR_ARRAY_SIZE],
    pub key: [c_char; CHAR_ARRAY_SIZE],
    pub value: [c_char; CHAR_ARRAY_SIZE],
    pub error: [c_char; CHAR_ARRAY_SIZE],
}

impl From<Response> for ResponseC {
    fn from(response: Response) -> Self {
        Self {
            json: bytes_to_char_array(&response.json),
            key: bytes_to_char_array(&response.key),
            value: bytes_to_char_array(&response.value),
            error: str_to_char_array(response.error.unwrap_or("".to_owned()).as_str()),
        }
    }
}

#[repr(C)]
pub struct Responses {
    array: *mut ResponseC,
    length: u32,
    capacity: u32,
}

impl From<Vec<Option<Response>>> for Responses {
    fn from(messages: Vec<Option<Response>>) -> Self {
        let mut vector: Vec<ResponseC> = messages.iter().map(|message| message.clone().unwrap_or(Response{
            json: vec![],
            key: vec![],
            value: vec![],
            error: None,
        }).into()).collect();

        let responses = Self {
            length: vector.len() as u32,
            array: vector.as_mut_ptr(),
            capacity: vector.capacity() as u32,
        };
        mem::forget(vector);
        responses
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_responses_free(responses: Responses) {
    unsafe {
        let _ = Vec::from_raw_parts(responses.array, responses.length as usize, responses.capacity as usize);
    }
}
