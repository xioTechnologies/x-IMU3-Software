namespace Ximu3Examples
{
    class FileConnection : Connection
    {
        public FileConnection()
        {
            Ximu3.FileConnectionInfo connectionInfo = new("C:/x-IMU3 Example File.ximu3"); // replace with actual connection info

            Run(connectionInfo);
        }
    }
}
