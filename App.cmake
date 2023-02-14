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
  src/CustomAudioEditor.cpp
  src/CustomAudioProcessor.cpp

  ${RNBO_CLASS_FILE}

  ${RNBO_CPP_DIR}/RNBO.cpp
  ${RNBO_CPP_DIR}/adapters/juce/RNBO_JuceAudioProcessorUtils.cpp
  ${RNBO_CPP_DIR}/adapters/juce/RNBO_JuceAudioProcessorEditor.cpp
  ${RNBO_CPP_DIR}/adapters/juce/RNBO_JuceAudioProcessor.cpp
  )

if (EXISTS ${RNBO_BINARY_DATA_FILE})
  target_sources(RNBOApp PRIVATE ${RNBO_BINARY_DATA_FILE})
endif()

target_include_directories(RNBOApp
  PRIVATE
  ${RNBO_CPP_DIR}/
  ${RNBO_CPP_DIR}/src
  ${RNBO_CPP_DIR}/common/
  ${RNBO_CPP_DIR}/adapters/juce/
  ${RNBO_CPP_DIR}/src/3rdparty/
  src
)

target_compile_definitions(RNBOApp
  PRIVATE
  # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
  JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_gui_app` call
  JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_gui_app` call
  JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:RNBOApp,JUCE_PRODUCT_NAME>"
  JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:RNBOApp,JUCE_VERSION>")

# `target_link_libraries` links libraries and JUCE modules to other libraries or executables. Here,
# we're linking our executable target to the `juce::juce_gui_extra` module. Inter-module
# dependencies are resolved automatically, so `juce_core`, `juce_events` and so on will also be
# linked automatically. If we'd generated a binary data target above, we would need to link to it
# here too. This is a standard CMake command.
target_link_libraries(RNBOApp
  PRIVATE
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

