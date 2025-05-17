extern crate cbindgen;

use cbindgen::Config;
use std::env;
use std::path::Path;

fn main() {
    let crate_dir = env::var("CARGO_MANIFEST_DIR").unwrap();
    let crate_path = Path::new(&crate_dir);
    let config = Config::from_root_or_default(crate_path);

    #[rustfmt::skip]
    cbindgen::Builder::new()
        .with_crate(crate_path.to_str().unwrap())
        .with_config(config)
        .generate()
        .expect("Unable to generate bindings")
        .write_to_file("../C/Ximu3.h");
}
