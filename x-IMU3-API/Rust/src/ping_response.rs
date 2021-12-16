use serde::Deserialize;
use serde_json;
use std::fmt;

pub struct PingResponse {
    pub interface: String,
    pub device_name: String,
    pub serial_number: String,
}

impl PingResponse {
    pub(crate) fn parse_json(json: &str) -> Result<PingResponse, ()> {
        #[derive(Deserialize)]
        struct ParentObject {
            ping: ChildObject,
        }

        #[derive(Deserialize)]
        #[allow(non_snake_case)]
        struct ChildObject {
            interface: String,
            deviceName: String,
            serialNumber: String,
        }

        match serde_json::from_str::<ParentObject>(json) {
            Ok(object) => {
                Ok(PingResponse {
                    interface: object.ping.interface,
                    device_name: object.ping.deviceName,
                    serial_number: object.ping.serialNumber,
                })
            }
            Err(_) => Err(()),
        }
    }
}

impl fmt::Display for PingResponse {
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        write!(formatter, "{}, {} - {}", self.interface, self.device_name, self.serial_number)
    }
}
