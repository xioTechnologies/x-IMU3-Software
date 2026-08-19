namespace Ximu3Examples
{
    class FileConverter
    {
        public FileConverter()
        {
            // Blocking
            var destination = "C:/Users/Public/";
            var nameBlocking = "x-IMU3 File Converter Example Blocking";
            string[] filePaths = ["C:/Users/Public/x-IMU3 Example File.ximu3"]; // replace with actual file path

            var resultBlocking = Ximu3.FileConverter.Convert(destination, nameBlocking, filePaths);

            if (resultBlocking != Ximu3.CApi.XIMU3_Result.XIMU3_ResultOk)
            {
                Console.WriteLine("File converter failed: " + Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_result_to_string(resultBlocking)));
            }

            // Non-blocking
            var nameNonBlocking = "x-IMU3 File Converter Example Non-Blocking";

            using var fileConverter = new Ximu3.FileConverter(destination, nameNonBlocking, filePaths, Callback);

            var resultNonBlocking = fileConverter.GetResult();

            if (resultNonBlocking != Ximu3.CApi.XIMU3_Result.XIMU3_ResultOk)
            {
                Console.WriteLine("File converter failed: " + Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_result_to_string(resultNonBlocking)));
            }

            System.Threading.Thread.Sleep(60000);
        }

        private static void Callback(Ximu3.CApi.XIMU3_FileConverterProgress progress)
        {
            Console.WriteLine(
                Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_file_converter_status_to_string(progress.status)) + ", " +
                progress.percentage.ToString("0.0") + "%, " +
                progress.bytes_processed + " of " +
                progress.bytes_total + " bytes"
            );
            // Console.WriteLine(Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_file_converter_progress_to_string(progress))); // alternative to above
        }
    }
}
