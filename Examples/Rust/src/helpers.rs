use std::io::*;

pub fn get_key() -> char {
    let buffer = &mut String::new();

    stdin().read_line(buffer).ok();

    buffer.chars().nth(0).unwrap().to_ascii_uppercase()
}

pub fn yes_or_no(question: &str) -> bool {
    loop {
        println!("{} [Y/N]", question);

        match get_key() {
            'Y' => return true,
            'N' => return false,
            _ => continue,
        }
    }
}

pub fn wait(seconds: i32) {
    if seconds < 0 {
        loop {
            std::thread::sleep(std::time::Duration::from_secs(1));
        }
    } else {
        std::thread::sleep(std::time::Duration::from_secs(seconds as u64));
    }
}
