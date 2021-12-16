use std::fmt;
use std::sync::{Arc, Mutex};
use std::time::{SystemTime, UNIX_EPOCH};
use crate::decoder::*;
use crate::dispatcher::*;

#[repr(C)]
#[derive(Copy, Clone)]
pub struct Statistics {
    pub timestamp: u64,
    pub data_total: u64,
    pub data_rate: u32,
    pub message_total: u64,
    pub message_rate: u32,
    pub error_total: u64,
    pub error_rate: u32,
}

impl Statistics {
    pub fn new() -> Statistics {
        Statistics {
            timestamp: 0,
            data_total: 0,
            data_rate: 0,
            message_total: 0,
            message_rate: 0,
            error_total: 0,
            error_rate: 0,
        }
    }

    pub fn start(decoder: Arc<Mutex<Decoder>>) {
        decoder.lock().unwrap().statistics = Statistics::new();

        let initial_time = SystemTime::now().duration_since(UNIX_EPOCH).unwrap().as_micros();
        let mut previous_statistics = Statistics::new();

        std::thread::spawn(move || loop {
            std::thread::sleep(std::time::Duration::from_secs(1));

            if let Ok(mut decoder) = decoder.lock() {
                if decoder.statistics.timestamp == std::u64::MAX {
                    break;
                }

                decoder.statistics.timestamp = (SystemTime::now().duration_since(UNIX_EPOCH).unwrap().as_micros() - initial_time) as u64;

                let delta_time = (decoder.statistics.timestamp - previous_statistics.timestamp) as f32 / 1E6; // TODO: runtime error:
                // thread '<unnamed>' panicked at 'attempt to subtract with overflow', src\statistics.rs:48:34
                // note: run with `RUST_BACKTRACE=1` environment variable to display a backtrace
                // thread '<unnamed>' panicked at 'called `Result::unwrap()` on an `Err` value: "PoisonError { inner: .. }"', src\connections\udp_connection.rs:46:36
                // thread '<unnamed>' panicked at 'called `Result::unwrap()` on an `Err` value: "PoisonError { inner: .. }"', src\connection.rs:244:60

                let delta_data = decoder.statistics.data_total - previous_statistics.data_total;
                let delta_message = decoder.statistics.message_total - previous_statistics.message_total;
                let delta_error = decoder.statistics.error_total - previous_statistics.error_total;

                decoder.statistics.data_rate = (delta_data as f32 / delta_time).round() as u32;
                decoder.statistics.message_rate = (delta_message as f32 / delta_time).round() as u32;
                decoder.statistics.error_rate = (delta_error as f32 / delta_time).round() as u32;

                previous_statistics = decoder.statistics;

                decoder.dispatcher.incoming_sender.send(DispatcherData::Statistics(decoder.statistics)).ok();
            }
        });
    }

    pub fn stop(decoder: Arc<Mutex<Decoder>>) {
        decoder.lock().unwrap().statistics.timestamp = std::u64::MAX;
    }
}

impl fmt::Display for Statistics {
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        write!(formatter, "{:>8} us {:>8} bytes {:>8} bytes/s {:>8} messages {:>8} messages/s {:>8} errors {:>8} errors/s",
               self.timestamp,
               self.data_total,
               self.data_rate,
               self.message_total,
               self.message_rate,
               self.error_total,
               self.error_rate)
    }
}
