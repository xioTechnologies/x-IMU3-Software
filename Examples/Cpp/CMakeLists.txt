file(GLOB_RECURSE files "*.cpp")

add_executable(Cpp-Examples ${files})

set_property(TARGET Cpp-Examples PROPERTY CXX_STANDARD 20)

target_include_directories(Cpp-Examples PUBLIC "../../x-IMU3-API/Cpp")

if (APPLE)
    target_link_libraries(Cpp-Examples PRIVATE "-framework Cocoa" "-framework IOKit")
endif ()

target_sources(Cpp-Examples PRIVATE "../../x-IMU3-API/Cpp/ConnectionConfig.cpp")
