use crate::connection::*;
use crate::ffi::helpers::*;
use crate::ffi::result::*;
use crate::settings::*;
use std::os::raw::c_char;

#[no_mangle]
pub extern "C" fn XIMU3_settings_backup(file_path: *const c_char, connection: *mut Connection) -> Result {
    let file_path = unsafe { char_ptr_to_string(file_path) };
    let connection = unsafe { &*connection };
    Result::from(&backup(file_path.as_str(), connection))
}

#[no_mangle]
pub extern "C" fn XIMU3_settings_restore(file_path: *const c_char, connection: *mut Connection) -> Result {
    let file_path = unsafe { char_ptr_to_string(file_path) };
    let connection = unsafe { &*connection };
    Result::from(&restore(file_path.as_str(), connection))
}
