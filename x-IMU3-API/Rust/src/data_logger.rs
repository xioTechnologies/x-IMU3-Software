use crate::connection::*;
use crate::ping_response::*;
use serde_json;
use std::collections::HashMap;
use std::fs::File;
use std::io::{Seek, SeekFrom, Write};
use std::ops::Drop;
use std::path::Path;
use std::sync::{Arc, Mutex};

pub struct DataLogger<'a> {
    connections: Vec<&'a Connection>,
    closure_ids: Vec<Vec<u64>>,
    in_progress: Arc<Mutex<bool>>,
}

impl<'a> DataLogger<'a> {
    pub fn new(destination: &str, name: &str, connections: Vec<&'a Connection>) -> std::io::Result<Self> {
        // Create root directory
        Path::new(destination).read_dir()?;

        let root = Path::new(destination).join(name);

        std::fs::create_dir(&root)?;

        // Initialise structure
        let mut data_logger = Self {
            connections,
            closure_ids: Vec::new(),
            in_progress: Arc::new(Mutex::new(false)),
        };

        // Create connection directories
        let mut paths = Vec::new();

        for (index, _) in data_logger.connections.iter().enumerate() {
            let path = Path::new(&root).join(format!("Connection {}", index));
            std::fs::create_dir_all(&path)?;
            paths.push(path);
        }

        // Add closures
        let (sender, receiver) = crossbeam::channel::unbounded();
        const COMMAND_FILE_NAME: &str = "Command.json";

        for (index, connection) in data_logger.connections.iter().enumerate() {
            data_logger.closure_ids.push(Vec::new());

            let sender_clone = sender.clone();
            let path_clone = paths[index].clone();

            data_logger.closure_ids[index].push(connection.add_receive_error_closure(Box::new(move |receive_error| {
                sender_clone.send((path_clone.join("ReceiveError.txt"), "".to_owned(), receive_error.to_string() + "\n")).ok();
            })));

            let sender_clone = sender.clone();
            let path_clone = paths[index].clone();

            data_logger.closure_ids[index].push(connection.add_command_closure(Box::new(move |command| {
                sender_clone.send((path_clone.join(COMMAND_FILE_NAME), "[\n".to_owned(), format!("    {}\n]", command.json))).ok();
            })));

            let sender_clone = sender.clone();
            let path_clone = paths[index].clone();

            data_logger.closure_ids[index].push(connection.add_data_closure(Box::new(move |message| {
                sender_clone.send((path_clone.join(message.get_csv_file_name()), message.get_csv_headings().to_owned(), message.to_csv_row())).ok();
            })));
        }

        // Spawn thread
        *data_logger.in_progress.lock().unwrap() = true;
        let in_progress = data_logger.in_progress.clone();

        std::thread::spawn(move || {
            let mut files: HashMap<std::path::PathBuf, File> = HashMap::new();

            loop {
                match receiver.recv() {
                    Ok((path, preamble, line)) => {
                        if let Some(mut file) = files.get(&path) {
                            if path.file_name().map(|name| name == COMMAND_FILE_NAME).unwrap_or(false) {
                                file.seek(SeekFrom::End(-2)).ok(); // remove trailing "\n]"
                                file.write_all(",\n".as_bytes()).ok();
                            }
                            file.write_all(line.as_bytes()).ok();
                        } else {
                            if let Ok(mut file) = File::create(&path) {
                                file.write_all(preamble.as_bytes()).ok();
                                file.write_all(line.as_bytes()).ok();
                                files.insert(path, file);
                            }
                        }
                    }
                    Err(_) => break,
                }
            }

            drop(files);

            // Rename connection directories
            for path in &paths {
                if let Ok(json) = std::fs::read_to_string(path.join(COMMAND_FILE_NAME)) {
                    if let Ok(array) = serde_json::from_str::<Vec<serde_json::Value>>(&json) {
                        for element in array {
                            if let Ok(ping_response) = PingResponse::parse(&element.to_string()) {
                                let new_path = Path::new(&root).join(format!("{} {} ({})", ping_response.device_name, ping_response.serial_number, ping_response.interface));
                                std::fs::rename(path, new_path).ok();
                                break;
                            }
                        }
                    }
                }
            }

            *in_progress.lock().unwrap() = false;
        });

        // Send commands
        for connection in data_logger.connections.iter() {
            connection.send_commands_async(vec!["{\"ping\":null}", "{\"time\":null}"], 4, 200, Box::new(|_| {}));
        }

        Ok(data_logger)
    }

    pub fn log(destination: &str, name: &str, connections: Vec<&'a Connection>, seconds: u32) -> std::io::Result<()> {
        let data_logger = Self::new(destination, name, connections)?;

        std::thread::sleep(std::time::Duration::from_secs(seconds as u64));

        drop(data_logger);

        Ok(())
    }
}

impl Drop for DataLogger<'_> {
    fn drop(&mut self) {
        if self.closure_ids.len() > 0 {
            for (index, connection) in self.connections.iter().enumerate() {
                for closure_id in self.closure_ids[index].iter() {
                    connection.remove_closure(*closure_id);
                }
            }
        }

        while *self.in_progress.lock().unwrap() {
            std::thread::sleep(std::time::Duration::from_millis(1));
        }
    }
}
