using System;

namespace Ximu3Examples
{
    class FileConverter
    {
        public FileConverter()
        {
            string destination = "C:/";
            string name = "File Conversion Example";
            string[] file_paths = { "C:/file.ximu3" };

            if (Helpers.YesOrNo("Use async implementation?"))
            {
                using Ximu3.FileConverter file_converter = new Ximu3.FileConverter(destination, name, file_paths, FileConverterEvent);
                System.Threading.Thread.Sleep(60000);
            }
            else
            {
                PrintProgress(Ximu3.FileConverter.Convert(destination, name, file_paths));
            }
        }

        private void FileConverterEvent(Object sender, Ximu3.FileConverterEventArgs args)
        {
            PrintProgress(args.progress);
        }

        private void PrintProgress(Ximu3.FileConverterProgress progress)
        {
            Console.WriteLine(progress.Status + ", " +
                  progress.Percentage.ToString("0.0") + "%, " +
                  progress.BytesProcessed + " of " +
                  progress.BytesTotal + " bytes");
            // Console.WriteLine(progress); // alternative to above
        }
    }
}
