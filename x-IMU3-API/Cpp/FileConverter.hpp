#pragma once

#include "../C/Ximu3.h"
#include "Helpers.hpp"
#include <string>

namespace ximu3
{
    class FileConverter
    {
    public:
        FileConverter(const std::string& destination, const std::string& source, std::function<void(XIMU3_FileConverterProgress)> callback)
        {
            internalCallback = std::move(callback);
            fileConverter = XIMU3_file_converter_new(destination.c_str(), source.c_str(), Helpers::wrapCallable<XIMU3_FileConverterProgress>(internalCallback), &internalCallback);
        }

        ~FileConverter()
        {
            XIMU3_file_converter_free(fileConverter);
        }

        static XIMU3_FileConverterProgress convert(const std::string& destination, const std::string& source)
        {
            return XIMU3_file_converter_convert(destination.c_str(), source.c_str());
        }

    private:
        XIMU3_FileConverter* fileConverter;
        std::function<void(XIMU3_FileConverterProgress)> internalCallback;
    };
} // namespace ximu3
