use crate::connection_config::*;
use crate::connection_status::*;
use crate::receiver::*;
use std::sync::{Arc, Mutex};

pub trait GenericConnection {
    fn open(&mut self) -> crossbeam::channel::Receiver<std::io::Result<()>>;
    fn close(&self);
    fn get_config(&self) -> ConnectionConfig;
    fn get_status(&self) -> ConnectionStatus;
    fn get_receiver(&self) -> Arc<Mutex<Receiver>>;
    fn get_write_sender(&self) -> Option<crossbeam::channel::Sender<Vec<u8>>>;
}
