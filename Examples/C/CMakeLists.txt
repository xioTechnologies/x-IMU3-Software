file(GLOB_RECURSE files "*.c")

add_executable(C-Examples ${files})

target_include_directories(C-Examples PUBLIC "../../x-IMU3-API/C")

if (APPLE)
    target_link_libraries(C-Examples PRIVATE "-framework Cocoa" "-framework IOKit")
endif ()
