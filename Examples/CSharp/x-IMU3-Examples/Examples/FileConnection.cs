using System;

namespace Ximu3Examples
{
    class FileConnection : Connection
    {
        public FileConnection()
        {
            Run(new Ximu3.FileConnectionInfo("C:/file.ximu3"));
        }
    }
}
