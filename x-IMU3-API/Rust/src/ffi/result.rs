use std::os::raw::c_char;

#[repr(C)]
pub enum Result {
    Ok,
    Error,
}

#[no_mangle]
pub extern "C" fn XIMU3_result_to_string(result: Result) -> *const c_char {
    match result {
        Result::Ok => "Ok\0".as_ptr() as *const i8,
        Result::Error => "Error\0".as_ptr() as *const i8,
    }
}
