use std::os::raw::c_char;
use crate::connection::*;
use crate::data_logger::*;
use crate::ffi::helpers::*;
use crate::ffi::result::*;

pub struct DataLoggerC {
    internal: core::result::Result<DataLogger<'static>, ()>,
}

#[no_mangle]
pub extern "C" fn XIMU3_data_logger_new(directory: *const c_char, name: *const c_char, connections: *const *mut Connection, length: u32) -> *mut DataLoggerC {
    let connections = connection_array_to_vec(connections, length);
    Box::into_raw(Box::new(DataLoggerC { internal: DataLogger::new(char_ptr_to_str(directory), char_ptr_to_str(name), connections) }))
}

#[no_mangle]
pub extern "C" fn XIMU3_data_logger_free(data_logger: *mut DataLoggerC) {
    unsafe { Box::from_raw(data_logger) };
}

#[no_mangle]
pub extern "C" fn XIMU3_data_logger_get_result(data_logger: *mut DataLoggerC) -> Result {
    let data_logger: &DataLoggerC = unsafe { &*data_logger };
    match data_logger.internal {
        Ok(_) => Result::Ok,
        Err(_) => Result::Error,
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_data_logger_log(directory: *const c_char, name: *const c_char, connections: *const *mut Connection, length: u32, seconds: u32) -> Result {
    let connections = connection_array_to_vec(connections, length);
    match DataLogger::log(char_ptr_to_str(directory), char_ptr_to_str(name), connections, seconds) {
        Ok(_) => Result::Ok,
        Err(_) => Result::Error,
    }
}

pub fn connection_array_to_vec(connections: *const *mut Connection, length: u32) -> Vec<&'static Connection> {
    let mut vec_connections = Vec::new();

    for index in 0..length {
        let connection = unsafe { &**connections.offset(index as isize) };
        vec_connections.push(connection);
    }
    vec_connections
}
