namespace Ximu3Examples
{
    class FileConverter
    {
        public FileConverter()
        {
            // Blocking
            var destination = "C:/";
            var nameBlocking = "x-IMU3 File Conversion Example Blocking";
            string[] filePaths = ["C:/x-IMU3 Example File.ximu3"]; // replace with actual file path

            var progress = Ximu3.FileConverter.Convert(destination, nameBlocking, filePaths);

            PrintProgress(progress);

            // Non-blocking
            var nameNonBlocking = "x-IMU3 File Conversion Example Non-Blocking";

            using var fileConverter = new Ximu3.FileConverter(destination, nameNonBlocking, filePaths, Callback);

            System.Threading.Thread.Sleep(60000);
        }

        private void Callback(Ximu3.CApi.XIMU3_FileConverterProgress progress)
        {
            PrintProgress(progress);
        }

        private static void PrintProgress(Ximu3.CApi.XIMU3_FileConverterProgress progress)
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
