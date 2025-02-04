#pragma once

#include "../../../x-IMU3-API/Cpp/Ximu3.hpp"
#include "../Helpers.hpp"
#include <chrono>
#include <functional>
#include <inttypes.h> // PRIu64
#include <iostream>
#include <thread>

class FileConverter
{
public:
    FileConverter()
    {
        const auto destination = "C:/";
        const auto name = "File Conversion Example";
        const std::vector<std::string> file_paths = { "C:/file.ximu3" };

        if (helpers::yesOrNo("Use async implementation?"))
        {
            ximu3::FileConverter fileConverter(destination, name, file_paths, callback);
            std::this_thread::sleep_for(std::chrono::seconds(60));
        }
        else
        {
            printProgress(ximu3::FileConverter::convert(destination, name, file_paths));
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
               progress.bytes_total);
        // std::cout << XIMU3_file_converter_progress_to_string(progress) << std::endl; // alternative to above
    }
};
