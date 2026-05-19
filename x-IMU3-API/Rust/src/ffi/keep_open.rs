use crate::connection::*;
use crate::keep_open::*;

#[no_mangle]
pub extern "C" fn XIMU3_keep_open_new(connection: *mut Connection) -> *mut KeepOpen<'static> {
    let connection = unsafe { &*connection };
    Box::into_raw(Box::new(KeepOpen::new(connection)))
}

#[no_mangle]
pub extern "C" fn XIMU3_keep_open_free(keep_open: *mut KeepOpen) {
    unsafe { drop(Box::from_raw(keep_open)) };
}
