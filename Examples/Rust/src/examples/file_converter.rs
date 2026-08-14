use ximu3::file_converter::*;

pub fn run() {
    // Blocking
    let destination = "C:/Users/Public/";
    let name_blocking = "x-IMU3 File Conversion Example Blocking";
    let file_paths = vec!["C:/Users/Public/x-IMU3 Example File.ximu3"]; // replace with actual file path

    let result = FileConverter::convert(destination, name_blocking, file_paths.clone());

    if let Err(error) = result {
        println!("File converter failed: {error}");
    }

    // Non-blocking
    let name_non_blocking = "x-IMU3 File Conversion Example Non-Blocking";

    let closure = Box::new(|progress: FileConverterProgress| {
        println!("{}, {:.1}%, {} of {} bytes", progress.status, progress.percentage, progress.bytes_processed, progress.bytes_total);
        // println!("{progress}"); // alternative to above
    });

    let file_converter = FileConverter::new(destination, name_non_blocking, file_paths, closure);

    if let Err(error) = file_converter {
        println!("File converter failed: {error}");
    }

    std::thread::sleep(std::time::Duration::from_secs(60));
}
