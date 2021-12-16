import helpers
import ximu3


def print_progress(progress):
    print(progress.status + ", " +
          "{:.1f}".format(progress.percentage) + "%, " +
          str(progress.bytes_processed) + " of " +
          str(progress.file_size) + " bytes")
    # print(progress.to_string())  # alternative to above


def callback(progress):
    print_progress(progress)


destination = "C:/"
source = "C:/file.ximu3"

if helpers.yes_or_no("Use async implementation?"):
    _ = ximu3.FileConverter(destination, source, callback)
    helpers.wait(-1)
else:
    print_progress(ximu3.FileConverter.convert(destination, source))
