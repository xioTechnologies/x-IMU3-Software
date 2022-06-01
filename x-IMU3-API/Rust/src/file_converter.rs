use std::fmt;
use std::ops::Drop;
use std::path::Path;
use std::sync::{Arc, Mutex};
use crate::connection::*;
use crate::connection_info::*;
use crate::data_logger::*;

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
    pub file_size: u64,
}

impl fmt::Display for FileConverterProgress {
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        write!(formatter, "{}, {:.1}%, {} of {} bytes",
               self.status,
               self.percentage,
               self.bytes_processed,
               self.file_size)
    }
}

pub struct FileConverter {
    dropped: Arc<Mutex<bool>>,
}

impl FileConverter {
    pub fn new(destination: &str, source: &str, closure: Box<dyn Fn(FileConverterProgress) + Send>) -> FileConverter {
        let file_converter = FileConverter {
            dropped: Arc::new(Mutex::new(false)),
        };

        let mut progress = FileConverterProgress {
            status: FileConverterStatus::Failed,
            percentage: 0.0,
            bytes_processed: 0,
            file_size: 0,
        };

        match std::fs::metadata(source) {
            Ok(metadata) => progress.file_size = metadata.len(),
            Err(_) => {
                closure(progress);
                return file_converter;
            }
        }

        let name = Path::new(source).file_stem();

        if name.is_none() {
            closure(progress);
            return file_converter;
        }

        let directory = destination.to_owned();
        let name = name.unwrap().to_str().unwrap().to_owned();
        let mut connection = Connection::new(ConnectionInfo::FileConnectionInfo(FileConnectionInfo { file_path: source.to_owned() }));

        let dropped = file_converter.dropped.clone();

        std::thread::spawn(move || {
            if let Err(_) = connection.open() {
                if let Ok(dropped) = dropped.lock() {
                    if *dropped == false {
                        closure(progress.clone());
                    }
                }
                return;
            }

            {
                let (sender, receiver) = crossbeam::channel::unbounded();

                let mut data_logger = DataLogger::new(&directory, &name, vec!(&mut connection), Box::new(move |result| {
                    if result.is_err() {
                        sender.send(result).ok();
                    }
                }));

                if receiver.try_recv().is_ok() {
                    if let Ok(dropped) = dropped.lock() {
                        if *dropped == false {
                            closure(progress.clone());
                        }
                    }
                    return;
                }

                progress.status = FileConverterStatus::InProgress;

                loop {
                    progress.bytes_processed = data_logger.connections[0].get_statistics().data_total;
                    progress.percentage = 100.0 * ((progress.bytes_processed as f64) / (progress.file_size as f64)) as f32;

                    if let Ok(dropped) = dropped.lock() {
                        if *dropped {
                            return;
                        }
                        closure(progress.clone());
                    }

                    if progress.bytes_processed == progress.file_size {
                        break;
                    }

                    std::thread::sleep(std::time::Duration::from_millis(100));
                }
            } // drop data_logger

            connection.close();

            progress.status = FileConverterStatus::Complete;
            if let Ok(dropped) = dropped.lock() {
                if *dropped == false {
                    closure(progress.clone());
                }
            }
        });

        file_converter
    }

    pub fn convert(destination: &str, source: &str) -> FileConverterProgress {
        let (sender, receiver) = crossbeam::channel::unbounded();

        let _file_converter = FileConverter::new(destination, source, Box::new(move |progress| {
            sender.send(progress).ok();
        }));

        loop {
            let progress = receiver.recv().unwrap();

            if progress.status != FileConverterStatus::InProgress {
                return progress;
            }
        }
    }
}

impl Drop for FileConverter {
    fn drop(&mut self) {
        *self.dropped.lock().unwrap() = true;
    }
}
