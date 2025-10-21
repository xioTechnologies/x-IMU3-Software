use crate::command_message::*;
use crate::ffi::helpers::*;
use std::os::raw::c_char;
use std::slice;

#[repr(C)]
pub struct CommandMessageC {
    pub json: [c_char; CHAR_ARRAY_SIZE],
    pub key: [c_char; CHAR_ARRAY_SIZE],
    pub value: [c_char; CHAR_ARRAY_SIZE],
    pub error: [c_char; CHAR_ARRAY_SIZE],
}

impl From<CommandMessage> for CommandMessageC {
    fn from(command_message: CommandMessage) -> Self {
        Self {
            json: str_to_char_array(&command_message.json),
            key: str_to_char_array(&command_message.key),
            value: str_to_char_array(&command_message.value),
            error: str_to_char_array(&command_message.error.unwrap_or("".to_owned())),
        }
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_command_message_parse(json: *const c_char) -> CommandMessageC {
    let json = unsafe { char_ptr_to_string(json) };
    CommandMessage::parse(json.as_str()).into()
}

#[no_mangle]
pub extern "C" fn XIMU3_bytes_to_json_string(bytes: *const u8, length: u32) -> *const c_char {
    let bytes = unsafe { slice::from_raw_parts(bytes, length as usize) };
    str_to_char_ptr(CommandMessage::bytes_to_json_string(bytes).as_str())
}
