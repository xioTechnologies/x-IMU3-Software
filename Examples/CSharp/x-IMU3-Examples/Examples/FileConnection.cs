namespace Ximu3Examples
{
    class FileConnection : Connection
    {
        public FileConnection()
        {
            Ximu3.FileConnectionConfig config = new("C:/x-IMU3 Example File.ximu3"); // replace with actual connection config

            Run(config);
        }
    }
}
