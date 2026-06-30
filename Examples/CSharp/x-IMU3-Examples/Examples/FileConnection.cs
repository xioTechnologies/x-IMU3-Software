namespace Ximu3Examples
{
    class FileConnection : Connection
    {
        public FileConnection()
        {
            var config = new Ximu3.FileConnectionConfig("C:/Users/Public/x-IMU3 Example File.ximu3"); // replace with actual connection config

            Run(config);
        }
    }
}
