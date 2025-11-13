use crate::receive_error::*;
use serde_json;

#[derive(Clone)]
pub struct CommandMessage {
    pub json: Vec<u8>,
    pub(crate) terminated_json: Vec<u8>,
    pub key: Vec<u8>,
    pub value: Vec<u8>,
    pub error: Option<String>,
}

impl CommandMessage {
    pub(crate) fn parse_internal(json: &[u8]) -> Result<Self, ReceiveError> {
        let value = serde_json::from_slice::<serde_json::Value>(json).map_err(|_| ReceiveError::InvalidJson)?;

        let object = value.as_object().ok_or(ReceiveError::JsonIsNotAnObject)?;

        if object.len() != 1 {
            return Err(ReceiveError::JsonObjectIsNotASingleKeyValuePair);
        }

        let json = serde_json::to_vec(object).unwrap();

        let value = object.values().nth(0).unwrap();

        let mut command_message = Self {
            json: json.clone(),
            terminated_json: [json.as_slice(), &[b'\n']].concat(),
            key: object.keys().nth(0).unwrap().clone().into_bytes(),
            value: serde_json::to_vec(value).unwrap(),
            error: None,
        };

        if let Some(error_object) = value.as_object() {
            command_message.error = error_object.get("error").map(|value| value.as_str().unwrap_or(value.to_string().as_str()).to_owned());
        }

        Ok(command_message)
    }

    pub fn parse(json: &[u8]) -> Self {
        Self::parse_internal(json).unwrap_or(Self {
            json: b"".to_vec(),
            terminated_json: b"".to_vec(),
            key: b"".to_vec(),
            value: b"".to_vec(),
            error: None,
        })
    }
}
