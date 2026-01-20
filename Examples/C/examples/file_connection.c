#include "connection.h"
#include "Ximu3.h"

void file_connection() {
    const XIMU3_FileConnectionConfig config = (XIMU3_FileConnectionConfig){
        .file_path = "C:/x-IMU3 Example File.ximu3",
    }; // replace with actual connection config

    XIMU3_Connection *const connection = XIMU3_connection_new_file(config);

    run(connection);
}
