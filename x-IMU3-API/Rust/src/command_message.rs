use crate::decode_error::*;
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
    pub(crate) fn parse_bytes(bytes: &[u8]) -> Result<Self, DecodeError> {
        match std::str::from_utf8(bytes) {
            Ok(json) => Self::parse_json(json),
            Err(_) => Err(DecodeError::InvalidUtf8),
        }
    }

    pub(crate) fn parse_json(json: &str) -> Result<Self, DecodeError> {
        let value = serde_json::from_str::<serde_json::Value>(json).map_err(|_| DecodeError::InvalidJson)?;

        let object = value.as_object().ok_or(DecodeError::JsonIsNotAnObject)?;

        if object.len() != 1 {
            return Err(DecodeError::JsonObjectIsNotASingleKeyValuePair);
        }

        let json = serde_json::to_string(object).unwrap();

        let value = object.values().nth(0).unwrap();

        let mut command_message = Self {
            json: json.clone(),
            terminated_json: format!("{json}\n").into_bytes(),
            key: object.keys().nth(0).unwrap().to_owned(),
            value: value.to_string(),
            error: None,
        };

        if let Some(error_object) = value.as_object() {
            command_message.error = error_object.get("error").map(|value| value.as_str().unwrap_or(value.to_string().as_str()).to_owned());
        }

        Ok(command_message)
    }

    pub fn parse(json: &str) -> Self {
        Self::parse_json(json).unwrap_or(Self {
            json: "".to_owned(),
            terminated_json: b"".to_vec(),
            key: "".to_owned(),
            value: "".to_owned(),
            error: None,
        })
    }

    pub fn bytes_to_json_string(bytes: &[u8]) -> String {
        let string: String = bytes
            .iter()
            .map(|&byte| match byte {
                0x00..=0x1F => match byte {
                    0x08 => "\\b".to_string(),
                    0x0C => "\\f".to_string(),
                    b'\n' => "\\n".to_string(),
                    b'\r' => "\\r".to_string(),
                    b'\t' => "\\t".to_string(),
                    _ => format!("\\u{:04X}", byte),
                },
                b'"' => "\\\"".to_string(),
                b'\\' => "\\\\".to_string(),
                _ => unsafe { String::from_utf8_unchecked([byte].to_vec()) }, // TODO: use from_utf8_lossy() instead to avoid safety issues?
            })
            .collect();

        format!("\"{}\"", string)
    }
}
