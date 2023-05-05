use std::io::*;

pub fn get_key() -> char {
    let buffer = &mut String::new();

    stdin().read_line(buffer).ok();

    buffer.chars().nth(0).unwrap().to_ascii_uppercase()
}

pub fn ask_question(question: &str) -> bool {
    loop {
        println!("{} [Y/N]", question);

        match get_key() {
            'Y' => return true,
            'N' => return false,
            _ => continue,
        }
    }
}
