use std::io::*;

pub fn get_key() -> char {
    let buffer = &mut String::new();

    stdin().read_line(buffer).ok();

    buffer.chars().next().unwrap_or_default().to_ascii_uppercase()
}

pub fn yes_or_no(question: &str) -> bool {
    loop {
        print!("{question} [Y/N] ");

        std::io::stdout().flush().unwrap();

        match get_key() {
            'Y' => return true,
            'N' => return false,
            _ => continue,
        }
    }
}
