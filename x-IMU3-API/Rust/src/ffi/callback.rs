use std::os::raw::c_void;

pub type Callback<T> = extern "C" fn(data: T, context: *mut c_void);

pub struct VoidPtr(pub *mut c_void);

unsafe impl Send for VoidPtr {}
