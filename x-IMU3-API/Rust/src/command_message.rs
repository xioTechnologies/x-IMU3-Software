use crate::receive_error::*;
use serde_json;

#[derive(Clone)]
pub struct CommandMessage {
    pub json: String,
    pub(crate) terminated_json: Vec<u8>,
    pub key: String,
    pub value: String,
    pub error: Option<String>,
}

impl CommandMessage {
    pub(crate) fn parse_internal(json: &[u8]) -> Result<Self, ReceiveError> {
        let value = serde_json::from_slice::<serde_json::Value>(json).map_err(|_| ReceiveError::InvalidJson)?;

        let object = value.as_object().ok_or(ReceiveError::JsonIsNotAnObject)?;

        if object.len() != 1 {
            return Err(ReceiveError::JsonObjectIsNotASingleKeyValuePair);
        }

        let (key, value) = object.iter().next().ok_or(ReceiveError::JsonObjectIsNotASingleKeyValuePair)?;

        let json = serde_json::to_string(object).map_err(|_| ReceiveError::InvalidJson)?;

        let mut command_message = Self {
            json: json.clone(),
            terminated_json: format!("{json}\n").into_bytes(),
            key: key.to_string(),
            value: value.to_string(),
            error: None,
        };

        if let Some(error_object) = value.as_object() {
            command_message.error = error_object.get("error").and_then(|value| value.as_str()).map(String::from);
        }

        Ok(command_message)
    }

    pub fn parse(json: &str) -> Self {
        Self::parse_internal(json.as_bytes()).unwrap_or(Self {
            json: "".to_owned(),
            terminated_json: b"".to_vec(),
            key: "".to_owned(),
            value: "".to_owned(),
            error: None,
        })
    }
}
