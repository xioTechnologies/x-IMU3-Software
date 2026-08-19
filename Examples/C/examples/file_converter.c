#include "../helpers.h"
#include <inttypes.h>
#include <stdio.h>
#include "Ximu3.h"

static void callback(const XIMU3_FileConverterProgress progress, void *context);

void file_converter() {
    // Blocking
    const char *destination = "C:/Users/Public/";
    const char *nameBlocking = "x-IMU3 File Converter Example Blocking";
    const char *file_paths[] = {"C:/Users/Public/x-IMU3 Example File.ximu3"}; // replace with actual file path
    const int number_of_files = sizeof(file_paths) / sizeof(file_paths[0]);

    const XIMU3_Result result = XIMU3_file_converter_convert(destination, nameBlocking, file_paths, number_of_files);

    if (result != XIMU3_ResultOk) {
        printf("File converter failed: %s\n", XIMU3_result_to_string(result));
    }

    // Non-blocking
    const char *nameNonBlocking = "x-IMU3 File Converter Example Non-Blocking";

    XIMU3_FileConverter *const file_converter = XIMU3_file_converter_new(destination, nameNonBlocking, file_paths, number_of_files, callback, NULL);

    const XIMU3_Result resultNonBlocking = XIMU3_file_converter_get_result(file_converter);

    if (resultNonBlocking != XIMU3_ResultOk) {
        printf("File converter failed: %s\n", XIMU3_result_to_string(resultNonBlocking));
    }

    sleep(60);

    XIMU3_file_converter_free(file_converter);
}

static void callback(const XIMU3_FileConverterProgress progress, void *context) {
    printf("%s, %0.1f%%, %" PRIu64 " of %" PRIu64 " bytes\n",
           XIMU3_file_converter_status_to_string(progress.status),
           progress.percentage,
           progress.bytes_processed,
           progress.bytes_total);
    // printf("%s\n", XIMU3_file_converter_progress_to_string(progress)); // alternative to above
}
