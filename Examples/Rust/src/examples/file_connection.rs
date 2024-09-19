use ximu3::connection_info::*;
use super::connection;

pub fn run() {
    let connection_info = ConnectionInfo::FileConnectionInfo(FileConnectionInfo {
        file_path: "C:/file.ximu3".to_owned(),
    });

    connection::run(connection_info);
}
