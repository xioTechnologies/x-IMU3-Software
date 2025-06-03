namespace Ximu3Examples
{
    class FileConverter
    {
        public FileConverter()
        {
            string destination = "C:/";
            string name = "File Conversion Example";
            string[] filePaths = ["C:/file.ximu3"]; // replace with actual file path

            if (Helpers.YesOrNo("Use async implementation?"))
            {
                using Ximu3.FileConverter fileConverter = new(destination, name, filePaths, Callback);

                System.Threading.Thread.Sleep(60000);
            }
            else
            {
                PrintProgress(Ximu3.FileConverter.Convert(destination, name, filePaths));
            }
        }

        private void Callback(Ximu3.CApi.XIMU3_FileConverterProgress progress)
        {
            PrintProgress(progress);
        }

        private static void PrintProgress(Ximu3.CApi.XIMU3_FileConverterProgress progress)
        {
            Console.WriteLine(Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_file_converter_status_to_string(progress.status)) + ", " +
                  progress.percentage.ToString("0.0") + "%, " +
                  progress.bytes_processed + " of " +
                  progress.bytes_total + " bytes");
            // Console.WriteLine((Ximu3.Helpers.ToString(CApi.XIMU3_file_converter_progress_to_string(progress))); // alternative to above
        }
    }
}
