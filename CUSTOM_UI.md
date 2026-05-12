# Making a Custom UI

RNBO provides a default interface for audio plugins. This simple interface creates a slider for each parameter in your RNBO patch. Through JUCE, you can create a totally custom interface, complete with a unique look and feel, and following complex logic.

This guide won't really talk about user interface best practices, but it will show you how to set up a project with a custom UI. We'll look the two main approaches that you can use: building a native interface using the JUCE library in C++, or creating a web browser interface that binds to your audio processor.

Each approach has its own advantages. Building a native interface with JUCE can be more efficient, as it doesn't load a separate web browser component. However, using JUCE's WebBrowserComponent lets you work with HTML/CSS/JS to build your interface, which can be much easier. If you run a local web server, you can also modify your interface while your plugin is running using hot reloading.

For most people, we recommend starting with the web-based approach.

Please note, if you haven't yet followed the setup steps in `README.md`, you should do so before you follow this tutorial.

## Switching UI

Take a look at `CMakeLists.txt`. This is the main build configuration file for this project. It defines what files should be compiled together to produce a VST/AU plugin, as well as a standalone application.

If you look through that file, you will see a line like this:

```
set(RNBO_EDITOR_MODE "DEFAULT" CACHE STRING "UI editor: DEFAULT (Generic JUCE controls), NATIVE (Custom JUCE controls), WEBVIEW (WebBrowserComponent)")
```

This defines a configuration variable called `RNBO_EDITOR_MODE`, which can be set to `DEFAULT`, `NATIVE`, or `WEBVIEW`. 

| Value | Interface |
| - | - |
| DEFAULT | Creates a generic audio parameter editor by calling `RNBO::JuceAudioProcessor::createEditor()`. No customization is possible. |
| NATIVE | Uses the JUCE library to create a native interface in C++ |
| WEBVIEW | Uses WebBrowserComponent to create a web-based interface using browser technology |

In order to set this configuration variable, use `-DRNBO_EDITOR_MODE` during CMake configuration. For example:

```
cmake .. -DRNBO_EDITOR_MODE=WEBVIEW -G Ninja
```

This command will configure Ninja-based build, using a WebBrowserComponent to support a custom UI.

## Exporting the example patcher

For this example, we're going to create a custom user interface for a simple drone patcher, which you can export into `/export` as per the instructions over in `README.md`. These instructions in this document should work for both a plugin as well as a desktop app. 

The `three-param-kink.maxpat` patcher will produce a drone with adjustable timbre. You can find it in `patches`, and it looks like this:

![](./img/droner.png)

Before you continue, make sure that you've exported this patcher into the _export/_ directory. 

> You'll notice that the example patcher will have the name `three-param-kink.cpp`. When you configure CMake, be sure to set `RNBO_CLASS_FILE_NAME` to `three-param-kink.cpp`, so that the build script can find the exported RNBO file.

## Creating a Web-based UI

The easiest and fastest way to build a custom UI is by using WebBrowserComponent, which will let you design and style your UI using HTML/CSS/JavaScript. To start, open your terminal and configure CMake to use the web-based editor.

```
cd build
cmake .. -DRNBO_CLASS_FILE_NAME three-param-kink.cpp -DRNBO_EDITOR_MODE=WEBVIEW
```

If you look in `src/CustomAudioProcessor.cpp`, you'll see code like this:

```cpp
AudioProcessorEditor* CustomAudioProcessor::createEditor()
{
#if defined(RNBO_EDITOR_NATIVE)
    return new CustomAudioEditor (this, this->_rnboObject);
#elif defined(RNBO_EDITOR_WEBVIEW)
    return new WebBrowserAudioEditor (this, this->_rnboObject);
#else
    return RNBO::JuceAudioProcessor::createEditor();
#endif
}
```

As you can see, when the `RNBO_EDITOR_MODE` is set to `WEBVIEW`, the plugin will load `WebBrowserAudioEditor`. 

### Customizing the web UI with hot reloading

This example comes with a very simple web UI already in place. One thing that's really useful about using a WebBrowserComponent to serve our UI is that we can update our UI while the application is still running.

The `WebBrowserAudioEditor` class is responsble for providing resources to the WebBrowserComponent, like HTML files, images, and JavaScript files. At the top of "WebBrowserAudioEditor.cpp", you'll see code like this:

