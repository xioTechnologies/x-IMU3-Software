use std::fmt;

#[repr(C)]
#[derive(Clone, Copy, Debug)]
pub enum ApiError {
    Ok, // Ok variant only exists for FFI
    ErrorA,
    ErrorB,
    ErrorC,
} // ApiError renamed to Result by cbindgen

impl fmt::Display for ApiError {
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        match *self {
            Self::Ok => write!(formatter, "OK"),
            Self::ErrorA => write!(formatter, "Error A"),
            Self::ErrorB => write!(formatter, "Error B"),
            Self::ErrorC => write!(formatter, "Error C"),
        }
    }
}
