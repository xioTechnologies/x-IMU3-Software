use std::os::raw::c_char;
use crate::ffi::helpers::*;
use crate::command_message::*;

#[repr(C)]
pub struct CommandMessageC {
    pub json: [c_char; CHAR_ARRAY_SIZE],
    pub key: [c_char; CHAR_ARRAY_SIZE],
    pub value: [c_char; CHAR_ARRAY_SIZE],
    pub error: [c_char; CHAR_ARRAY_SIZE],
}

impl From<CommandMessage> for CommandMessageC {
    fn from(command_message: CommandMessage) -> Self {
        CommandMessageC {
            json: str_to_char_array(&command_message.json),
            key: str_to_char_array(&command_message.key),
            value: str_to_char_array(&command_message.value),
            error: str_to_char_array(&command_message.error.unwrap_or("".to_owned())),
        }
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_command_message_parse(json: *const c_char) -> CommandMessageC {
    CommandMessage::parse(char_ptr_to_str(json)).into()
}
