#include "Connection.h"

class FileConnection : public Connection
{
public:
    FileConnection()
    {
        run(ximu3::FileConnectionInfo("C:/file.ximu3"));
    }
};
