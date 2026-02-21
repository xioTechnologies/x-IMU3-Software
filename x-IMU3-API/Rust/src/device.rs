use crate::connection_config::*;
use std::fmt;

#[derive(Clone)]
pub struct Device {
    pub device_name: String,
    pub serial_number: String,
    pub connection_config: ConnectionConfig,
}

impl fmt::Display for Device {
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        write!(formatter, "{}, {}, {}", self.device_name, self.serial_number, self.connection_config.to_string())
    }
}
