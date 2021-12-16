use std::os::raw::{c_char, c_void};
use crate::connection::*;
use crate::data_logger::*;
use crate::ffi::callback::*;
use crate::ffi::helpers::*;
use crate::ffi::result::*;

#[no_mangle]
pub extern "C" fn XIMU3_data_logger_new(directory: *const c_char, name: *const c_char, connections: *const *mut Connection, length: u32, callback: Callback<Result>, context: *mut c_void) -> *mut DataLogger<'static> {
    let connections = connection_array_to_vec(connections, length);
    let void_ptr = VoidPtr(context);
    Box::into_raw(Box::new(DataLogger::new(char_ptr_to_str(directory), char_ptr_to_str(name), connections, Box::new(move |result| {
        match result {
            Ok(()) => callback(Result::Ok, void_ptr.0),
            Err(_) => callback(Result::Error, void_ptr.0),
        }
    }))))
}

#[no_mangle]
pub extern "C" fn XIMU3_data_logger_free(data_logger: *mut DataLogger) {
    unsafe { Box::from_raw(data_logger) };
}

#[no_mangle]
pub extern "C" fn XIMU3_data_logger_log(directory: *const c_char, name: *const c_char, connections: *const *mut Connection, length: u32, seconds: u32) -> Result {
    let connections = connection_array_to_vec(connections, length);
    match DataLogger::log(char_ptr_to_str(directory), char_ptr_to_str(name), connections, seconds) {
        Ok(()) => Result::Ok,
        Err(_) => Result::Error,
    }
}

pub fn connection_array_to_vec(connections: *const *mut Connection, length: u32) -> Vec<&'static mut Connection> {
    let mut vec_connections = Vec::new();

    for index in 0..length {
        let connection = unsafe { &mut **connections.offset(index as isize) };
        vec_connections.push(connection);
    }
    vec_connections
}
