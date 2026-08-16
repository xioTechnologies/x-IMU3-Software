use crate::connection::*;
use crate::keep_open::*;

#[no_mangle]
pub extern "C" fn XIMU3_keep_open_new(connection: *mut Connection) -> *mut KeepOpen {
    let connection = unsafe { &*connection };

    Box::into_raw(Box::new(KeepOpen::new(connection)))
}

#[no_mangle]
pub extern "C" fn XIMU3_keep_open_free(keep_open: *mut KeepOpen) {
    if keep_open.is_null() {
        return;
    }

    unsafe { drop(Box::from_raw(keep_open)) };
}
