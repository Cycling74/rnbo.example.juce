cmake_minimum_required(VERSION 3.15)

project(RNBO_JUCE_EXAMPLE VERSION 1.2.0)

set(CMAKE_CXX_STANDARD 14) #JUCE requires 14
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: Debug Release" FORCE)
endif()

set(RNBO_CPP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/export/code/rnbo/" CACHE FILEPATH "The path to the the RNBO c++ source directory")
set(RNBO_CLASS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/export/code/rnbo_source.cpp" CACHE FILEPATH "The file that holds the generated RNBO class code")

# Include the JUCE submodule, needed for JUCE-based CMake definitions
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/thirdparty/juce ${CMAKE_BINARY_DIR}/juce)


# Or, instead, use Conan to get JUCE, should speed up clean builds
# include(${RNBO_CPP_DIR}/cmake/RNBOJuce.cmake)

# Optionally, enable ccache, should speed up builds
# include(${RNBO_CPP_DIR}/cmake/CCache.cmake)



### START OF APP SECTION

# Feel free to delete this APP SECTION if you don't care to build apps

# If your app depends the VST2 SDK, perhaps to host VST2 plugins, CMake needs to be told where
# to find the SDK on your system. This setup should be done before calling `juce_add_gui_app`.

# juce_set_vst2_sdk_path(...)

# `juce_add_gui_app` adds an executable target with the name passed as the first argument
# (RNBOApp here). This target is a normal CMake target, but has a lot of extra properties set
# up by default. This function accepts many optional arguments. Check the readme at
# `docs/CMake API.md` in the JUCE repo for the full list.

juce_add_gui_app(RNBOApp
  # VERSION ...                       # Set this if the app version is different to the project version
  # ICON_BIG ...                      # ICON_* arguments specify a path to an image file to use as an icon
  # ICON_SMALL ...
  # DOCUMENT_EXTENSIONS ...           # Specify file extensions that should be associated with this app
  COMPANY_NAME "cycling74"            # Specify the name of the app's author
  PRODUCT_NAME "RNBO App Example")    # The name of the final executable, which can differ from the target name

# `juce_generate_juce_header` will create a JuceHeader.h for a given target, which will be generated
# into your build tree. This should be included with `#include <JuceHeader.h>`. The include path for
# this header will be automatically added to the target. The main function of the JuceHeader is to
# include all your JUCE module headers; if you're happy to include module headers directly, you
# probably don't need to call this.

# the RNBO adapters currently need this
juce_generate_juce_header(RNBOApp)

# `target_sources` adds source files to a target. We pass the target that needs the sources as the
# first argument, then a visibility parameter for the sources which should normally be PRIVATE.
# Finally, we supply a list of source files that will be built into the target. This is a standard
# CMake command.

target_sources(RNBOApp
  PRIVATE
  src/Main.cpp
  src/MainComponent.cpp

  ${RNBO_CLASS_FILE}

  ${RNBO_CPP_DIR}/RNBO.cpp
  ${RNBO_CPP_DIR}/adapters/juce/RNBO_JuceAudioProcessorUtils.cpp
  ${RNBO_CPP_DIR}/adapters/juce/RNBO_JuceAudioProcessorEditor.cpp
  ${RNBO_CPP_DIR}/adapters/juce/RNBO_JuceAudioProcessor.cpp
  )

include_directories(
  "${RNBO_CPP_DIR}/"
  "${RNBO_CPP_DIR}/src"
  "${RNBO_CPP_DIR}/common/"
  "${RNBO_CPP_DIR}/adapters/juce/"
  )

target_compile_definitions(RNBOApp
  PRIVATE
  # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
  JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_gui_app` call
  JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_gui_app` call
  JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:RNBOApp,JUCE_PRODUCT_NAME>"
  JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:RNBOApp,JUCE_VERSION>")

# If your target needs extra binary assets, you can add them here. The first argument is the name of
# a new static library target that will include all the binary resources. There is an optional
# `NAMESPACE` argument that can specify the namespace of the generated binary data class. Finally,
# the SOURCES argument should be followed by a list of source files that should be built into the
# static library. These source files can be of any kind (wav data, images, fonts, icons etc.).
# Conversion to binary-data will happen when your target is built.

# juce_add_binary_data(GuiAppData SOURCES ...)

# `target_link_libraries` links libraries and JUCE modules to other libraries or executables. Here,
# we're linking our executable target to the `juce::juce_gui_extra` module. Inter-module
# dependencies are resolved automatically, so `juce_core`, `juce_events` and so on will also be
# linked automatically. If we'd generated a binary data target above, we would need to link to it
# here too. This is a standard CMake command.
target_link_libraries(RNBOApp
  PRIVATE
  # GuiAppData            # If we'd created a binary data target, we'd link to it here
  juce::juce_gui_extra
  juce::juce_audio_basics
  juce::juce_audio_devices
  juce::juce_audio_formats
  juce::juce_audio_processors
  juce::juce_audio_utils
  juce::juce_data_structures
  PUBLIC
  juce::juce_recommended_config_flags
  juce::juce_recommended_lto_flags
  juce::juce_recommended_warning_flags)

#### END OF APP SECTION


#### START OF PLUGIN SECTION

# Feel free to delete this PLUGIN SECTION if you don't care to build plugins

# build VST3 and Unity for all platforms, add AU on MacOS
set(PLUGIN_FORMATS VST3 Unity)
if (CMAKE_SYSTEM_NAME STREQUAL Darwin)
  LIST(APPEND PLUGIN_FORMATS AU)
endif()

# if you are building a VST2 or AAX plugin, CMake needs to be told where to find these SDKs on your
# system. This setup should be done before calling `juce_add_plugin`.

# juce_set_vst2_sdk_path(...)
# juce_set_aax_sdk_path(...)

# `juce_add_plugin` adds a static library target with the name passed as the first argument
# (RNBOAudioPlugin here). This target is a normal CMake target, but has a lot of extra properties set
# up by default. As well as this shared code static library, this function adds targets for each of
# the formats specified by the FORMATS arguments. This function accepts many optional arguments.
# Check the readme at `docs/CMake API.md` in the JUCE repo for the full list.

juce_add_plugin(RNBOAudioPlugin
  # VERSION ...                        # Set this if the plugin version is different to the project version
  # ICON_BIG ...                       # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
  # ICON_SMALL ...
  COMPANY_NAME "Your Company Name"     # Specify the name of the plugin's author
  IS_SYNTH TRUE                        # Is this a synth or an effect?
  NEEDS_MIDI_INPUT TRUE                # Does the plugin need midi input?
  NEEDS_MIDI_OUTPUT TRUE               # Does the plugin need midi output?
  IS_MIDI_EFFECT FALSE                 # Is this plugin a MIDI effect?
  EDITOR_WANTS_KEYBOARD_FOCUS FALSE    # Does the editor need keyboard focus?
  COPY_PLUGIN_AFTER_BUILD FALSE        # Should the plugin be installed to a default location after building?
  PLUGIN_MANUFACTURER_CODE "Exmp"      # A four-character manufacturer id with at least one upper-case character
  PLUGIN_CODE "Rnb0"                   # A unique four-character plugin id with at least one upper-case character
  FORMATS ${PLUGIN_FORMATS}            # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
  PRODUCT_NAME "RNBO Plugin")          # The name of the final executable, which can differ from the target name

# `juce_generate_juce_header` will create a JuceHeader.h for a given target, which will be generated
# into your build tree. This should be included with `#include <JuceHeader.h>`. The include path for
# this header will be automatically added to the target. The main function of the JuceHeader is to
# include all your JUCE module headers; if you're happy to include module headers directly, you
# probably don't need to call this.

# the RNBO adapters currently need this
juce_generate_juce_header(RNBOAudioPlugin)

# `target_sources` adds source files to a target. We pass the target that needs the sources as the
# first argument, then a visibility parameter for the sources (PRIVATE is normally best practice,
# although it doesn't really affect executable targets). Finally, we supply a list of source files
# that will be built into the target. This is a standard CMake command.

target_sources(RNBOAudioPlugin PRIVATE
  "${RNBO_CPP_DIR}/adapters/juce/RNBO_JuceAudioProcessor.cpp"
  "${RNBO_CPP_DIR}/adapters/juce/RNBO_JuceAudioProcessorEditor.cpp"
  "${RNBO_CPP_DIR}/RNBO.cpp"
  ${RNBO_CLASS_FILE}
  )

include_directories(
  "${RNBO_CPP_DIR}/"
  "${RNBO_CPP_DIR}/common/"
  "${RNBO_CPP_DIR}/adapters/juce/"
  )

# `target_compile_definitions` adds some preprocessor definitions to our target. In a Projucer
# project, these might be passed in the 'Preprocessor Definitions' field. JUCE modules also make use
# of compile definitions to switch certain features on/off, so if there's a particular feature you
# need that's not on by default, check the module header for the correct flag to set here. These
# definitions will be visible both to your code, and also the JUCE module code, so for new
# definitions, pick unique names that are unlikely to collide! This is a standard CMake command.

target_compile_definitions(RNBOAudioPlugin
  PUBLIC
  # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
  JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
  JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
  JUCE_VST3_CAN_REPLACE_VST2=0
  )

# If your target needs extra binary assets, you can add them here. The first argument is the name of
# a new static library target that will include all the binary resources. There is an optional
# `NAMESPACE` argument that can specify the namespace of the generated binary data class. Finally,
# the SOURCES argument should be followed by a list of source files that should be built into the
# static library. These source files can be of any kind (wav data, images, fonts, icons etc.).
# Conversion to binary-data will happen when your target is built.

# juce_add_binary_data(AudioPluginData SOURCES ...)

# `target_link_libraries` links libraries and JUCE modules to other libraries or executables. Here,
# we're linking our executable target to the `juce::juce_audio_utils` module. Inter-module
# dependencies are resolved automatically, so `juce_core`, `juce_events` and so on will also be
# linked automatically. If we'd generated a binary data target above, we would need to link to it
# here too. This is a standard CMake command.

target_link_libraries(RNBOAudioPlugin
  PRIVATE
  # AudioPluginData           # If we'd created a binary data target, we'd link to it here
  juce::juce_audio_utils
  PUBLIC
  juce::juce_recommended_config_flags
  juce::juce_recommended_lto_flags
  juce::juce_recommended_warning_flags
  )

#TODO windows and linux
if(APPLE)
  install(
    TARGETS RNBOAudioPlugin_VST3
    DESTINATION ~/Library/Audio/Plug-Ins/VST3/
    )
  install(
    TARGETS RNBOAudioPlugin_AU
    DESTINATION ~/Library/Audio/Plug-Ins/Components/
    )
endif()

#### END OF PLUGIN SECTION
