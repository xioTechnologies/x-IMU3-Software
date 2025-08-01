use serde::Deserialize;
use serde_json;
use std::fmt;

pub struct PingResponse {
    pub interface: String,
    pub device_name: String,
    pub serial_number: String,
}

impl PingResponse {
    pub(crate) fn parse(json: &str) -> std::io::Result<Self> {
        #[derive(Deserialize)]
        struct ParentObject {
            ping: ChildObject,
        }

        #[derive(Deserialize)]
        struct ChildObject {
            interface: String,
            #[serde(alias = "deviceName")]
            name: String,
            #[serde(alias = "serialNumber")]
            sn: String,
        }

        let object = serde_json::from_str::<ParentObject>(json).map_err(|_| std::io::Error::new(std::io::ErrorKind::InvalidData, "Unable to parse JSON."))?;

        Ok(Self {
            interface: object.ping.interface,
            device_name: object.ping.name,
            serial_number: object.ping.sn,
        })
    }
}

impl fmt::Display for PingResponse {
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        write!(formatter, "{}, {}, {}", self.interface, self.device_name, self.serial_number)
    }
}
