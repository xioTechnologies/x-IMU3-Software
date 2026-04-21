use serde_json;
use serde_json::value::RawValue;

#[derive(Clone)]
pub struct CommandMessage {
    pub json: Vec<u8>,
    pub key: Vec<u8>,
    pub value: Vec<u8>,
    pub error: Option<String>, // only applicable to responses
}

impl CommandMessage {
    pub(crate) fn parse(json: &[u8]) -> Option<Self> {
        let map: std::collections::HashMap<String, &RawValue> = serde_json::from_slice(json).ok()?;

        if map.len() != 1 {
            return None;
        }

        let (key, value) = map.iter().next()?;

        Some(Self {
            json: json.to_vec(),
            key: key.as_bytes().to_vec(),
            value: value.get().as_bytes().to_vec(),
            error: serde_json::from_str::<serde_json::Value>(value.get()).ok().and_then(|v| v.as_object().and_then(|o| o.get("error")).and_then(|v| v.as_str()).map(String::from)),
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
