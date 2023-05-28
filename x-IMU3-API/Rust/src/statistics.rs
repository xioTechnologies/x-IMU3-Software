use std::fmt;

#[repr(C)]
#[derive(Clone, Copy)]
pub struct Statistics {
    pub timestamp: u64,
    pub data_total: u64,
    pub data_rate: u32,
    pub message_total: u64,
    pub message_rate: u32,
    pub error_total: u64,
    pub error_rate: u32,
}

impl Default for Statistics {
    fn default() -> Statistics {
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
