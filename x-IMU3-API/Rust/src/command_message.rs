use serde_json;
use crate::decode_error::*;

#[derive(Clone)]
pub struct CommandMessage {
    pub json: String,
    pub terminated_json: String,
    pub key: String,
}

impl CommandMessage {
    pub fn parse_bytes(bytes: &[u8]) -> Result<CommandMessage, DecodeError> {
        match std::str::from_utf8(bytes) {
            Ok(json) => Self::parse_json(json),
            Err(_) => Err(DecodeError::InvalidUtf8),
        }
    }

    pub fn parse_json(json: &str) -> Result<CommandMessage, DecodeError> {
        match serde_json::from_str::<serde_json::Value>(json) {
            Ok(value) => {
                match value.as_object() {
                    None => Err(DecodeError::JsonIsNotAnObject),
                    Some(object) => {
                        if object.len() != 1 {
                            return Err(DecodeError::JsonObjectIsNotASingleKeyValuePair);
                        }

                        let json = serde_json::to_string(object).unwrap();
                        let terminated_json = format!("{}\r\n", json);
                        let key = object.keys().nth(0).unwrap().to_owned();

                        Ok(CommandMessage { json, terminated_json, key })
                    }
                }
            }
            Err(_) => Err(DecodeError::InvalidJson)
        }
    }
}
