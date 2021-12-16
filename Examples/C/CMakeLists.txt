file(GLOB_RECURSE files "*.c")

add_executable(C-Examples ${files})

if (APPLE)
    target_link_libraries(C-Examples PRIVATE "-framework Cocoa" "-framework IOKit")
endif ()
