use crate::connection_config::*;
use crate::receiver::*;
use crossbeam::channel::Sender;
use std::sync::{Arc, Mutex};

pub trait GenericConnection {
    fn open(&mut self) -> std::io::Result<()>;
    fn close(&self);
    fn get_config(&self) -> ConnectionConfig;
    fn get_receiver(&self) -> Arc<Mutex<Receiver>>;
    fn get_write_sender(&self) -> Option<Sender<Vec<u8>>>;
}
