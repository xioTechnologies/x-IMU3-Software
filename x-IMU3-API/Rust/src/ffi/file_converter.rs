use crate::ffi::callback::*;
use crate::ffi::helpers::*;
use crate::file_converter::*;
use std::os::raw::{c_char, c_void};

#[no_mangle]
pub extern "C" fn XIMU3_file_converter_status_to_string(status: FileConverterStatus) -> *const c_char {
    str_to_char_ptr(&status.to_string())
}

#[no_mangle]
pub extern "C" fn XIMU3_file_converter_progress_to_string(progress: FileConverterProgress) -> *const c_char {
    str_to_char_ptr(&progress.to_string())
}

#[no_mangle]
pub extern "C" fn XIMU3_file_converter_new(destination: *const c_char, name: *const c_char, file_paths: *const *const c_char, length: u32, callback: Callback<FileConverterProgress>, context: *mut c_void) -> *mut FileConverter {
    let file_paths = char_ptr_array_to_vec_string(file_paths, length);
    let file_paths = file_paths.iter().map(|s| s.as_str()).collect();
    let void_ptr = VoidPtr(context);
    Box::into_raw(Box::new(FileConverter::new(char_ptr_to_string(destination).as_str(), char_ptr_to_string(name).as_str(), file_paths, Box::new(move |progress| callback(progress, void_ptr.0)))))
}

#[no_mangle]
pub extern "C" fn XIMU3_file_converter_free(file_converter: *mut FileConverter) {
    unsafe { drop(Box::from_raw(file_converter)) };
}

#[no_mangle]
pub extern "C" fn XIMU3_file_converter_convert(destination: *const c_char, name: *const c_char, file_paths: *const *const c_char, length: u32) -> FileConverterProgress {
    let file_paths = char_ptr_array_to_vec_string(file_paths, length);
    let file_paths = file_paths.iter().map(|s| s.as_str()).collect();
    FileConverter::convert(char_ptr_to_string(destination).as_str(), char_ptr_to_string(name).as_str(), file_paths)
}
