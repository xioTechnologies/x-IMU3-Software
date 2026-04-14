use ximu3::file_converter::*;

pub fn run() {
    // Blocking
    let destination = "C:/";
    let name_blocking = "x-IMU3 File Conversion Example Blocking";
    let file_paths = vec!["C:/x-IMU3 Example File.ximu3"]; // replace with actual file path

    let progress = FileConverter::convert(destination, name_blocking, file_paths.clone());

    print_progress(progress);

    // Non-blocking
    let name_non_blocking = "x-IMU3 File Conversion Example Non-Blocking";

    let closure = Box::new(|progress| {
        print_progress(progress);
    });

    let _file_converter = FileConverter::new(destination, name_non_blocking, file_paths, closure);

    std::thread::sleep(std::time::Duration::from_secs(60));
}

fn print_progress(progress: FileConverterProgress) {
    println!("{}, {:.1}%, {} of {} bytes", progress.status, progress.percentage, progress.bytes_processed, progress.bytes_total);
    // println!("{progress}"); // alternative to above
}
