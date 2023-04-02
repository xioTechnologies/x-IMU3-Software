use core::slice;
use std::ffi::CStr;
use std::mem;
use std::os::raw::c_char;

pub const CHAR_ARRAY_SIZE: usize = 256;

pub const EMPTY_CHAR_ARRAY: [c_char; CHAR_ARRAY_SIZE] = ['\0' as c_char; CHAR_ARRAY_SIZE];

#[repr(C)]
pub struct CharArrays {
    array: *mut [c_char; CHAR_ARRAY_SIZE],
    length: u32,
    capacity: u32,
}

impl From<Vec<String>> for CharArrays {
    fn from(strings: Vec<String>) -> Self {
        let mut vector: Vec<[c_char; CHAR_ARRAY_SIZE]> = strings.iter().map(|string| str_to_char_array(string)).collect();

        let char_arrays = CharArrays {
            array: vector.as_mut_ptr(),
            length: vector.len() as u32,
            capacity: vector.capacity() as u32,
        };
        mem::forget(vector);
        char_arrays
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_char_arrays_free(char_arrays: CharArrays) {
    unsafe {
        Vec::from_raw_parts(char_arrays.array, char_arrays.length as usize, char_arrays.capacity as usize);
    }
}

macro_rules! str_to_char_ptr { // each invocation of this macro uses a different static variable
    ($string:expr) => {{
        static mut CHAR_ARRAY: [c_char; CHAR_ARRAY_SIZE] = EMPTY_CHAR_ARRAY;
        unsafe {
            CHAR_ARRAY = str_to_char_array($string);
            CHAR_ARRAY.as_ptr()
        }
    }}
}

pub fn str_to_char_array(string: &str) -> [c_char; CHAR_ARRAY_SIZE] {
    let mut char_array = EMPTY_CHAR_ARRAY;

    let mut string = string.to_string();
    string.truncate(char_array.len() - 1); // last character must remain null
    string.as_str().chars().enumerate().for_each(|(index, char)| char_array[index] = char as c_char);
    char_array
}

pub fn char_array_to_string(char_array: &[c_char]) -> String {
    let bytes = unsafe { slice::from_raw_parts(char_array.as_ptr() as *const u8, char_array.len()) };

    if let Some(length) = bytes.iter().position(|&byte| byte == '\0' as u8) {
        String::from_utf8_lossy(&bytes[..length]).to_string()
    } else {
        "".to_owned() // return empty string if char array not null terminated
    }
}

pub fn char_ptr_to_str(char_ptr: *const c_char) -> &'static str {
    let c_str = unsafe { CStr::from_ptr(char_ptr) };

    match c_str.to_str() {
        Ok(str) => str,
        Err(_) => "", // return empty string if invalid UTF-8 data
    }
}

pub fn char_ptr_array_to_vec_str(char_ptr_array: *const *const c_char, length: u32) -> Vec<&'static str> {
    let mut vec_str = Vec::new();

    for index in 0..length {
        let c_str = unsafe { CStr::from_ptr(*char_ptr_array.offset(index as isize)) };

        match c_str.to_str() {
            Ok(str) => vec_str.push(str),
            Err(_) => vec_str.push(""), // set empty string if invalid UTF-8 data
        }
    }
    vec_str
}
