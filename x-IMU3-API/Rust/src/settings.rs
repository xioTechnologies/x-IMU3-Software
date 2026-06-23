use crate::connection::*;
use std::fs::File;
use std::io::Write;
use std::path::Path;

pub fn backup(file_path: &str, connection: &Connection) -> std::io::Result<()> {
    let file_path = Path::new(file_path).with_extension("json");

    let mut file = File::create_new(file_path)?;

    file.write_all(b"[\n")?;

    for index in 0.. {
        let command = format!("{{\"enumerate {index}\":null}}").into_bytes();

        let response = connection.send_command(command, DEFAULT_RETRIES, DEFAULT_TIMEOUT);

        let response = match response {
            Some(response) => response,
            None => return Err(std::io::ErrorKind::TimedOut.into()),
        };

        if response.error.is_some() {
            return Err(std::io::ErrorKind::Other.into());
        }

        if response.value == b"null" {
            break;
        }

        if index > 0 {
            file.write_all(b",\n")?;
        }

        file.write_all(b"    ")?;
        file.write_all(&response.value)?;
    }

    file.write_all(b"\n]\n")?;

    Ok(())
}

pub fn restore(file_path: &str, connection: &Connection) -> std::io::Result<()> {
    let file_path = Path::new(file_path).with_extension("json");

    let bytes = std::fs::read(&file_path)?;

    let array = serde_json::from_slice::<Vec<serde_json::Value>>(&bytes).map_err(|_| std::io::ErrorKind::InvalidData)?;

    for element in array {
        let command = element.to_string().into_bytes();

        connection.send_command(command, DEFAULT_RETRIES, DEFAULT_TIMEOUT).ok_or(std::io::ErrorKind::TimedOut)?;
    }

    Ok(())
}
