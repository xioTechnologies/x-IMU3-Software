use crate::command_message::*;
use crate::ffi::helpers::*;
use std::mem;
use std::os::raw::c_char;

#[repr(C)]
pub struct CommandMessageC {
    pub json: [c_char; CHAR_ARRAY_SIZE],
    pub key: [c_char; CHAR_ARRAY_SIZE],
    pub value: [c_char; CHAR_ARRAY_SIZE],
    pub error: [c_char; CHAR_ARRAY_SIZE],
}

impl From<Option<CommandMessage>> for CommandMessageC {
    fn from(message: Option<CommandMessage>) -> Self {
        match message {
            Some(message) => Self {
                json: bytes_to_char_array(&message.json),
                key: bytes_to_char_array(&message.key),
                value: bytes_to_char_array(&message.value),
                error: str_to_char_array(message.error.unwrap_or("".to_owned()).as_str()),
            },
            None => Self {
                json: EMPTY_CHAR_ARRAY,
                key: EMPTY_CHAR_ARRAY,
                value: EMPTY_CHAR_ARRAY,
                error: EMPTY_CHAR_ARRAY,
            },
        }
    }
}

#[repr(C)]
pub struct CommandMessages {
    array: *mut CommandMessageC,
    length: u32,
    capacity: u32,
}

impl From<Vec<Option<CommandMessage>>> for CommandMessages {
    fn from(messages: Vec<Option<CommandMessage>>) -> Self {
        let mut vector: Vec<CommandMessageC> = messages.iter().map(|message| message.clone().into()).collect();

        let messages = Self {
            length: vector.len() as u32,
            array: vector.as_mut_ptr(),
            capacity: vector.capacity() as u32,
        };
        mem::forget(vector);
        messages
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_command_messages_free(messages: CommandMessages) {
    unsafe {
        let _ = Vec::from_raw_parts(messages.array, messages.length as usize, messages.capacity as usize);
    }
}
