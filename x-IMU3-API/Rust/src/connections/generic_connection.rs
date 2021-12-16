use crossbeam::channel::Sender;
use std::sync::{Arc, Mutex};
use crate::connection_info::*;
use crate::decoder::*;

pub trait GenericConnection {
    fn open(&mut self) -> std::io::Result<()>;
    fn close(&mut self);
    fn get_info(&mut self) -> ConnectionInfo;
    fn get_decoder(&mut self) -> Arc<Mutex<Decoder>>;
    fn get_write_sender(&mut self) -> Option<Sender<String>>;
}
