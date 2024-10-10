use std::os::raw::c_char;
use crate::ffi::helpers::*;
use crate::command_message::*;

#[repr(C)]
pub struct CommandMessageC {
    pub json: [c_char; CHAR_ARRAY_SIZE],
    pub terminated_json: [c_char; CHAR_ARRAY_SIZE],
    pub key: [c_char; CHAR_ARRAY_SIZE],
    pub value: [c_char; CHAR_ARRAY_SIZE],
    pub error_response: [c_char; CHAR_ARRAY_SIZE],
}

impl Default for CommandMessageC {
    fn default() -> CommandMessageC {
        CommandMessageC {
            json: EMPTY_CHAR_ARRAY,
            terminated_json: EMPTY_CHAR_ARRAY,
            key: EMPTY_CHAR_ARRAY,
            value: EMPTY_CHAR_ARRAY,
            error_response: EMPTY_CHAR_ARRAY,
        }
    }
}

impl From<CommandMessage> for CommandMessageC {
    fn from(command_message: CommandMessage) -> Self {
        CommandMessageC {
            json: str_to_char_array(&command_message.json),
            terminated_json: str_to_char_array(&command_message.terminated_json),
            key: str_to_char_array(&command_message.key),
            value: str_to_char_array(&command_message.value),
            error_response: str_to_char_array(&command_message.error_response.unwrap_or("".to_owned())),
        }
    }
}

impl From<CommandMessageC> for CommandMessage {
    fn from(command_message: CommandMessageC) -> Self {
        CommandMessage {
            json: char_array_to_string(&command_message.json),
            terminated_json: char_array_to_string(&command_message.terminated_json),
            key: char_array_to_string(&command_message.key),
            value: char_array_to_string(&command_message.value),
            error_response: Some(char_array_to_string(&command_message.error_response)),
        }
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_command_message_from_json(json: *const c_char) -> CommandMessageC {
    CommandMessage::from_json(char_ptr_to_str(json)).into()
}
