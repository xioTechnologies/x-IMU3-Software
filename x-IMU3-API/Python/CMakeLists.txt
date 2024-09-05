file(GLOB_RECURSE files "*.c")

add_library(Python-C-API ${files})

find_package(Python3 COMPONENTS Development)

target_include_directories(Python-C-API PRIVATE ${Python3_INCLUDE_DIRS})
