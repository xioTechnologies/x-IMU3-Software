# x-IMU3 Software

This is main software repository for the [x-IMU3](https://x-io.co.uk/x-imu3/). The repository includes the x-IMU3 GUI and APIs with examples for C, C++, C#, Python, and Rust. The x-IMU3 GUI is written in C++ using [JUCE](https://juce.com/). The core API is written in Rust with a generated C interface layer. APIs in all other languages are wrappers for the C interface layer.

## Development Setup

1. Install C++ toolchain
    - Install [Visual Studio](https://visualstudio.microsoft.com/), including MSVC (Windows)
    - Install [Xcode](https://developer.apple.com/xcode/) (macOS)
2. Install Rust toolchain using [rustup](https://rustup.rs/)
3. Install [Clion](https://www.jetbrains.com/clion/), including Rust plugin
4. Configure Clion toolchain (Windows)
    - Settings > Build, Execution, Deployment > Toolchains > Add > Visual Studio, Architecture: x86_amd64

The Run/Debug Configuration may indicate "Error: No Rust toolchain specified" after cloning the repo for the first time. If this happens then open Settings/Preferences from the File menu, navigate to Rust under Languages & Frameworks and click OK.

## Branch Protection

The github repository applies the following branch protection rules to the main branch. All commits must be made to a non-protected branch and submitted via a pull request before they can be merged into the the main branch.

- *Require a pull request before merging*
- *Require linear history*
- *Include administrators*

## Release Process

1. Update version (edit and run [version.py](https://github.com/xioTechnologies/x-IMU3-Software/tree/master/version.py)) and commit
2. Push new version tag to master
3. Publish APIs
    - Python - Compile Rust-API then run [setup.py](https://github.com/xioTechnologies/x-IMU3-Software/tree/master/x-IMU3-API/Python/setup.py)
    - Rust - Run [publish.py](https://github.com/xioTechnologies/x-IMU3-Software/tree/master/x-IMU3-API/Rust/publish.py)
