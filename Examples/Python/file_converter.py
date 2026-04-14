import time

import ximu3


def print_progress(progress: ximu3.FileConverterProgress) -> None:
    print(
        ", ".join(
            [
                ximu3.file_converter_status_to_string(progress.status),
                "{:.1f}%".format(progress.percentage),
                f"{progress.bytes_processed} of {progress.bytes_total} bytes",
            ]
        )
    )
    # print(progress)  # alternative to above


def callback(progress: ximu3.FileConverterProgress) -> None:
    print_progress(progress)


# Blocking
destination = "C:/"
name_blocking = "x-IMU3 File Conversion Example Blocking"
file_paths = ["C:/x-IMU3 Example File.ximu3"]  # replace with actual file path

progress = ximu3.FileConverter.convert(destination, name_blocking, file_paths)

print_progress(progress)

# Non-blocking
name_non_blocking = "x-IMU3 File Conversion Example Non-Blocking"

_ = ximu3.FileConverter(destination, name_non_blocking, file_paths, callback)

time.sleep(60)
