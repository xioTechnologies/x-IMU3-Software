file(GLOB_RECURSE files "*.c")

add_library(ximu3-Python-Package ${files})

find_package(Python3 COMPONENTS Development)

target_include_directories(ximu3-Python-Package PRIVATE ${Python3_INCLUDE_DIRS})
