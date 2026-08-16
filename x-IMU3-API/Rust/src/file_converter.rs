use crate::connection::*;
use crate::connection_config::*;
use crate::connection_status::*;
use crate::data_logger::*;
use std::fmt;
use std::ops::Drop;
use std::sync::{Arc, Mutex};

#[repr(C)]
#[derive(Clone, PartialEq)]
pub enum FileConverterStatus {
    InProgress,
    Complete,
}

impl fmt::Display for FileConverterStatus {
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        match *self {
            Self::InProgress => write!(formatter, "In progress"),
            Self::Complete => write!(formatter, "Complete"),
        }
    }
}

#[repr(C)]
#[derive(Clone)]
pub struct FileConverterProgress {
    pub status: FileConverterStatus,
    pub percentage: f32,
    pub bytes_processed: u64,
    pub bytes_total: u64,
}

impl fmt::Display for FileConverterProgress {
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        write!(formatter, "{}, {:.1}%, {} of {} bytes", self.status, self.percentage, self.bytes_processed, self.bytes_total)
    }
}

pub struct FileConverter {
    dropped: Arc<Mutex<bool>>,
}

impl FileConverter {
    pub fn new(destination: &str, name: &str, file_paths: Vec<&str>, closure: Box<dyn Fn(FileConverterProgress) + Send>) -> std::io::Result<Self> {
        let file_converter = Self {
            dropped: Arc::new(Mutex::new(false)),
        };

        let mut progress = FileConverterProgress {
            status: FileConverterStatus::InProgress,
            percentage: 0.0,
            bytes_processed: 0,
            bytes_total: file_paths.iter().map(|file_path| std::fs::metadata(file_path).map(|metadata| metadata.len())).sum::<std::io::Result<u64>>()?,
        };

        let connections: Vec<Connection> = file_paths
            .iter()
            .map(|&file_path| {
                Connection::new(&ConnectionConfig::FileConnectionConfig(FileConnectionConfig {
                    file_path: file_path.to_owned(),
                }))
            })
            .collect();

        let data_logger = DataLogger::new(&destination, &name, connections.iter().collect())?;

        connections.iter().try_for_each(|connection| connection.open())?;

        let dropped = file_converter.dropped.clone();

        std::thread::spawn(move || {
            loop {
                progress.bytes_processed = connections.iter().map(|connection| connection.get_statistics().data_total).sum();
                progress.percentage = 100.0 * ((progress.bytes_processed as f64) / (progress.bytes_total as f64)) as f32;

                if connections.iter().all(|connection| connection.get_status() == ConnectionStatus::Disconnected) {
                    break;
                }

                if let Ok(dropped) = dropped.lock() {
                    if *dropped {
                        return;
                    }
                    closure(progress.clone());
                }

                std::thread::sleep(std::time::Duration::from_millis(100));
            }

            drop(data_logger);

            progress.status = FileConverterStatus::Complete;

            if let Ok(dropped) = dropped.lock() {
                if *dropped {
                    return;
                }
                closure(progress.clone());
            }
        });

        Ok(file_converter)
    }

    pub fn convert(destination: &str, name: &str, file_paths: Vec<&str>) -> std::io::Result<()> {
        let (sender, receiver) = crossbeam::channel::unbounded();

        let closure = Box::new(move |progress| {
            sender.send(progress).ok();
        });

        let _file_converter = Self::new(destination, name, file_paths, closure)?;

        loop {
            if let Ok(progress) = receiver.recv() {
                match progress.status {
                    FileConverterStatus::InProgress => continue,
                    FileConverterStatus::Complete => return Ok(()),
                }
            }
        }
    }
}

impl Drop for FileConverter {
    fn drop(&mut self) {
        *self.dropped.lock().unwrap() = true;
    }
}
