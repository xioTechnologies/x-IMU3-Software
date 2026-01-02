#include "../helpers.h"
#include <inttypes.h>
#include <stdio.h>
#include "Ximu3.h"

static void callback(const XIMU3_FileConverterProgress progress, void* context);

static void print_progress(const XIMU3_FileConverterProgress progress);

void file_converter()
{
    const char* destination = "C:/";
    const char* name = "x-IMU3 File Conversion Example";
    const char* file_paths[] = { "C:/x-IMU3 Example File.ximu3" }; // replace with actual file path
    const int number_of_files = sizeof(file_paths) / sizeof(file_paths[0]);

    if (yes_or_no("Use async implementation?"))
    {
        XIMU3_FileConverter* const file_converter = XIMU3_file_converter_new(destination, name, file_paths, number_of_files, callback, NULL);

        sleep(60);

        XIMU3_file_converter_free(file_converter);
    }
    else
    {
        print_progress(XIMU3_file_converter_convert(destination, name, file_paths, number_of_files));
    }
}

static void callback(const XIMU3_FileConverterProgress progress, void* context)
{
    print_progress(progress);
}

static void print_progress(const XIMU3_FileConverterProgress progress)
{
    printf("%s, %0.1f%%, %" PRIu64 " of %" PRIu64 " bytes\n",
           XIMU3_file_converter_status_to_string(progress.status),
           progress.percentage,
           progress.bytes_processed,
           progress.bytes_total);
    // printf("%s\n", XIMU3_file_converter_progress_to_string(progress)); // alternative to above
}
