use serde_json;
use crate::decode_error::*;

#[derive(Clone)]
pub struct CommandMessage {
    pub json: String,
    pub(crate) terminated_json: String,
    pub key: String,
    pub value: String,
    pub error: Option<String>,
}

impl CommandMessage {
    pub(crate) fn parse_bytes(bytes: &[u8]) -> Result<CommandMessage, DecodeError> {
        match std::str::from_utf8(bytes) {
            Ok(json) => Self::parse_json(json),
            Err(_) => Err(DecodeError::InvalidUtf8),
        }
    }

    pub(crate) fn parse_json(json: &str) -> Result<CommandMessage, DecodeError> {
        let value = serde_json::from_str::<serde_json::Value>(json).map_err(|_| DecodeError::InvalidJson)?;

        let object = value.as_object().ok_or(DecodeError::JsonIsNotAnObject)?;

        if object.len() != 1 {
            return Err(DecodeError::JsonObjectIsNotASingleKeyValuePair);
        }

        let value = object.values().nth(0).unwrap();

        let mut command_message = CommandMessage {
            json: serde_json::to_string(object).unwrap(),
            terminated_json: format!("{}\n", json),
            key: object.keys().nth(0).unwrap().to_owned(),
            value: value.to_string(),
            error: None,
        };

        if let Some(error_object) = value.as_object() {
            command_message.error = error_object.get("error").map(|value| value.as_str().unwrap_or(value.to_string().as_str()).to_owned());
        }

        Ok(command_message)
    }

    pub fn parse(json: &str) -> CommandMessage {
        CommandMessage::parse_json(json).unwrap_or(CommandMessage {
            json: "".to_owned(),
            terminated_json: "".to_owned(),
            key: "".to_owned(),
            value: "".to_owned(),
            error: None,
        })
    }
}
