# build VST3 for all platforms, add AU on MacOS
set(PLUGIN_FORMATS VST3)
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
  src/Plugin.cpp
  src/CustomAudioEditor.cpp
  src/CustomAudioProcessor.cpp
  )

if (EXISTS ${RNBO_BINARY_DATA_FILE})
  target_sources(RNBOAudioPlugin PRIVATE ${RNBO_BINARY_DATA_FILE})
endif()

target_include_directories(RNBOAudioPlugin
  PRIVATE
  ${RNBO_CPP_DIR}/
  ${RNBO_CPP_DIR}/src
  ${RNBO_CPP_DIR}/common/
  ${RNBO_CPP_DIR}/adapters/juce/
  ${RNBO_CPP_DIR}/src/3rdparty/
  src
)

# `target_compile_definitions` adds some preprocessor definitions to our target. In a Projucer
# project, these might be passed in the 'Preprocessor Definitions' field. JUCE modules also make use
# of compile definitions to switch certain features on/off, so if there's a particular feature you
# need that's not on by default, check the module header for the correct flag to set here. These
# definitions will be visible both to your code, and also the JUCE module code, so for new
# definitions, pick unique names that are unlikely to collide! This is a standard CMake command.

set(RNBO_JUCE_PARAM_DEFAULT_NOTIFY 1)
if (NOT PLUGIN_PARAM_DEFAULT_NOTIFY)
	set(RNBO_JUCE_PARAM_DEFAULT_NOTIFY 0)
endif()

target_compile_definitions(RNBOAudioPlugin
  PUBLIC
  # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
  JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
  JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
  JUCE_VST3_CAN_REPLACE_VST2=0
  RNBO_JUCE_NO_CREATE_PLUGIN_FILTER=1 #don't have RNBO create its own createPluginFilter function, we'll create it ourselves
  RNBO_JUCE_PARAM_DEFAULT_NOTIFY=${RNBO_JUCE_PARAM_DEFAULT_NOTIFY}
  )

# `target_link_libraries` links libraries and JUCE modules to other libraries or executables. Here,
# we're linking our executable target to the `juce::juce_audio_utils` module. Inter-module
# dependencies are resolved automatically, so `juce_core`, `juce_events` and so on will also be
# linked automatically. If we'd generated a binary data target above, we would need to link to it
# here too. This is a standard CMake command.

target_link_libraries(RNBOAudioPlugin
  PRIVATE
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
