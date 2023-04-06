# This file is modified from https://github.com/juce-framework/JUCE/blob/master/examples/CMake/GuiApp/CMakeLists.txt

Set(FETCHCONTENT_QUIET FALSE)

# Fetch JUCE
FetchContent_Declare(
        JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG 7.0.5
        GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(JUCE)

Set(CMAKE_DISABLE_FIND_PACKAGE_ZLIB TRUE)
Set(CMAKE_DISABLE_FIND_PACKAGE_PNG TRUE)
Set(CMAKE_DISABLE_FIND_PACKAGE_BZip2 TRUE)
Set(CMAKE_DISABLE_FIND_PACKAGE_BrotliDec TRUE)
Set(CMAKE_DISABLE_FIND_PACKAGE_HarfBuzz TRUE)

# Fetch FreeType
FetchContent_Declare(
        freetype
        GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
        GIT_TAG VER-2-12-1
        GIT_PROGRESS TRUE
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(freetype)
FetchContent_GetProperties(freetype SOURCE_DIR FREETYPE_SRC_DIR)

set(FREETYPE_INCLUDE_DIRS ${FREETYPE_SRC_DIR}/include)
set(FREETYPE_LIBRARY freetype)
set(FREETYPE_LIBRARIES freetype)

find_package(Freetype REQUIRED CONFIGS "freetype-config-version.cmake" PATHS "${FREETYPE_SRC_DIR}/../freetype-build/" NO_DEFAULT_PATH)

# Example GUI App CMakeLists.txt

# To get started on a new GUI app, copy this entire folder (containing this file and C++ sources) to
# a convenient location, and then start making modifications. For other examples of CMakeLists for
# GUI apps, check `extras/Projucer` and `examples/DemoRunner` in the JUCE repo.

# The first line of any CMake project should be a call to `cmake_minimum_required`, which checks
# that the installed CMake will be able to understand the following CMakeLists, and ensures that
# CMake's behaviour is compatible with the named version. This is a standard CMake command, so more
# information can be found in the CMake docs.

cmake_minimum_required(VERSION 3.15)

# The top-level CMakeLists.txt file for a project must contain a literal, direct call to the
# `project()` command. `project()` sets up some helpful variables that describe source/binary
# directories, and the current project version. This is a standard CMake command.

project(X_IMU3_GUI VERSION 1.1.0)

# If you've installed JUCE somehow (via a package manager, or directly using the CMake install
# target), you'll need to tell this project that it depends on the installed copy of JUCE. If you've
# included JUCE directly in your source tree (perhaps as a submodule), you'll need to tell CMake to
# include that subdirectory as part of the build.

# find_package(JUCE CONFIG REQUIRED)        # If you've installed JUCE to your system
# or
# add_subdirectory(JUCE)                    # If you've put JUCE in a subdirectory called JUCE

# If your app depends the VST2 SDK, perhaps to host VST2 plugins, CMake needs to be told where
# to find the SDK on your system. This setup should be done before calling `juce_add_gui_app`.

# juce_set_vst2_sdk_path(...)

# `juce_add_gui_app` adds an executable target with the name passed as the first argument
# (GuiAppExample here). This target is a normal CMake target, but has a lot of extra properties set
# up by default. This function accepts many optional arguments. Check the readme at
# `docs/CMake API.md` in the JUCE repo for the full list.

juce_add_gui_app(x-IMU3-GUI
        # VERSION ...                       # Set this if the app version is different to the project version
        ICON_BIG "Images/xio_icon.png"
        ICON_SMALL "Images/xio_icon.png"
        # DOCUMENT_EXTENSIONS ...           # Specify file extensions that should be associated with this app
        # COMPANY_NAME ...                  # Specify the name of the app's author
        PRODUCT_NAME "x-IMU3 GUI")     # The name of the final executable, which can differ from the target name

# `juce_generate_juce_header` will create a JuceHeader.h for a given target, which will be generated
# into your build tree. This should be included with `#include <JuceHeader.h>`. The include path for
# this header will be automatically added to the target. The main function of the JuceHeader is to
# include all your JUCE module headers; if you're happy to include module headers directly, you
# probably don't need to call this.

# juce_generate_juce_header(x-IMU3-GUI)

# `target_sources` adds source files to a target. We pass the target that needs the sources as the
# first argument, then a visibility parameter for the sources which should normally be PRIVATE.
# Finally, we supply a list of source files that will be built into the target. This is a standard
# CMake command.

FILE(GLOB_RECURSE CPP_FILES "Source/*.cpp")

target_sources(x-IMU3-GUI
        PRIVATE
        ${CPP_FILES})

if (APPLE)
    target_sources(x-IMU3-GUI PRIVATE Source/UIUtilsMac.mm)
endif ()

# `target_compile_definitions` adds some preprocessor definitions to our target. In a Projucer
# project, these might be passed in the 'Preprocessor Definitions' field. JUCE modules also make use
# of compile definitions to switch certain features on/off, so if there's a particular feature you
# need that's not on by default, check the module header for the correct flag to set here. These
# definitions will be visible both to your code, and also the JUCE module code, so for new
# definitions, pick unique names that are unlikely to collide! This is a standard CMake command.

target_compile_definitions(x-IMU3-GUI
        PRIVATE
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_gui_app` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_gui_app` call
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:x-IMU3-GUI,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:x-IMU3-GUI,JUCE_VERSION>"
        JUCE_OPENGL3=1
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_MODAL_LOOPS_PERMITTED=1)

# If your target needs extra binary assets, you can add them here. The first argument is the name of
# a new static library target that will include all the binary resources. There is an optional
# `NAMESPACE` argument that can specify the namespace of the generated binary data class. Finally,
# the SOURCES argument should be followed by a list of source files that should be built into the
# static library. These source files can be of any kind (wav data, images, fonts, icons etc.).
# Conversion to binary-data will happen when your target is built.

FILE(GLOB_RECURSE FONT_FILES "Fonts/*.*")
FILE(GLOB_RECURSE IMAGE_FILES "Images/*.*")
FILE(GLOB FIRMWARE_FILE "Source/Firmware/*.hex")
FILE(GLOB_RECURSE OPEN_GL_FILES "Source/OpenGL/Assets/*.*")

juce_add_binary_data(x-IMU3-GUI-AppData SOURCES
        "Source/DeviceSettings/DeviceSettingsEnums.xml"
        "Source/DeviceSettings/DeviceSettings.xml"
        "Source/Dialogs/CommandKeys.xml"
        ${FONT_FILES}
        ${IMAGE_FILES}
        ${FIRMWARE_FILE}
        ${OPEN_GL_FILES})

# `target_link_libraries` links libraries and JUCE modules to other libraries or executables. Here,
# we're linking our executable target to the `juce::juce_gui_extra` module. Inter-module
# dependencies are resolved automatically, so `juce_core`, `juce_events` and so on will also be
# linked automatically. If we'd generated a binary data target above, we would need to link to it
# here too. This is a standard CMake command.

target_link_libraries(x-IMU3-GUI
        PRIVATE
        x-IMU3-GUI-AppData
        juce::juce_gui_extra
        juce::juce_opengl
        "${FREETYPE_LIBRARIES}"
        PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

target_include_directories(x-IMU3-GUI
        PRIVATE
        "Source"
        "../x-IMU3-API/Cpp"
        "${FREETYPE_INCLUDE_DIRS}"
        "../x-IMU3-Bootloader")

add_dependencies(x-IMU3-GUI freetype)

set_property(TARGET x-IMU3-GUI PROPERTY CXX_STANDARD 17)
