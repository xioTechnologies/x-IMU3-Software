use crate::receive_error::*;
use serde_json;

#[derive(Clone)]
pub struct CommandMessage {
    pub json: Vec<u8>,
    pub key: Vec<u8>,
    pub value: Vec<u8>,
    pub error: Option<String>,
}

impl CommandMessage {
    pub(crate) fn parse(json: &[u8]) -> Result<Self, ReceiveError> {
        let value = serde_json::from_slice::<serde_json::Value>(json).map_err(|_| ReceiveError::InvalidJson)?;

        let object = value.as_object().ok_or(ReceiveError::JsonIsNotAnObject)?;

        if object.len() != 1 {
            return Err(ReceiveError::JsonObjectIsNotASingleKeyValuePair);
        }

        let (key, value) = object.iter().next().ok_or(ReceiveError::JsonObjectIsNotASingleKeyValuePair)?;

        Ok(Self {
            json: serde_json::to_vec(object).map_err(|_| ReceiveError::UnknownError)?,
            key: key.as_bytes().to_vec(),
            value: serde_json::to_vec(value).map_err(|_| ReceiveError::UnknownError)?,
            error: value.as_object().and_then(|object| object.get("error")).and_then(|value| value.as_str()).map(String::from),
        })
    }
}
