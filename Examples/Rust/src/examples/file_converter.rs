use ximu3::file_converter::*;
use crate::helpers;

pub fn run() {
    let destination = "C:/";
    let source = "C:/file.ximu3";

    if helpers::yes_or_no("Use async implementation?") {
        let _file_converter = FileConverter::new(destination, source, Box::new(|progress| {
            print_progress(progress);
        }));

        std::thread::sleep(std::time::Duration::from_secs(60));
    } else {
        print_progress(FileConverter::convert(destination, source));
    }
}

fn print_progress(progress: FileConverterProgress) {
    println!("{}, {:.1}%, {} of {} bytes",
             progress.status,
             progress.percentage,
             progress.bytes_processed,
             progress.file_size);
    // println!("{}", progress); // alternative to above
}
