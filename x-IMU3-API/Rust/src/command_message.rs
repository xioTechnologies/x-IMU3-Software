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

        // TODO: from_slice requires UTF-8 encoding so serial accessory data must encode >0x7E as \uHHHH

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

    pub fn bytes_to_json_string(bytes: &[u8]) -> String {
        // TODO: String must not contain invalid UTF-8 so this return type must change
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
                _ => unsafe { String::from_utf8_unchecked([byte].to_vec()) },
            })
            .collect();

        format!("\"{}\"", string)
    }

    pub fn bytes_to_json_string_2(bytes: &[u8]) -> Vec<u8> {
        let mut json = vec![b'"'];

        for &byte in bytes {
            match byte {
                0x08 => json.extend_from_slice(b"\\b"),
                0x09 => json.extend_from_slice(b"\\t"),
                0x0A => json.extend_from_slice(b"\\n"),
                0x0C => json.extend_from_slice(b"\\f"),
                0x0D => json.extend_from_slice(b"\\r"),
                0x00..=0x1F | 0x7F..=0xFF => json.extend_from_slice(format!("\\u{:04X}", byte).as_bytes()),
                b'"' => json.extend_from_slice(b"\\\""),
                b'\\' => json.extend_from_slice(b"\\\\"),
                _ => json.push(byte),
            }
        }

        json.push(b'"');
        json
    }
}
