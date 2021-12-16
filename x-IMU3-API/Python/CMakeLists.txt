file(GLOB_RECURSE files "*.c")

add_library(Python-C-API ${files})

find_package(PythonLibs)

target_include_directories(Python-C-API PRIVATE ${PYTHON_INCLUDE_PATH})

target_link_libraries(Python-C-API PRIVATE ${PYTHON_LIBRARIES})
