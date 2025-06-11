namespace Ximu3Examples
{
    class FileConnection : Connection
    {
        public FileConnection()
        {
            Ximu3.FileConnectionInfo connectionInfo = new("C:/file.ximu3"); // replace with actual connection info 

            Run(connectionInfo);
        }
    }
}
