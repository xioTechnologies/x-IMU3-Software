#pragma once

#include "Connection.h"

class FileConnection : public Connection
{
public:
    FileConnection()
    {
        const ximu3::FileConnectionInfo connectionInfo("C:/file.ximu3"); // replace with actual connection info

        run(connectionInfo);
    }
};
