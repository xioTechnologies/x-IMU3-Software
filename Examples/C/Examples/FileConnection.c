#include "Connection.h"
#include "Ximu3.h"

void FileConnection()
{
    const XIMU3_FileConnectionInfo connectionInfo = (XIMU3_FileConnectionInfo) {
        .file_path = "C:/file.ximu3",
    }; // replace with actual connection info

    XIMU3_Connection* const connection = XIMU3_connection_new_file(connectionInfo);

    Run(connection, XIMU3_file_connection_info_to_string(connectionInfo));
}
