use std::os::raw::c_char;

#[repr(C)]
pub enum Result {
    Ok,
    Error,
}

impl From<&std::io::Error> for Result {
    fn from(_: &std::io::Error) -> Self {
        Result::Error
    }
}

impl<T> From<&std::io::Result<T>> for Result {
    fn from(result: &std::io::Result<T>) -> Self {
        match result {
            Ok(_) => Result::Ok,
            Err(error) => error.into(),
        }
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_result_to_string(result: Result) -> *const c_char {
    match result {
        Result::Ok => "OK\0".as_ptr() as *const c_char,
        Result::Error => "Error\0".as_ptr() as *const c_char,
    }
}
