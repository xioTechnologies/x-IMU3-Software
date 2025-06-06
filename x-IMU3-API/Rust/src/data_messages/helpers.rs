use core::slice;
use std::cmp;
use std::os::raw::c_char;

pub const DATA_MESSAGE_CHAR_ARRAY_SIZE: usize = 256;

pub const DATA_MESSAGE_EMPTY_CHAR_ARRAY: [c_char; DATA_MESSAGE_CHAR_ARRAY_SIZE] = ['\0' as c_char; DATA_MESSAGE_CHAR_ARRAY_SIZE];

pub fn string_to_char_array(mut string: String) -> ([c_char; DATA_MESSAGE_CHAR_ARRAY_SIZE], usize) {
    let mut char_array = DATA_MESSAGE_EMPTY_CHAR_ARRAY;

    string.truncate(char_array.len() - 1); // last character must remain null

    string.as_bytes().iter().enumerate().for_each(|(index, &byte)| {
        char_array[index] = byte as c_char;
    });

    (char_array, string.len())
}

pub fn slice_to_char_array(mut slice: &[u8]) -> ([c_char; DATA_MESSAGE_CHAR_ARRAY_SIZE], usize) {
    let mut char_array = DATA_MESSAGE_EMPTY_CHAR_ARRAY;

    slice = &slice[..cmp::min(slice.len(), char_array.len() - 1)]; // last character must remain null

    slice.iter().enumerate().for_each(|(index, &byte)| {
        char_array[index] = byte as c_char;
    });

    (char_array, slice.len())
}

pub fn char_array_to_string(char_array: &[c_char], number_of_bytes: usize) -> String {
    let bytes = unsafe { slice::from_raw_parts(char_array.as_ptr() as *const u8, cmp::min(number_of_bytes, char_array.len())) };

    let string: String = bytes
        .iter()
        .map(|&byte| {
            if (0x20..=0x7E).contains(&byte) {
                byte as char
            } else {
                ' '
            }
        })
        .collect();

    string
}
