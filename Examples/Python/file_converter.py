import helpers
import time
import ximu3


def print_progress(progress):
    print(
        ", ".join(
            [
                ximu3.file_converter_status_to_string(progress.status),
                "{:.1f}%".format(progress.percentage),
                f"{progress.bytes_processed} of {progress.bytes_total} bytes",
            ]
        )
    )
    # print(progress.to_string())  # alternative to above


def callback(progress):
    print_progress(progress)


destination = "C:/"
name = "File Conversion Example"
file_paths = ["C:/file.ximu3"]  # replace with actual file path

if helpers.yes_or_no("Use async implementation?"):
    _ = ximu3.FileConverter(destination, name, file_paths, callback)
    time.sleep(60)
else:
    print_progress(ximu3.FileConverter.convert(destination, name, file_paths))
