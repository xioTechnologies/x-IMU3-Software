use super::connection;
use ximu3::connection_config::*;

pub fn run() {
    let config = &ConnectionConfig::FileConnectionConfig(FileConnectionConfig {
        file_path: "C:/x-IMU3 Example File.ximu3".to_owned(),
    }); // replace with actual connection config

    connection::run(config);
}
