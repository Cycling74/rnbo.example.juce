cmake_minimum_required(VERSION 3.15)

# On M1 Macs, uncomment to enable universal binaries
# set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "")

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: Debug Release")

project(RNBO_JUCE_EXAMPLE VERSION 2.0.0)

set(CMAKE_CXX_STANDARD 14) #JUCE requires 14
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

#write description header file if description.json exists, sets RNBO_INCLUDE_DESCRIPTION_FILE if the file exists
include(${RNBO_CPP_DIR}/cmake/RNBODescriptionHeader.cmake)
set(DESCRIPTION_INCLUDE_DIR ${CMAKE_BINARY_DIR}/include)
rnbo_write_description_header_if_exists(${RNBO_DESCRIPTION_FILE} ${DESCRIPTION_INCLUDE_DIR} ${RNBO_PRESETS_FILE})
include_directories(${DESCRIPTION_INCLUDE_DIR})

if (EXISTS ${RNBO_BINARY_DATA_FILE})
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


# Optionally, enable ccache, should speed up builds
# include(${RNBO_CPP_DIR}/cmake/CCache.cmake)


# Comment out this line if you really want to emulate MIDI CC with Audio Parameters.
# See the discussion here: https://forums.steinberg.net/t/vst3-and-midi-cc-pitfall/201879/11
add_compile_definitions(JUCE_VST3_EMULATE_MIDI_CC_WITH_PARAMETERS=0)

# setup your application, you can remove this include if you don't want to build applications
include(${CMAKE_CURRENT_LIST_DIR}/App.cmake)

# setup your plugin(s), you can remove this include if you don't want to build plugins
include(${CMAKE_CURRENT_LIST_DIR}/Plugin.cmake)
