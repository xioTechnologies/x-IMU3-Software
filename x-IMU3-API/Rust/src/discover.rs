use crate::network_announcement::*;
use crate::port_scanner::*;
use std::ops::Drop;
use std::sync::{Arc, Mutex};

pub struct Discover {
    dropped: Arc<Mutex<bool>>,
}

impl Discover {
    pub fn new() -> Self {
        let discover = Self {
            dropped: Arc::new(Mutex::new(false)),
        };

        let dropped = discover.dropped.clone();

        std::thread::spawn(move || loop {
            let port_scanner = PortScanner::new(Box::new(|devices| {
                // TODO
            }));

            let network_announcement = NetworkAnnouncement::new().unwrap();

            network_announcement.add_closure(Box::new(|message| {
                // TODO
            }));

            if let Ok(dropped) = dropped.lock() {
                if *dropped {
                    return;
                }
            }
        });

        discover
    }
}

impl Drop for Discover {
    fn drop(&mut self) {
        *self.dropped.lock().unwrap() = true;
    }
}
