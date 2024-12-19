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
    let mut string = string.to_string();
    let mut char_array = EMPTY_CHAR_ARRAY;

    string.truncate(char_array.len() - 1); // last character must remain null

    string.as_bytes().iter().enumerate().for_each(|(index, &byte)| {
        char_array[index] = byte as c_char;
    });

    char_array
}

pub fn char_ptr_to_string(char_ptr: *const c_char) -> String {
    let c_str = unsafe { CStr::from_ptr(char_ptr) };
    let bytes = Vec::from(c_str.to_bytes());
    let string = unsafe { String::from_utf8_unchecked(bytes) };

    string
}

pub fn char_array_to_string(char_array: &[c_char]) -> String {
    char_ptr_to_string(char_array.as_ptr())
}

pub fn char_ptr_array_to_vec_string(char_ptr_array: *const *const c_char, length: u32) -> Vec<String> {
    let mut vec_string = Vec::new();

    for index in 0..length {
        let char_ptr = unsafe { *char_ptr_array.offset(index as isize) };
        vec_string.push(char_ptr_to_string(char_ptr));
    }

    vec_string
}
