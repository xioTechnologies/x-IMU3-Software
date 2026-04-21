use serde_json;
use serde_json::value::RawValue;
use std::fmt;

#[repr(C)]
#[derive(Clone, Copy)]
pub enum JsonType {
    String,
    Number,
    Boolean,
    Null,
    Object,
    Array,
}

impl From<&serde_json::Value> for JsonType {
    fn from(value: &serde_json::Value) -> Self {
        match value {
            serde_json::Value::String(_) => Self::String,
            serde_json::Value::Number(_) => Self::Number,
            serde_json::Value::Bool(_) => Self::Boolean,
            serde_json::Value::Null => Self::Null,
            serde_json::Value::Object(_) => Self::Object,
            serde_json::Value::Array(_) => Self::Array,
        }
    }
}

impl fmt::Display for JsonType {
    fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::String => write!(formatter, "String"),
            Self::Number => write!(formatter, "Number"),
            Self::Boolean => write!(formatter, "Boolean"),
            Self::Null => write!(formatter, "Null"),
            Self::Object => write!(formatter, "Object"),
            Self::Array => write!(formatter, "Array"),
        }
    }
}

#[derive(Clone)]
pub struct CommandMessage {
    pub json: Vec<u8>,
    pub key: Vec<u8>,
    pub value: Vec<u8>,
    pub value_type: JsonType,
    pub error: Option<String>, // only applicable to responses
}

impl CommandMessage {
    pub fn parse(json: &[u8]) -> Option<Self> {
        let map: std::collections::HashMap<String, &RawValue> = serde_json::from_slice(json).ok()?;

        if map.len() != 1 {
            return None;
        }

        let (key, raw_value) = map.iter().next()?;

        let value = serde_json::from_str::<serde_json::Value>(raw_value.get()).ok()?;

        Some(Self {
            json: json.to_vec(),
            key: key.as_bytes().to_vec(),
            value: raw_value.get().as_bytes().to_vec(),
            value_type: (&value).into(),
            error: value.as_object().and_then(|o| o.get("error")).and_then(|v| v.as_str()).map(String::from),
        })
    }

    pub fn bytes_to_json(bytes: &[u8]) -> String {
        let string: String = bytes
            .iter()
            .map(|&byte| match byte {
                0x08 => "\\b".to_string(),
                0x0C => "\\f".to_string(),
                b'\n' => "\\n".to_string(),
                b'\r' => "\\r".to_string(),
                b'\t' => "\\t".to_string(),
                b'"' => "\\\"".to_string(),
                b'\\' => "\\\\".to_string(),
                0x00..=0x1F | 0x7F..=0xFF => format!("\\u{:04X}", byte),
                _ => (byte as char).to_string(),
            })
            .collect();

        format!("\"{}\"", string)
    }

    // TODO
    // pub fn bytes_to_json_string_2(bytes: &[u8]) -> Vec<u8> {
    //     let mut json = vec![b'"'];
    //
    //     for &byte in bytes {
    //         match byte {
    //             0x08 => json.extend_from_slice(b"\\b"),
    //             0x09 => json.extend_from_slice(b"\\t"),
    //             0x0A => json.extend_from_slice(b"\\n"),
    //             0x0C => json.extend_from_slice(b"\\f"),
    //             0x0D => json.extend_from_slice(b"\\r"),
    //             0x00..=0x1F | 0x7F..=0xFF => json.extend_from_slice(format!("\\u{:04X}", byte).as_bytes()),
    //             b'"' => json.extend_from_slice(b"\\\""),
    //             b'\\' => json.extend_from_slice(b"\\\\"),
    //             _ => json.push(byte),
    //         }
    //     }
    //
    //     json.push(b'"');
    //     json
    // }
}
