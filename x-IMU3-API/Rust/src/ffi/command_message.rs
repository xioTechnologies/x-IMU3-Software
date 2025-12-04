use crate::command_message::*;
use crate::ffi::helpers::*;
use std::ffi::CStr;
use std::os::raw::c_char;

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
            json: bytes_to_char_array(&command_message.json),
            key: bytes_to_char_array(&command_message.key),
            value: bytes_to_char_array(&command_message.value),
            error: str_to_char_array(command_message.error.unwrap_or("".to_owned()).as_str()),
        }
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_command_message_parse(json: *const c_char) -> CommandMessageC {
    let json = unsafe { CStr::from_ptr(json).to_bytes() };
    CommandMessage::parse(json).into()
}
