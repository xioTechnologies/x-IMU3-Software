#include "connection.h"
#include "Ximu3.h"

void file_connection()
{
    const XIMU3_FileConnectionInfo connection_info = (XIMU3_FileConnectionInfo) {
        .file_path = "C:/x-IMU3 Example File.ximu3",
    }; // replace with actual connection info

    XIMU3_Connection* const connection = XIMU3_connection_new_file(connection_info);

    run(connection, XIMU3_file_connection_info_to_string(connection_info));
}
