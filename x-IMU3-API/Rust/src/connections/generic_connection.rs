use crossbeam::channel::Sender;
use std::sync::{Arc, Mutex};
use crate::connection_info::*;
use crate::connection_status::*;
use crate::decoder::*;

pub trait GenericConnection {
    fn open(&mut self) -> std::io::Result<()>;
    fn close(&self);
    fn get_info(&self) -> ConnectionInfo;
    fn get_status(&self) -> ConnectionStatus;
    fn get_decoder(&self) -> Arc<Mutex<Decoder>>;
    fn get_write_sender(&self) -> Option<Sender<String>>;
}
