include(FetchContent)
Set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
        JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG 7.0.12
        GIT_PROGRESS TRUE
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

Set(CMAKE_DISABLE_FIND_PACKAGE_ZLIB TRUE)
Set(CMAKE_DISABLE_FIND_PACKAGE_PNG TRUE)
Set(CMAKE_DISABLE_FIND_PACKAGE_BZip2 TRUE)
Set(CMAKE_DISABLE_FIND_PACKAGE_BrotliDec TRUE)
Set(CMAKE_DISABLE_FIND_PACKAGE_HarfBuzz TRUE)
FetchContent_Declare(
        freetype
        GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
        GIT_TAG VER-2-13-2
        GIT_PROGRESS TRUE
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(freetype)
FetchContent_GetProperties(freetype SOURCE_DIR FREETYPE_SRC_DIR)
set(FREETYPE_INCLUDE_DIRS ${FREETYPE_SRC_DIR}/include)
set(FREETYPE_LIBRARY freetype)
set(FREETYPE_LIBRARIES freetype)
find_package(Freetype REQUIRED CONFIGS "freetype-config-version.cmake" PATHS "${FREETYPE_SRC_DIR}/../freetype-build/" NO_DEFAULT_PATH)

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG 1.0.1
        GIT_PROGRESS TRUE
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glm)

juce_add_gui_app(x-IMU3-GUI
        ICON_BIG "Images/xio_icon.png"
        ICON_SMALL "Images/xio_icon.png"
        PRODUCT_NAME "x-IMU3 GUI" # The name of the final executable, which can differ from the target name
        BUNDLE_ID "uk.co.x-io.x-IMU3-GUI")

FILE(GLOB_RECURSE CPP_FILES "Source/*.cpp")
target_sources(x-IMU3-GUI PRIVATE ${CPP_FILES})
if (APPLE)
    target_sources(x-IMU3-GUI PRIVATE Source/UIUtilsMac.mm)
endif ()

target_compile_definitions(x-IMU3-GUI
        PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:x-IMU3-GUI,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:x-IMU3-GUI,JUCE_VERSION>"
        JUCE_OPENGL3=1
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_MODAL_LOOPS_PERMITTED=1
        GLM_FORCE_SILENT_WARNINGS
        GLM_FORCE_SIZE_T_LENGTH) # Resolves signed conversion warning on Ubuntu

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

target_link_libraries(x-IMU3-GUI
        PRIVATE
        x-IMU3-GUI-AppData
        juce::juce_opengl
        "${FREETYPE_LIBRARIES}"
        glm::glm
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

set_property(TARGET x-IMU3-GUI PROPERTY CXX_STANDARD 20)
