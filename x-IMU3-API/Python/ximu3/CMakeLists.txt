file(GLOB_RECURSE files "*.c")

add_library(ximu3-Python-Package ${files})

find_package(Python COMPONENTS Development)

target_link_libraries(ximu3-Python-Package PRIVATE Python::Python)
