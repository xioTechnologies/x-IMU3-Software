file(GLOB_RECURSE files "*.cpp")

add_executable(Cpp-Examples ${files})

set_property(TARGET Cpp-Examples PROPERTY CXX_STANDARD 17)

if (APPLE)
    target_link_libraries(Cpp-Examples PRIVATE "-framework Cocoa" "-framework IOKit")
endif ()
