#pragma once

#include "../../C/Ximu3.h"
#include "FileConverterStatus.h"

using namespace System;

namespace Ximu3
{
    public ref class FileConverterProgress
    {
    internal:
        FileConverterProgress(ximu3::XIMU3_FileConverterProgress progress) : progress{ new ximu3::XIMU3_FileConverterProgress{progress} }
        {
        }

    public:
        ~FileConverterProgress()
        {
            delete progress;
        }

        property FileConverterStatus Status
        {
            FileConverterStatus get()
            {
                return (FileConverterStatus)progress->status;
            }
        }

        property float Percentage
        {
            float get()
            {
                return progress->percentage;
            }
        }

        property int64_t BytesProcessed
        {
            int64_t get()
            {
                return progress->bytes_processed;
            }
        }

        property int64_t FileSize
        {
            int64_t get()
            {
                return progress->file_size;
            }
        }

        String^ ToString() override
        {
            return gcnew String(ximu3::XIMU3_file_converter_progress_to_string(*progress));
        }

    private:
        ximu3::XIMU3_FileConverterProgress* progress;
    };
}
