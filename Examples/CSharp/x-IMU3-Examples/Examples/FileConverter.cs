using System;

namespace Ximu3Examples
{
    class FileConverter
    {
        public FileConverter()
        {
            string destination = "C:/";
            string source = "C:/file.ximu3";

            if (Helpers.YesOrNo("Use async implementation?") == true)
            {
                Ximu3.FileConverter file_converter = new Ximu3.FileConverter(destination, source, FileConverterEvent);
                Helpers.Wait(-1);
                file_converter.Dispose();
            }
            else
            {
                PrintProgress(Ximu3.FileConverter.Convert(destination, source));
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
                  progress.FileSize + " bytes");
            // Console.WriteLine(progress); // alternative to above
        }
    }
}
