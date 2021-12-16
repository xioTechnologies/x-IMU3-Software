#include "../../../x-IMU3-API/C/Ximu3.h"
#include "../Helpers.h"
#include <inttypes.h> // PRIu64
#include <stdio.h>

static void Callback(const XIMU3_FileConverterProgress progress, void* context);

static void PrintProgress(const XIMU3_FileConverterProgress progress);

void FileConverter()
{
    const char* destination = "C:/";
    const char* source = "C:/file.ximu3";

    if (YesOrNo("Use async implementation?") == true)
    {
        XIMU3_FileConverter* const file_converter = XIMU3_file_converter_new(destination, source, Callback, NULL);
        Wait(-1);
        XIMU3_file_converter_free(file_converter);
    }
    else
    {
        PrintProgress(XIMU3_file_converter_convert(destination, source));
    }
}

static void Callback(const XIMU3_FileConverterProgress progress, void* context)
{
    PrintProgress(progress);
}

static void PrintProgress(const XIMU3_FileConverterProgress progress)
{
    printf("%s, %0.1f%%, %" PRIu64 " of %" PRIu64 " bytes\n",
           XIMU3_file_converter_status_to_string(progress.status),
           progress.percentage,
           progress.bytes_processed,
           progress.file_size);
    // printf("%s\n", XIMU3_file_converter_progress_to_string(progress)); // alternative to above
}
