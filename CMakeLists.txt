cmake_minimum_required(VERSION 3.22) #JUCE 8 requires CMake 3.22+

if (APPLE AND CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "arm64")
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Universal architectures")
endif()

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: Debug Release")

project(RNBO_JUCE_EXAMPLE VERSION 2.0.0)

set(CMAKE_CXX_STANDARD 17) #JUCE 8 requires 17
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)

set(RNBO_CPP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/export/rnbo/" CACHE FILEPATH "The path to the the RNBO c++ source directory")
set(RNBO_EXPORT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/export/" CACHE FILEPATH "path to your export directory")
set(RNBO_CLASS_FILE_NAME "rnbo_source.cpp" CACHE STRING "the name of your rnbo class file")
mark_as_advanced(RNBO_CLASS_FILE_NAME)

set(RNBO_CLASS_NAME "rnbomatic")
set(RNBO_CLASS_FILE "${RNBO_EXPORT_DIR}/${RNBO_CLASS_FILE_NAME}")
set(RNBO_DESCRIPTION_FILE "${RNBO_EXPORT_DIR}/description.json")
set(RNBO_PRESETS_FILE "${RNBO_EXPORT_DIR}/presets.json")
set(RNBO_BINARY_DATA_FILE "${RNBO_EXPORT_DIR}/${RNBO_CLASS_NAME}_binary.cpp")
set(RNBO_BINARY_DATA_STORAGE_NAME "${RNBO_CLASS_NAME}_binary")
set(PLUGIN_PARAM_DEFAULT_NOTIFY ON CACHE BOOL "Should parameter changes from inside your rnbo patch send output by default?")

# Choose which editor is shown when the plugin/app opens.
#   DEFAULT  — the generic editor provided by RNBO::JuceAudioProcessor
#   NATIVE   — native JUCE controls (src/CustomAudioEditor)
#   WEBVIEW  — WebBrowserComponent UI (src/WebBrowserAudioEditor)
set(RNBO_EDITOR_MODE "DEFAULT" CACHE STRING "UI editor: DEFAULT (Generic JUCE controls), NATIVE (Custom JUCE controls), WEBVIEW (WebBrowserComponent)")
set_property(CACHE RNBO_EDITOR_MODE PROPERTY STRINGS DEFAULT NATIVE WEBVIEW)

# Default path for the custom native UI
set(NATIVE_EDITOR_DIR "${CMAKE_CURRENT_LIST_DIR}/src/nativeui" CACHE STRING "Path to the custom native UI implementation, defaults to src/nativeui")

# Default path for the web UI
set(WEB_EDITOR_DIR "${CMAKE_CURRENT_LIST_DIR}/src/webui" CACHE STRING "Path to the web UI implementation, defaults to src/webui")
get_filename_component(WEB_EDITOR_DIR "${WEB_EDITOR_DIR}" ABSOLUTE BASE_DIR "${CMAKE_SOURCE_DIR}")

#write description header file if description.json exists, sets RNBO_INCLUDE_DESCRIPTION_FILE if the file exists
include(${RNBO_CPP_DIR}/cmake/RNBODescriptionHeader.cmake)
set(DESCRIPTION_INCLUDE_DIR ${CMAKE_BINARY_DIR}/include)
rnbo_write_description_header_if_exists(${RNBO_DESCRIPTION_FILE} ${DESCRIPTION_INCLUDE_DIR} ${RNBO_PRESETS_FILE})
include_directories(${DESCRIPTION_INCLUDE_DIR})

if (EXISTS ${RNBO_BINARY_DATA_FILE})
  file(GLOB RNBO_BINARY_DATA_FILES "${RNBO_EXPORT_DIR}/${RNBO_CLASS_NAME}_binary*.cpp")
  add_definitions(-DRNBO_BINARY_DATA_STORAGE_NAME=${RNBO_BINARY_DATA_STORAGE_NAME})
endif()

# Include the JUCE submodule, needed for JUCE-based CMake definitions
add_subdirectory(
  ${CMAKE_CURRENT_LIST_DIR}/thirdparty/juce
  ${CMAKE_BINARY_DIR}/juce
  EXCLUDE_FROM_ALL #don't build examples etc, also don't install
)

# Or, instead, use Conan to get JUCE, should speed up clean builds
# include(${RNBO_CPP_DIR}/cmake/RNBOJuce.cmake)

# Compile web UI files into the binary — only needed for the WEBVIEW editor.
if(RNBO_EDITOR_MODE STREQUAL "WEBVIEW")
    add_subdirectory(${WEB_EDITOR_DIR})
endif()

# Create the correct UI header depending on UI style
configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/src/ui-config.h.in"
    "${PROJECT_BINARY_DIR}/src/ui-config.h"
)

# Optionally, enable ccache, should speed up builds
# include(${RNBO_CPP_DIR}/cmake/CCache.cmake)

# Comment out this line if you really want to emulate MIDI CC with Audio Parameters.
# See the discussion here: https://forums.steinberg.net/t/vst3-and-midi-cc-pitfall/201879/11
add_compile_definitions(JUCE_VST3_EMULATE_MIDI_CC_WITH_PARAMETERS=0)

# setup your application, you can remove this include if you don't want to build applications
include(${CMAKE_CURRENT_LIST_DIR}/App.cmake)

# setup your plugin(s), you can remove this include if you don't want to build plugins
include(${CMAKE_CURRENT_LIST_DIR}/Plugin.cmake)
