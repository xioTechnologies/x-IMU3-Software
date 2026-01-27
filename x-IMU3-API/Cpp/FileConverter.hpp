#pragma once

#include "../C/Ximu3.h"
#include "Helpers.hpp"
#include <string>

namespace ximu3 {
    class FileConverter {
    public:
        FileConverter(const std::string &destination, const std::string &name, const std::vector<std::string> &files, std::function<void(XIMU3_FileConverterProgress)> callback_) {
            const auto charPtrVector = Helpers::toCharPtrVector(files);
            callback = std::move(callback_);
            wrapped = XIMU3_file_converter_new(destination.c_str(), name.c_str(), charPtrVector.data(), (uint32_t) charPtrVector.size(), Helpers::wrapCallable<XIMU3_FileConverterProgress>(callback), &callback);
        }

        ~FileConverter() {
            XIMU3_file_converter_free(wrapped);
        }

        static XIMU3_FileConverterProgress convert(const std::string &destination, const std::string &name, const std::vector<std::string> &files) {
            const auto charPtrVector = Helpers::toCharPtrVector(files);
            return XIMU3_file_converter_convert(destination.c_str(), name.c_str(), charPtrVector.data(), (uint32_t) charPtrVector.size());
        }

    private:
        XIMU3_FileConverter *wrapped;
        std::function<void(XIMU3_FileConverterProgress)> callback;
    };
} // namespace ximu3