```cpp
#if JUCE_ANDROID
static const juce::String kDevServerAddress = "http://10.0.2.2:3000/";
#else
static const juce::String kDevServerAddress = "http://localhost:3000/";
#endif
```

That means that if we run a development server on port 3000, then the web interface will load files from that server. Open your terminal and navigate to `src/webui/`.

```
cd src/webui
```

Now, run a local web server. If you have `npx` installed, you can use `live-server`, which supports hot reloading.

```
npx live-server --port=3000
```

When you run the server, your default web browser should open automatically, and you should see something like this:

![](./img/001-kink-synth-ui.png)

Close the web browser. From your terminal, build the app and plugin.

```
cd build
cmake --build .
```

This should build a JUCE standalone app to `build/RNBOApp_artefacts/Release/RNBO App Example.app` (or to `build/RNBOApp_artefacts/Debug/RNBO App Example.app` if you're using a debug configuration). After the build script finishes running, open the app.

### Binding the interface to the engine

The `WebBrowserAudioEditor` class takes care of loading a 

### Adding a control

## Creating a Native UI

Let's make a simple interface for this patcher with three JUCE sliders. First, download the Projucer if you haven’t already. The Projucer is part of JUCE, but the JUCE Github repository does not contain a Projucer executable. To get the Projucer, instead download a JUCE installer for your platform from the JUCE website.

Create a new project, selecting the Basic plug-in project template. Use the defaults for modules and exporters (we won't be using these anyway). Now we need to decide where to save the `.jucer` file. We're not really going to be using this file too much, so it might be nice to keep it isolated from the rest of our code. I'm going to make a new folder in the root of the repository called `ui`, and I'll save the JUCE project there. After creating the project, your directory structure should look something like this:

![](./img/directory_structure.png)

The Projucer will automatically create four files for the PluginProcessor and PluginEditor. We won't be using these, so you can just delete them.

By default the GUI editor is not enabled. You may need to enable it from the Tools menu.

![](./img/tools_menu.png)

From the "GUI Editor" menu, select "Add new GUI Component" to add a `.cpp` and `.h` file for your new component. I named mine `RootComponent` because it's hard to come up with a good, original name. You can call yours whatever you want. Now let's add three sliders to our component. Navigate to "Subcomponents" and right-click to add these sliders. Let's also be careful to change the name of each component. Our three parameters are called `kink1`, `kink2`, and `kink3`, so give the sliders each one of these names. Later on, we'll use this name to map each slider to the RNBO parameter with the same name.

When you are done, "Save All."

![](./img/named_slider.png)

Now we have two main tasks ahead of us.

1. Replace the default RNBO plugin UI with our custom interface.
2. Connect our sliders to the RNBO parameters.

### Adding the Interface to CMake

First, we need to make sure that the RootComponent.cpp and RootComponent.h files get added to our project. First, add these files to `Plugin.cmake` in the repository root.

```cmake
target_sources(RNBOAudioPlugin PRIVATE
  "${RNBO_CPP_DIR}/adapters/juce/RNBO_JuceAudioProcessor.cpp"
  "${RNBO_CPP_DIR}/adapters/juce/RNBO_JuceAudioProcessorEditor.cpp"
  "${RNBO_CPP_DIR}/RNBO.cpp"
  ${RNBO_CLASS_FILE}
  src/Plugin.cpp
  src/CustomAudioEditor.cpp
  src/CustomAudioProcessor.cpp
  ui/NewProject/Source/RootComponent.cpp
  )

include_directories(
  "${RNBO_CPP_DIR}/"
  "${RNBO_CPP_DIR}/common/"
  "${RNBO_CPP_DIR}/adapters/juce/"
  "${RNBO_CPP_DIR}/src/3rdparty/"
  "src"
  "ui/NewProject/Source"
  )
```

Next, find `App.cmake` and add these files there as well.

```cmake
target_sources(RNBOApp
  PRIVATE
  src/Main.cpp
  src/MainComponent.cpp
  src/CustomAudioEditor.cpp
  src/CustomAudioProcessor.cpp
  ui/NewProject/Source/RootComponent.cpp

  ${RNBO_CLASS_FILE}

  ${RNBO_CPP_DIR}/RNBO.cpp
  ${RNBO_CPP_DIR}/adapters/juce/RNBO_JuceAudioProcessorUtils.cpp
  ${RNBO_CPP_DIR}/adapters/juce/RNBO_JuceAudioProcessorEditor.cpp
  ${RNBO_CPP_DIR}/adapters/juce/RNBO_JuceAudioProcessor.cpp
  )

include_directories(
  "src"
  "${RNBO_CPP_DIR}/"
  "${RNBO_CPP_DIR}/src"
  "${RNBO_CPP_DIR}/common/"
  "${RNBO_CPP_DIR}/adapters/juce/"
  "${RNBO_CPP_DIR}/src/3rdparty/"
  "ui/NewProject/Source"
  )
```

Now use CMake in the usual way to generate and build. Remember to set the correct name for `RNBO_CLASS_FILE` on line 17 of `CMakeLists.txt`, for example, `three-param-kink.cpp`. Once you've done so, you can enter something like this into your terminal:

```sh
cd build
cmake -G Ninja ..
cmake --build .
```

The plugin should build without errors, but of course we don't see our new `RootComponent` with its sliders yet. We need to add the `RootComponent` to our custom UI.

### Adding the Custom Root Component
Open up `src/CustomAudioEditor.h`. First, add `RootComponent.h` to the include definitions.

```cpp
#include "JuceHeader.h"
#include "RNBO.h"
#include "RNBO_JuceAudioProcessor.h"
#include "RootComponent.h"
```

Next, find the declaration for the default `_label` member variable and replace it with one for a `RootComponent` component.

```cpp
// Label                _label;
RootComponent           _rootComponent;
```

Finally, open up `src/CustomAudioEditor.cpp`. Find the constructor, where the default label is configured and sized. Replace that code with new code to size and configure the `RootComponent`.

```cpp
CustomAudioEditor::CustomAudioEditor (RNBO::JuceAudioProcessor* const p, RNBO::CoreObject& rnboObject)
    : AudioProcessorEditor (p)
    , _rnboObject(rnboObject)
    , _audioProcessor(p)
{
    _audioProcessor->AudioProcessor::addListener(this);

    // _label.setText("Hi I'm Custom Interface", NotificationType::dontSendNotification);
    // _label.setBounds(0, 0, 400, 300);
    // _label.setColour(Label::textColourId, Colours::black);
    // addAndMakeVisible(_label);
    // setSize (_label.getWidth(), _label.getHeight());

    addAndMakeVisible(_rootComponent);
    setSize(_rootComponent.getWidth(), _rootComponent.getHeight());
}
```

Rebuild using CMake, and you should see the generated UI loading in place of the default custom UI.

```sh
cd build
cmake ..
cmake --build .
```

### Connecting the Sliders

To make the sliders functional, we modify `RootComponent.h` and `RootComponent.cpp`. When the sliders change, we want to update the parameters of the `AudioProcessor`. When we get a parameter change notification from the `AudioProcessor`, we want to update the sliders.

Open up `RootComponent.h`. At the top of the file, include these RNBO header files.

```cpp
//[Headers]     -- You can add your own extra header files here --
#include <JuceHeader.h>
#include "RNBO.h"
#include "RNBO_JuceAudioProcessor.h"
//[/Headers]
```

Now add the following between the `[UserMethods]` tags:

```cpp
//[UserMethods]     -- You can add your own custom methods in this section.
void setAudioProcessor(RNBO::JuceAudioProcessor *p);
void updateSliderForParam(unsigned long index, double value);
//[/UserMethods]
```

Also add the following private instance variables
```cpp
//[UserVariables]   -- You can add your own custom variables in this section.
RNBO::JuceAudioProcessor *processor = nullptr;
HashMap<int, Slider *> slidersByParameterIndex; // used to map parameter index to slider we want to control
//[/UserVariables]
```

We'll need to call `setAudioProcessor` from the `CustomAudioEditor`. Open `CustomAudioEditor.cpp` and add the following line:

```cpp
_rootComponent.setAudioProcessor(p); // <--- add this line
addAndMakeVisible(_rootComponent);
setSize(_rootComponent.getWidth(), _rootComponent.getHeight());
```

Now let's implement `setAudioProcessor`. Open up `RootComponent.cpp` and add the following after `[MiscUserCode]`.

```cpp
//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void RootComponent::setAudioProcessor(RNBO::JuceAudioProcessor *p)
{
    processor = p;

    RNBO::ParameterInfo parameterInfo;
    RNBO::CoreObject& coreObject = processor->getRnboObject();

    for (unsigned long i = 0; i < coreObject.getNumParameters(); i++) {
        auto parameterName = coreObject.getParameterId(i);
        RNBO::ParameterValue value = coreObject.getParameterValue(i);
        Slider *slider = nullptr;
        if (juce::String(parameterName) == juce__slider.get()->getName()) {
            slider = juce__slider.get();
        } else if (juce::String(parameterName) == juce__slider2.get()->getName()) {
            slider = juce__slider2.get();
        } else if (juce::String(parameterName) == juce__slider3.get()->getName()) {
            slider = juce__slider3.get();
        }

        if (slider) {
            slidersByParameterIndex.set(i, slider);
            coreObject.getParameterInfo(i, &parameterInfo);
            slider->setRange(parameterInfo.min, parameterInfo.max);
            slider->setValue(value);
        }
    }
}
//[/MiscUserCode]
```

Notice how we use the name of the slider to map the slider to a parameter with the matching ID. Now, in `RootComponent.cpp` find the function called `sliderValueChanged` and update it as follows:

```cpp
void RootComponent::sliderValueChanged (juce::Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    if (processor == nullptr) return;
    RNBO::CoreObject& coreObject = processor->getRnboObject();
    auto parameters = processor->getParameters();
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == juce__slider.get())
    {
        //[UserSliderCode_juce__slider] -- add your slider handling code here..
        //[/UserSliderCode_juce__slider]
    }
    else if (sliderThatWasMoved == juce__slider2.get())
    {
        //[UserSliderCode_juce__slider2] -- add your slider handling code here..
        //[/UserSliderCode_juce__slider2]
    }
    else if (sliderThatWasMoved == juce__slider3.get())
    {
        //[UserSliderCode_juce__slider3] -- add your slider handling code here..
        //[/UserSliderCode_juce__slider3]
    }

    //[UsersliderValueChanged_Post]
    RNBO::ParameterIndex index = coreObject.getParameterIndexForID(sliderThatWasMoved->getName().toRawUTF8());
    if (index != -1) {
        const auto param = processor->getParameters()[index];
        auto newVal = sliderThatWasMoved->getValue();

        if (param && param->getValue() != newVal)
        {
            auto normalizedValue = coreObject.convertToNormalizedParameterValue(index, newVal);
            param->beginChangeGesture();
            param->setValueNotifyingHost(normalizedValue);
            param->endChangeGesture();
        }
    }
    //[/UsersliderValueChanged_Post]
}
```

This is all we need to control the RNBO patch using the sliders in our custom UI. However, to be really complete, we should also make sure that the sliders will update if RNBO changes the value of a parameter internally. Open `CustomAudioEditor.cpp` and add the following to `audioProcessorParameterChanged`.

```cpp
void CustomAudioEditor::audioProcessorParameterChanged (AudioProcessor*, int parameterIndex, float value)
{
    _rootComponent.updateSliderForParam(parameterIndex, value);
}
```

Now open `RootComponent.cpp` and implement `updateSliderForParam` inside of the `[MiscUserCode]` tags.

```cpp
void RootComponent::updateSliderForParam(unsigned long index, double value)
{
    if (processor == nullptr) return;
    RNBO::CoreObject& coreObject = processor->getRnboObject();
    auto denormalizedValue = coreObject.convertFromNormalizedParameterValue(index, value);
    auto slider = slidersByParameterIndex.getReference((int) index);
    if (slider && (slider->getThumbBeingDragged() == -1)) {
        slider->setValue(denormalizedValue, NotificationType::dontSendNotification);
    }
}
```

That's it. Compile and build. You may need to restart Max, or whatever DAW you're using, in order to see changes to your plugin.

## Epilogue

Obviously this has just scratched the surface of what's possible with custom C++ interfaces. If you want to read more, a great place to get started would be https://www.theaudioprogrammer.com/. In particular, they have a #design-ux-and-ui channel in their Discord that is full of helpful and supportive people. Best of luck and have fun building your custom UI.
