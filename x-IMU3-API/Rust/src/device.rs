use crate::connection_config::*;
use std::fmt;

#[derive(Clone, PartialEq)]
pub struct Device {
    pub model: String,
    pub serial_number: String,
    pub device_name: String,
    pub connection_config: ConnectionConfig,
}

impl fmt::Display for Device {
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        write!(formatter, "{}, {}, {}, {}", self.model, self.serial_number, self.device_name, self.connection_config.to_string())
    }
}
