use crate::connection::*;
use crate::demux::*;
use crate::ffi::result::*;

pub struct DemuxC {
    internal: std::io::Result<Demux<'static>>,
}

#[no_mangle]
pub extern "C" fn XIMU3_demux_new(connection: *mut Connection, channels: *const u8, length: u32) -> *mut DemuxC {
    let connection = unsafe { &*connection };
    let channels = unsafe { std::slice::from_raw_parts(channels, length as usize) };
    Box::into_raw(Box::new(DemuxC {
        internal: Demux::new(connection, channels.into()),
    }))
}

#[no_mangle]
pub extern "C" fn XIMU3_demux_free(demux: *mut DemuxC) {
    unsafe { drop(Box::from_raw(demux)) };
}

#[no_mangle]
pub extern "C" fn XIMU3_demux_get_result(demux: *mut DemuxC) -> Result {
    let demux = unsafe { &*demux };
    Result::from(&demux.internal)
}
