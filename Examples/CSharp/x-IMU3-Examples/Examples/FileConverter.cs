namespace Ximu3Examples
{
    class FileConverter
    {
        public FileConverter()
        {
            string destination = "C:/";
            string name = "File Conversion Example";
            string[] file_paths = ["C:/file.ximu3"];

            if (Helpers.YesOrNo("Use async implementation?"))
            {
                using Ximu3.FileConverter file_converter = new(destination, name, file_paths, Callback);
                System.Threading.Thread.Sleep(60000);
            }
            else
            {
                PrintProgress(Ximu3.FileConverter.Convert(destination, name, file_paths));
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
