#include "../../../x-IMU3-API/Cpp/Ximu3.hpp"
#include "../Helpers.hpp"
#include <functional>
#include <inttypes.h> // PRIu64
#include <iostream>

class FileConverter
{
public:
    FileConverter()
    {
        const auto destination = "C:/"; // TODO: Example should work for macOS and Windows
        const auto source = "C:/file.ximu3";

        if (helpers::yesOrNo("Use async implementation?") == true)
        {
            ximu3::FileConverter fileConverter(destination, source, callback);
            helpers::wait(-1);
        }
        else
        {
            printProgress(ximu3::FileConverter::convert(destination, source));
        }
    }

private:
    std::function<void(const ximu3::XIMU3_FileConverterProgress)> callback = [](const auto& progress)
    {
        printProgress(progress);
    };

    static void printProgress(const ximu3::XIMU3_FileConverterProgress progress)
    {
        printf("%s, %0.1f%%, %" PRIu64 " of %" PRIu64 " bytes\n",
               XIMU3_file_converter_status_to_string(progress.status),
               progress.percentage,
               progress.bytes_processed,
               progress.file_size);
        // std::cout << XIMU3_file_converter_progress_to_string(progress) << std::endl; // alternative to above
    }
};
