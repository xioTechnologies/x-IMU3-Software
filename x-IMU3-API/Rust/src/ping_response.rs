use crate::command_message::*;
use serde_json;
use std::fmt;

#[derive(Clone)]
pub struct PingResponse {
    pub interface: String,
    pub model: String,
    pub serial_number: String,
    pub device_name: String,
}

impl PingResponse {
    pub(crate) fn parse(response: &CommandMessage) -> Option<Self> {
        if response.key != b"ping" {
            return None;
        }

        let ping: serde_json::Value = serde_json::from_slice(&response.value).ok()?;

        Some(Self {
            interface: ping.get("interface").and_then(|value| value.as_str()).unwrap_or("").to_string(),
            model: ping.get("model").and_then(|value| value.as_str()).unwrap_or("").to_string(),
            serial_number: ping.get("sn").and_then(|value| value.as_str()).unwrap_or("").to_string(),
            device_name: ping.get("name").and_then(|value| value.as_str()).unwrap_or("").to_string(),
        })
    }
}

impl fmt::Display for PingResponse {
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        write!(formatter, "{}, {}, {}, {}", self.interface, self.model, self.serial_number, self.device_name)
    }
}
