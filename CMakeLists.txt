cmake_minimum_required(VERSION 3.15)

project(RNBO_JUCE_EXAMPLE VERSION 1.2.0)

set(CMAKE_CXX_STANDARD 14) #JUCE requires 14
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# On M1 Macs, uncomment to enable universal binaries
# set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: Debug Release" FORCE)
endif()

set(RNBO_CPP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/export/code/rnbo/" CACHE FILEPATH "The path to the the RNBO c++ source directory")
set(RNBO_CLASS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/export/code/rnbo_source.cpp" CACHE FILEPATH "The file that holds the generated RNBO class code")

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
