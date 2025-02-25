namespace Ximu3Examples
{
    class FileConnection : Connection
    {
        public FileConnection()
        {
            Ximu3.CApi.XIMU3_FileConnectionInfo connectionInfo = new()
            {
                file_path = Ximu3.Helpers.ToBytes("C:/file.ximu3")
            };
            Run(connectionInfo);
        }
    }
}
