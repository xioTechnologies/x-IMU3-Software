use crate::connection::*;
use std::collections::HashMap;
use std::fs::File;
use std::io::{Seek, SeekFrom, Write};
use std::ops::Drop;
use std::path::{Path, PathBuf};
use std::sync::{Arc, Mutex};

pub struct DataLogger {
    connections: Vec<(InternalConnection, Vec<u64>)>,
    in_progress: Arc<Mutex<bool>>,
}

impl DataLogger {
    pub fn new(destination: &str, name: &str, connections: Vec<&Connection>) -> std::io::Result<Self> {
        let paths = Self::create_directory(destination, name, &connections)?;

        let mut data_logger = Self {
            connections: connections.iter().map(|connection| (connection.internal.clone(), Vec::new())).collect(),
            in_progress: Arc::new(Mutex::new(true)),
        };

        let (sender, receiver) = crossbeam::channel::unbounded();

        const COMMAND_FILE_NAME: &str = "Command.json";

        for (index, (connection, closure_ids)) in data_logger.connections.iter_mut().enumerate() {
            closure_ids.push(connection.lock().unwrap().get_receiver().lock().unwrap().dispatcher.add_receive_error_closure(Box::new({
                let sender = sender.clone();
                let path = paths[index].clone();

                move |error| {
                    sender.send((path.join("ReceiveError.txt"), "".to_string(), error.to_string() + "\n")).ok();
                }
            })));

            closure_ids.push(connection.lock().unwrap().get_receiver().lock().unwrap().dispatcher.add_command_closure(Box::new({
                let sender = sender.clone();
                let path = paths[index].clone();

                move |command| {
                    sender.send((path.join(COMMAND_FILE_NAME), "[\n".to_string(), format!("    {}\n]", String::from_utf8_lossy(&command.json)))).ok();
                }
            })));

            closure_ids.push(connection.lock().unwrap().get_receiver().lock().unwrap().dispatcher.add_data_closure(Box::new({
                let sender = sender.clone();
                let path = paths[index].clone();

                move |message| {
                    sender.send((path.join(message.get_csv_file_name()), message.get_csv_headings().to_string(), message.to_csv_row())).ok();
                }
            })));
        }

        let in_progress = data_logger.in_progress.clone();

        std::thread::spawn(move || {
            let mut files: HashMap<std::path::PathBuf, File> = HashMap::new();

            loop {
                let (path, preamble, line) = match receiver.recv() {
                    Ok(tuple) => tuple,
                    Err(_) => break,
                };

                if let Some(mut file) = files.get(&path) {
                    if path.file_name().map(|name| name == COMMAND_FILE_NAME).unwrap_or(false) {
                        file.seek(SeekFrom::End(-2)).ok(); // remove trailing "\n]"
                        file.write_all(",\n".as_bytes()).ok();
                    }
                    file.write_all(line.as_bytes()).ok();
                    continue;
                }

                if let Ok(mut file) = File::create(&path) {
                    file.write_all(preamble.as_bytes()).ok();
                    file.write_all(line.as_bytes()).ok();
                    files.insert(path, file);
                }
            }

            drop(files);

            *in_progress.lock().unwrap() = false;
        });

        Ok(data_logger)
    }

    fn create_directory(destination: &str, name: &str, connections: &[&Connection]) -> std::io::Result<Vec<PathBuf>> {
        Path::new(destination).read_dir()?;

        let root = Path::new(destination).join(name);

        std::fs::create_dir(&root)?;

        let directories = connections
            .iter()
            .map(|connection| {
                let ping_response = connection.internal.lock().unwrap().get_receiver().lock().unwrap().dispatcher.ping_response.lock().unwrap().clone();
                let config = connection.internal.lock().unwrap().get_config();

                let directory = match ping_response {
                    Some(ping_response) => format!("{} {} ({})", ping_response.device_name, ping_response.serial_number, ping_response.interface),
                    None => config.to_string(),
                };

                directory
                    .chars()
                    .map(|c| match c {
                        '<' | '>' | ':' | '"' | '/' | '\\' | '|' | '?' | '*' => '_',
                        c => c,
                    })
                    .collect::<String>()
            })
            .collect::<Vec<_>>();

        let metadata = serde_json::json!({
            "name": name,
            "time": std::time::SystemTime::now().duration_since(std::time::UNIX_EPOCH).unwrap().as_secs().to_string(), // TODO: use YYYY-MM-DD hh:mm:ss
            "connections": connections.iter().zip(directories.iter()).map(|(connection, directory)| {
                let ping_response = connection.internal.lock().unwrap().get_receiver().lock().unwrap().dispatcher.ping_response.lock().unwrap().clone();
                let config = connection.internal.lock().unwrap().get_config();

                serde_json::json!({
                    "model": "", // TODO: use model from ping_response
                    "serial_number": ping_response.as_ref().map(|ping_response| ping_response.serial_number.clone()).unwrap_or_default(),
                    "device_name": ping_response.as_ref().map(|ping_response| ping_response.device_name.clone()).unwrap_or_default(),
                    "config": config.to_string(),
                    "directory": directory,
                })
            }).collect::<Vec<_>>(),
        });

        std::fs::write(root.join("metadata.json"), serde_json::to_string_pretty(&metadata).unwrap())?;

        let paths = directories.iter().map(|directory| root.join(directory)).collect::<Vec<_>>();

        for path in &paths {
            std::fs::create_dir_all(path)?;
        }

        Ok(paths)
    }

    pub fn log(destination: &str, name: &str, connections: Vec<&Connection>, seconds: u32) -> std::io::Result<()> {
        let data_logger = Self::new(destination, name, connections)?;

        std::thread::sleep(std::time::Duration::from_secs(seconds as u64));

        drop(data_logger);

        Ok(())
    }
}

impl Drop for DataLogger {
    fn drop(&mut self) {
        for (connection, closure_ids) in &self.connections {
            for closure_id in closure_ids {
                connection.lock().unwrap().get_receiver().lock().unwrap().dispatcher.remove_closure(*closure_id);
            }
        }

        while *self.in_progress.lock().unwrap() {
            std::thread::sleep(std::time::Duration::from_millis(1));
        }
    }
}
