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
        // TODO: from_slice requires UTF-8 encoding
        let str = unsafe { String::from_utf8_unchecked(Vec::from(json)) };
        let value = serde_json::from_str::<serde_json::Value>(str.as_str()).map_err(|_| ReceiveError::InvalidJson)?;

        let object = value.as_object().ok_or(ReceiveError::JsonIsNotAnObject)?;

        if object.len() != 1 {
            return Err(ReceiveError::JsonObjectIsNotASingleKeyValuePair);
        }

        let (key, value) = object.iter().next().ok_or(ReceiveError::JsonObjectIsNotASingleKeyValuePair)?;

        let json = serde_json::to_vec(object).map_err(|_| ReceiveError::UnknownError)?;

        let mut command_message = Self {
            json: json.clone(),
            terminated_json: [json.as_slice(), &[b'\n']].concat(),
            key: key.as_bytes().to_vec(),
            value: serde_json::to_vec(value).map_err(|_| ReceiveError::UnknownError)?,
            error: None,
        };

        if let Some(error_object) = value.as_object() {
            command_message.error = error_object.get("error").and_then(|value| value.as_str()).map(String::from);
        }

        Ok(command_message)
    }

    pub fn parse(json: &[u8]) -> Self {
        Self::parse_internal(json).unwrap_or(Self {
            json: vec![],
            terminated_json: vec![],
            key: vec![],
            value: vec![],
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
