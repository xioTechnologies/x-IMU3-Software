use crate::command_message::*;
use crate::ffi::helpers::*;
use std::mem;
use std::os::raw::c_char;

#[no_mangle]
pub extern "C" fn XIMU3_json_type_to_string(json_type: JsonType) -> *const c_char {
    str_to_char_ptr(&json_type.to_string())
}

#[repr(C)]
pub struct CommandMessageC {
    pub json: [c_char; CHAR_ARRAY_SIZE],
    pub key: [c_char; CHAR_ARRAY_SIZE],
    pub value: [c_char; CHAR_ARRAY_SIZE],
    pub value_type: JsonType,
    pub error: [c_char; CHAR_ARRAY_SIZE],
}

impl From<Option<CommandMessage>> for CommandMessageC {
    fn from(message: Option<CommandMessage>) -> Self {
        match message {
            Some(message) => Self {
                json: bytes_to_char_array(&message.json),
                key: bytes_to_char_array(&message.key),
                value: bytes_to_char_array(&message.value),
                value_type: message.value_type,
                error: str_to_char_array(message.error.unwrap_or("".to_owned()).as_str()),
            },
            None => Self {
                json: EMPTY_CHAR_ARRAY,
                key: EMPTY_CHAR_ARRAY,
                value: EMPTY_CHAR_ARRAY,
                value_type: JsonType::Null,
                error: EMPTY_CHAR_ARRAY,
            },
        }
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_command_message_parse(json: *const c_char) -> CommandMessageC {
    let json = unsafe { char_ptr_to_bytes(json) };
    CommandMessage::parse(&json).into()
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

#[no_mangle]
pub extern "C" fn XIMU3_bytes_to_json(bytes: *const u8, length: u32) -> *const c_char {
    // TODO: maybe rename to XIMU3_command_message_bytes_to_json
    let bytes = unsafe { std::slice::from_raw_parts(bytes, length as usize) };
    str_to_char_ptr(CommandMessage::bytes_to_json(bytes).as_str())
}

// TODO
// #[no_mangle]
// pub extern "C" fn XIMU3_bytes_to_json_string_2(bytes: *const u8, length: u32) -> *const c_char {
//     let bytes = unsafe { std::slice::from_raw_parts(bytes, length as usize) };
//     let json = CommandMessage::bytes_to_json_string_2(bytes);
//     unsafe { bytes_to_char_ptr(&json) }
// }
