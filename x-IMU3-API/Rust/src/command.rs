use serde_json;

#[derive(Clone)]
pub struct Command {
    pub(crate) terminated_json: Vec<u8>,
    pub key: Vec<u8>,
}

impl Command {
    pub(crate) fn parse(json: &[u8]) -> Option<Self> {
        let value = serde_json::from_slice::<serde_json::Value>(json).ok()?;

        let object = value.as_object()?;

        if object.len() != 1 {
            return None;
        }

        let json = serde_json::to_vec(object).unwrap();

        Some(Self {
            terminated_json: [json.as_slice(), &[b'\n']].concat(),
            key: object.keys().nth(0).unwrap().clone().into_bytes(),
        })
    }
}
