use super::connection;
use ximu3::connection_info::*;

pub fn run() {
    let connection_info = &ConnectionInfo::FileConnectionInfo(FileConnectionInfo {
        file_path: "C:/x-IMU3 Example File.ximu3".to_owned(),
    }); // replace with actual connection info

    connection::run(connection_info);
}
