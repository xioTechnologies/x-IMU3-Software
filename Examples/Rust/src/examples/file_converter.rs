use ximu3::file_converter::*;
use crate::helpers;

pub fn run() {
    let destination = "C:/";
    let name = "File Conversion Example";
    let file_paths = vec!["C:/file.ximu3"];

    if helpers::ask_question("Use async implementation?") {
        let _file_converter = FileConverter::new(destination, name, file_paths, Box::new(|progress| {
            print_progress(progress);
        }));

        std::thread::sleep(std::time::Duration::from_secs(60));
    } else {
        print_progress(FileConverter::convert(destination, name, file_paths));
    }
}

fn print_progress(progress: FileConverterProgress) {
    println!("{}, {:.1}%, {} of {} bytes",
             progress.status,
             progress.percentage,
             progress.bytes_processed,
             progress.bytes_total);
    // println!("{}", progress); // alternative to above
}
