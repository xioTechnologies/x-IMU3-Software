import time

import ximu3


def callback(progress: ximu3.FileConverterProgress) -> None:
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


# Blocking
destination = "C:/Users/Public/"
name_blocking = "x-IMU3 File Converter Example Blocking"
file_paths = ["C:/Users/Public/x-IMU3 Example File.ximu3"]  # replace with actual file path

ximu3.FileConverter.convert(destination, name_blocking, file_paths)

# Non-blocking
name_non_blocking = "x-IMU3 File Converter Example Non-Blocking"

_ = ximu3.FileConverter(destination, name_non_blocking, file_paths, callback)

time.sleep(60)
