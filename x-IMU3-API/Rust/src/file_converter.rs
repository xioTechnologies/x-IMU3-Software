use crate::connection::*;
use crate::connection_info::*;
use crate::data_logger::*;
use std::fmt;
use std::ops::Drop;
use std::sync::atomic::{AtomicUsize, Ordering};
use std::sync::{Arc, Mutex};

#[repr(C)]
#[derive(Clone, PartialEq)]
pub enum FileConverterStatus {
    Complete,
    Failed,
    InProgress,
}

impl fmt::Display for FileConverterStatus {
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        match *self {
            FileConverterStatus::Complete => write!(formatter, "Complete"),
            FileConverterStatus::Failed => write!(formatter, "Failed"),
            FileConverterStatus::InProgress => write!(formatter, "In progress"),
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
        write!(formatter, "{}, {:.1}%, {} of {} bytes", self.status, self.percentage, self.bytes_processed, self.bytes_total,)
    }
}

pub struct FileConverter {
    dropped: Arc<Mutex<bool>>,
}

impl FileConverter {
    pub fn new(destination: &str, name: &str, file_paths: Vec<&str>, closure: Box<dyn Fn(FileConverterProgress) + Send>) -> FileConverter {
        let file_converter = FileConverter {
            dropped: Arc::new(Mutex::new(false)),
        };

        let mut progress = FileConverterProgress {
            status: FileConverterStatus::Failed,
            percentage: 0.0,
            bytes_processed: 0,
            bytes_total: 0,
        };

        for file_path in &file_paths {
            match std::fs::metadata(file_path) {
                Ok(metadata) => progress.bytes_total += metadata.len(),
                Err(_) => {
                    closure(progress);
                    return file_converter;
                }
            }
        }

        let dropped = file_converter.dropped.clone();
        let destination = destination.to_owned();
        let name = name.to_owned();

        let connections: Vec<Connection> = file_paths
            .iter()
            .map(|&file_path| {
                Connection::new(&ConnectionInfo::FileConnectionInfo(FileConnectionInfo {
                    file_path: file_path.to_owned(),
                }))
            })
            .collect();

        std::thread::spawn(move || {
            let data_logger = DataLogger::new(&destination, &name, connections.iter().collect());

            if data_logger.is_err() {
                if let Ok(dropped) = dropped.lock() {
                    if *dropped == false {
                        closure(progress.clone());
                    }
                }
                return;
            }

            let end_of_file_counter = Arc::new(AtomicUsize::new(0));

            for connection in connections.iter() {
                let end_of_file_counter = end_of_file_counter.clone();

                connection.add_end_of_file_closure(Box::new(move || {
                    end_of_file_counter.fetch_add(1, Ordering::SeqCst);
                }));

                if connection.open().is_err() {
                    if let Ok(dropped) = dropped.lock() {
                        if *dropped == false {
                            closure(progress.clone());
                        }
                    }
                    return;
                }
            }

            progress.status = FileConverterStatus::InProgress;

            loop {
                progress.bytes_processed = connections.iter().map(|connection| connection.get_statistics().data_total).sum();
                progress.percentage = 100.0 * ((progress.bytes_processed as f64) / (progress.bytes_total as f64)) as f32;

                if end_of_file_counter.load(Ordering::SeqCst) == connections.len() {
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

            drop(data_logger.unwrap());

            progress.status = FileConverterStatus::Complete;
            if let Ok(dropped) = dropped.lock() {
                if *dropped == false {
                    closure(progress.clone());
                }
            }
        });

        file_converter
    }

    pub fn convert(destination: &str, name: &str, files: Vec<&str>) -> FileConverterProgress {
        let (sender, receiver) = crossbeam::channel::unbounded();

        let closure = Box::new(move |progress| {
            sender.send(progress).ok();
        });

        let _file_converter = FileConverter::new(destination, name, files, closure);

        loop {
            if let Ok(progress) = receiver.recv() {
                if progress.status != FileConverterStatus::InProgress {
                    return progress;
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
