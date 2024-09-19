#include "../../../x-IMU3-API/C/Ximu3.h"
#include "Connection.h"

void FileConnection()
{
    const XIMU3_FileConnectionInfo connectionInfo = (XIMU3_FileConnectionInfo) {
        .file_path = "C:/file.ximu3",
    };
    Run(XIMU3_connection_new_file(connectionInfo), XIMU3_file_connection_info_to_string(connectionInfo));
}
