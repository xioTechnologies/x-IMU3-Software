use serde_json;
use std::fmt;

pub struct PingResponse {
    pub interface: String,
    pub device_name: String,
    pub serial_number: String,
}

impl PingResponse {
    pub(crate) fn parse(json: &[u8]) -> Option<Self> {
        let object: serde_json::Value = serde_json::from_slice(json).ok()?;
        let ping = &object["ping"];

        if ping.is_object() == false {
            return Some(Self {
                interface: "".to_string(),
                device_name: "".to_string(),
                serial_number: "".to_string(),
            });
        }

        Some(Self {
            interface: ping.get("interface").and_then(|value| value.as_str()).unwrap_or("").to_string(),
            device_name: ping.get("name").and_then(|value| value.as_str()).unwrap_or("").to_string(),
            serial_number: ping.get("sn").and_then(|value| value.as_str()).unwrap_or("").to_string(),
        })
    }
}

impl fmt::Display for PingResponse {
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        write!(formatter, "{}, {}, {}", self.interface, self.device_name, self.serial_number)
    }
}
