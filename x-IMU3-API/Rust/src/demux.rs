use crate::connection::*;
use std::ops::Drop;
use std::sync::{Arc, Mutex};

pub struct Demux<'a> {
    dropped: Arc<Mutex<bool>>,
    connection: &'a Connection,
    closure_ids: Vec<u64>,
}

impl Demux<'_> {
    pub fn new(connection: &Connection, channels: Vec<u8>) -> Demux {
        for channel in channels {
            println!("Set up each channel {channel}");
        }

        let closure_id = connection.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_mux_closure(Box::new(|message| {
            println!("Forward {message}");
        }));

        let demux = Demux {
            dropped: Arc::new(Mutex::new(false)),
            connection,
            closure_ids: vec![closure_id],
        };

        // TODO:

        let dropped = demux.dropped.clone();
        let connection = connection.internal.clone();

        std::thread::spawn(move || loop {
            // do stuff here

            if let Ok(dropped) = dropped.lock() {
                if *dropped {
                    return;
                }
            }
        });

        demux
    }
}

impl Drop for Demux<'_> {
    fn drop(&mut self) {
        *self.dropped.lock().unwrap() = true;
    }
}
