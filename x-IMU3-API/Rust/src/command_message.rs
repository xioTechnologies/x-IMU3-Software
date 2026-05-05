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
}
