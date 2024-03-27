[![tag](https://img.shields.io/github/v/tag/xioTechnologies/x-IMU3-Software.svg)](https://github.com/xioTechnologies/x-IMU3-Software/tags/)
[![build](https://img.shields.io/github/actions/workflow/status/xioTechnologies/x-IMU3-Software/main.yml?branch=main)](https://github.com/xioTechnologies/x-IMU3-Software/actions/workflows/main.yml)
[![pypi](https://img.shields.io/pypi/v/ximu3.svg)](https://pypi.org/project/ximu3/)
[![python](https://img.shields.io/pypi/pyversions/ximu3.svg)](https://pypi.org/project/ximu3/)
[![License](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)

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
5. Select Ninja to improve compile time (Windows)
    - Settings > Build, Execution, Deployment > CMake > Generator: Ninja

## Distributions

- [C API for Windows, macOS, Linux, iOS, and Android](https://github.com/xioTechnologies/x-IMU3-Software/releases/latest)
- [Rust crate](https://crates.io/crates/ximu3/)
- [Python package](https://pypi.org/project/ximu3/)
- [x-IMU3 GUI for Windows](https://github.com/xioTechnologies/x-IMU3-Software/releases/latest/download/x-IMU3-GUI-Setup.exe)
- [x-IMU3 GUI for macOS (Intel)](https://github.com/xioTechnologies/x-IMU3-Software/releases/latest/download/x-IMU3-GUI-Intel.dmg)
- [x-IMU3 GUI for macOS (Apple Silicon)](https://github.com/xioTechnologies/x-IMU3-Software/releases/latest/download/x-IMU3-GUI-Apple-Silicon.dmg)
- [x-IMU3 GUI for Ubuntu ](https://github.com/xioTechnologies/x-IMU3-Software/releases/latest/download/x-IMU3-GUI)

## Licence

The x-IMU3 Software is covered by the [MIT Licence](https://opensource.org/licenses/MIT) except for the x-IMU3 GUI which uses [JUCE](https://juce.com/) and so is covered by the [GNU GPLv3 License](https://www.gnu.org/licenses/gpl-3.0) as per the [JUCE 7 EULA](https://juce.com/legal/juce-7-license/).
