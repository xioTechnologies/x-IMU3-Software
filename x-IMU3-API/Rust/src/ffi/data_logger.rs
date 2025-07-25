use crate::connection::*;
use crate::data_logger::*;
use crate::ffi::helpers::*;
use crate::ffi::result::*;
use std::os::raw::c_char;

pub struct DataLoggerC {
    internal: std::io::Result<DataLogger<'static>>,
}

#[no_mangle]
pub extern "C" fn XIMU3_data_logger_new(destination: *const c_char, name: *const c_char, connections: *const *mut Connection, length: u32) -> *mut DataLoggerC {
    let connections = connection_array_to_vec(connections, length);
    Box::into_raw(Box::new(DataLoggerC {
        internal: DataLogger::new(char_ptr_to_string(destination).as_str(), char_ptr_to_string(name).as_str(), connections),
    }))
}

#[no_mangle]
pub extern "C" fn XIMU3_data_logger_free(data_logger: *mut DataLoggerC) {
    unsafe { drop(Box::from_raw(data_logger)) };
}

#[no_mangle]
pub extern "C" fn XIMU3_data_logger_get_result(data_logger: *mut DataLoggerC) -> Result {
    let data_logger = unsafe { &*data_logger };
    Result::from(&data_logger.internal)
}

#[no_mangle]
pub extern "C" fn XIMU3_data_logger_log(destination: *const c_char, name: *const c_char, connections: *const *mut Connection, length: u32, seconds: u32) -> Result {
    let connections = connection_array_to_vec(connections, length);
    Result::from(&DataLogger::log(char_ptr_to_string(destination).as_str(), char_ptr_to_string(name).as_str(), connections, seconds))
}

pub fn connection_array_to_vec(connections: *const *mut Connection, length: u32) -> Vec<&'static Connection> {
    let mut vec_connections = Vec::new();

    for index in 0..length {
        let connection = unsafe { &**connections.offset(index as isize) };
        vec_connections.push(connection);
    }
    vec_connections
}
