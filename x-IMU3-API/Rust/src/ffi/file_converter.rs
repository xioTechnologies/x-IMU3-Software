use std::os::raw::{c_char, c_void};
use crate::file_converter::*;
use crate::ffi::callback::*;
use crate::ffi::helpers::*;

#[no_mangle]
pub extern "C" fn XIMU3_file_converter_status_to_string(status: FileConverterStatus) -> *const c_char {
    str_to_char_ptr!(&status.to_string())
}

#[no_mangle]
pub extern "C" fn XIMU3_file_converter_progress_to_string(progress: FileConverterProgress) -> *const c_char {
    str_to_char_ptr!(&progress.to_string())
}

#[no_mangle]
pub extern "C" fn XIMU3_file_converter_new(destination: *const c_char, source: *const c_char, callback: Callback<FileConverterProgress>, context: *mut c_void) -> *mut FileConverter {
    let void_ptr = VoidPtr(context);
    Box::into_raw(Box::new(FileConverter::new(char_ptr_to_str(destination), char_ptr_to_str(source), Box::new(move |progress| callback(progress, void_ptr.0)))))
}

#[no_mangle]
pub extern "C" fn XIMU3_file_converter_free(file_converter: *mut FileConverter) {
    unsafe { Box::from_raw(file_converter) };
}

#[no_mangle]
pub extern "C" fn XIMU3_file_converter_convert(destination: *const c_char, source: *const c_char) -> FileConverterProgress {
    FileConverter::convert(char_ptr_to_str(destination), char_ptr_to_str(source))
}
