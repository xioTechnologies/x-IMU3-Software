[![Tags](https://img.shields.io/github/v/tag/xioTechnologies/x-IMU3-Software.svg)](https://github.com/xioTechnologies/x-IMU3-Software/tags/)
[![Build](https://img.shields.io/github/actions/workflow/status/xioTechnologies/x-IMU3-Software/main.yml?branch=main)](https://github.com/xioTechnologies/x-IMU3-Software/actions/workflows/main.yml)
[![Pypi](https://img.shields.io/pypi/v/ximu3.svg)](https://pypi.org/project/ximu3/)
[![Python](https://img.shields.io/pypi/pyversions/ximu3.svg)](https://pypi.org/project/ximu3/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

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

## Branch Protection

The github repository applies the following branch protection rules to the main branch. All commits must be made to a non-protected branch and submitted via a pull request before they can be merged into the the main branch.

- *Require a pull request before merging*
- *Require linear history*
- *Include administrators*
