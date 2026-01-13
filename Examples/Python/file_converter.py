import time

import helpers
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


destination = "C:/"
name = "x-IMU3 File Conversion Example"
file_paths = ["C:/x-IMU3 Example File.ximu3"]  # replace with actual file path

if helpers.yes_or_no("Use async implementation?"):
    _ = ximu3.FileConverter(destination, name, file_paths, callback)
    time.sleep(60)
else:
    print_progress(ximu3.FileConverter.convert(destination, name, file_paths))
