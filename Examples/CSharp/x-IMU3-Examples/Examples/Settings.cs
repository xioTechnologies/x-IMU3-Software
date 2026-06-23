namespace Ximu3Examples
{
    class Settings
    {
        public Settings()
        {
            // Search for connection
            var devices = Ximu3.PortScanner.ScanFilter(Ximu3.CApi.XIMU3_PortType.XIMU3_PortTypeUsb);

            System.Threading.Thread.Sleep(1000); // wait for OS to release port

            if (devices.Length == 0)
            {
                Console.WriteLine("No USB connections available");
                return;
            }

            Console.WriteLine("Found " + devices[0].ToString());

            // Open connection
            var connection = new Ximu3.Connection(Ximu3.ConnectionConfig.From(devices[0])!);

            var result = connection.Open();

            if (result != Ximu3.CApi.XIMU3_Result.XIMU3_ResultOk)
            {
                Console.WriteLine("Unable to open " + connection.GetConfig() + ". " + Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_result_to_string(result)) + ".");
                return;
            }

            // Backup settings
            var filePath = "C:/Users/Public/x-IMU3 Example Settings.json";

            var resultBackup = Ximu3.Settings.Backup(filePath, connection);

            if (resultBackup != Ximu3.CApi.XIMU3_Result.XIMU3_ResultOk)
            {
                Console.WriteLine("Backup failed. " + Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_result_to_string(resultBackup)) + ".");
            }

            // Restore settings
            var resultRestore = Ximu3.Settings.Restore(filePath, connection);

            if (resultRestore != Ximu3.CApi.XIMU3_Result.XIMU3_ResultOk)
            {
                Console.WriteLine("Restore failed. " + Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_result_to_string(resultRestore)) + ".");
            }

            // Save command
            connection.SendCommand("{\"save\":null}");

            // Close connection
            connection.Close();
        }
    }
}
