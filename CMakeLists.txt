cmake_minimum_required(VERSION 3.22)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "")
project(x-IMU3-Software VERSION 1.4.5)

add_custom_target(Rust-API COMMAND cargo build --manifest-path=${CMAKE_SOURCE_DIR}/x-IMU3-API/Rust/Cargo.toml --release)

find_library(XIMU3 NAMES ximu3 PATHS "${CMAKE_SOURCE_DIR}/x-IMU3-API/Rust/target/release" NO_DEFAULT_PATH)

if (NOT XIMU3)
    message(WARNING "Please provide the missing library file and then reload the CMake project. https://github.com/xioTechnologies/x-IMU3-Software/blob/main/x-IMU3-API/Rust/target/release/README.md")
    return()
endif ()

add_definitions(-D_CRT_SECURE_NO_WARNINGS) # disable MSVC warnings for unsafe functions, e.g. strcpy

add_subdirectory("Examples/C")
add_subdirectory("Examples/Cpp")
add_subdirectory("x-IMU3-API/Python")
add_subdirectory("x-IMU3-Bootloader")
add_subdirectory("x-IMU3-GUI")

target_link_libraries(x-IMU3-GUI PRIVATE ${XIMU3})
target_link_libraries(C-Examples PRIVATE ${XIMU3})
target_link_libraries(Cpp-Examples PRIVATE ${XIMU3})

if (UNIX AND NOT APPLE)
    target_link_libraries(x-IMU3-GUI PRIVATE pthread dl m udev)
    target_link_libraries(C-Examples PRIVATE pthread dl m udev)
    target_link_libraries(Cpp-Examples PRIVATE pthread dl m udev)
endif ()

target_link_libraries(x-IMU3-GUI PRIVATE x-IMU3-Bootloader)
