#pragma once

#include <chrono>
#include <functional>
#include <inttypes.h>
#include <iostream>
#include <string>
#include <thread>
#include "Ximu3.hpp"

class FileConverter {
public:
    FileConverter() {
        // Blocking
        const auto destination = "C:/Users/Public/";
        const auto nameBlocking = "x-IMU3 File Converter Example Blocking";
        const std::vector<std::string> filePaths = {"C:/Users/Public/x-IMU3 Example File.ximu3"}; // replace with actual file path

        const auto resultBlocking = ximu3::FileConverter::convert(destination, nameBlocking, filePaths);

        if (resultBlocking != ximu3::XIMU3_ResultOk) {
            std::cout << "File converter failed: " << XIMU3_result_to_string(resultBlocking) << std::endl;
        }

        // Non-blocking
        const auto nameNonBlocking = "x-IMU3 File Converter Example Non-Blocking";

        ximu3::FileConverter fileConverter(destination, nameNonBlocking, filePaths, callback);

        const auto resultNonBlocking = fileConverter.getResult();

        if (resultNonBlocking != ximu3::XIMU3_ResultOk) {
            std::cout << "File converter failed: " << XIMU3_result_to_string(resultNonBlocking) << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::seconds(60));
    }

private:
    std::function<void(const ximu3::XIMU3_FileConverterProgress)> callback = [](const auto &progress) {
        printf("%s, %0.1f%%, %" PRIu64 " of %" PRIu64 " bytes\n",
               ximu3::XIMU3_file_converter_status_to_string(progress.status),
               progress.percentage,
               progress.bytes_processed,
               progress.bytes_total);
        // std::cout << ximu3::XIMU3_file_converter_progress_to_string(progress) << std::endl; // alternative to above
    };
};
