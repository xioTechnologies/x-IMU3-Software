use std::cell::RefCell;
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

impl From<Vec<Vec<u8>>> for CharArrays {
    fn from(bytes: Vec<Vec<u8>>) -> Self {
        let mut vector: Vec<[c_char; CHAR_ARRAY_SIZE]> = bytes.iter().map(|bytes| bytes_to_char_array(bytes)).collect();

        let char_arrays = Self {
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
        let _ = Vec::from_raw_parts(char_arrays.array, char_arrays.length as usize, char_arrays.capacity as usize);
    }
}

thread_local! {
    static CHAR_ARRAY: RefCell<[c_char; CHAR_ARRAY_SIZE]> = RefCell::new(EMPTY_CHAR_ARRAY);
}

pub fn str_to_char_ptr(string: &str) -> *const c_char {
    CHAR_ARRAY.with(|char_array| {
        let mut array = char_array.borrow_mut();
        *array = str_to_char_array(string);
        array.as_ptr()
    }) // returns a pointer valid until the next call of this function on the same thread
}

pub fn bytes_to_char_array(bytes: &[u8]) -> [c_char; CHAR_ARRAY_SIZE] {
    let mut char_array = EMPTY_CHAR_ARRAY;

    let bytes = &bytes[..(char_array.len() - 1).min(bytes.len())]; // last character must remain null

    bytes.iter().enumerate().for_each(|(index, &byte)| {
        char_array[index] = byte as c_char;
    });

    char_array
}

pub fn str_to_char_array(string: &str) -> [c_char; CHAR_ARRAY_SIZE] {
    bytes_to_char_array(string.as_bytes())
}

/// # Safety
/// `char_ptr` must point to a valid C string.
pub unsafe fn char_ptr_to_string(char_ptr: *const c_char) -> String {
    let c_str = unsafe { CStr::from_ptr(char_ptr) };
    String::from_utf8_lossy(c_str.to_bytes()).to_string()
}

/// # Safety
/// `char_ptr` must point to a valid C string.
pub unsafe fn char_ptr_to_bytes(char_ptr: *const c_char) -> Vec<u8> {
    let c_str = unsafe { CStr::from_ptr(char_ptr) };
    c_str.to_bytes().to_vec()
}

/// # Safety
/// `char_array` must contain a valid C string.
pub unsafe fn char_array_to_string(char_array: &[c_char]) -> String {
    unsafe { char_ptr_to_string(char_array.as_ptr()) }
}

/// # Safety
/// `char_ptr_array` must be an array of `length` elements.
/// Each element must point to a valid C string.
pub unsafe fn char_ptr_array_to_vec_string(char_ptr_array: *const *const c_char, length: u32) -> Vec<String> {
    let mut vec_string = Vec::new();

    for index in 0..length {
        let char_ptr = unsafe { *char_ptr_array.offset(index as isize) };
        vec_string.push(char_ptr_to_string(char_ptr));
    }

    vec_string
}

/// # Safety
/// `char_ptr_array` must be an array of `length` elements.
/// Each element must point to a valid C string.
pub unsafe fn char_ptr_array_to_vec_bytes(char_ptr_array: *const *const c_char, length: u32) -> Vec<Vec<u8>> {
    let mut vec_bytes = Vec::new();

    for index in 0..length {
        let char_ptr = unsafe { *char_ptr_array.offset(index as isize) };
        vec_bytes.push(char_ptr_to_bytes(char_ptr));
    }

    vec_bytes
}
