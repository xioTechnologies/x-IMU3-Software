#pragma once

#include "Connection.h"
#include "Ximu3.hpp"

class FileConnection : public Connection {
public:
    FileConnection() {
        const ximu3::FileConnectionConfig config("C:/x-IMU3 Example File.ximu3"); // replace with actual connection config

        run(config);
    }
};
