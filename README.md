# RNBO JUCE Examples

So you want to build your own DAW or a Plugin? This template should get you started with your own Standalone Desktop application and Audio Plugin, using the source code export feature of RNBO, part of [Max 8](https://cycling74.com/max8/) made by [Cycling '74](https://cycling74.com/). 

This project is based on the cross-platform JUCE framework for handling audio processing. You have the option of using JUCE to manage your UI as well. Please be aware that the JUCE has its own license terms (mostly GPL with the availability of commercial licenses). See their [website](http://www.juce.com/) for further details.

## Prerequisites

- Download and install [CMake](https://cmake.org/download/). Version 3.18 or higher is required. On MacOS, we recommend installing CMake with [Homebrew](https://brew.sh/)
- Download and install [git](https://git-scm.com/downloads).
- Some kind of build system and compiler. You have options here.
 - (MacOS) Install Xcode command line tools by running `sudo xcode-select --install` on the command line. You'll use `make` to compile your application. (You will not be able to use Xcode to build your application unless you install Xcode itself.)
 - (MacOS/Linux/Unix-like) Install [Ninja](https://github.com/ninja-build/ninja/releases), easiest way is probably `brew install ninja` or `sudo apt-get install ninja`
 - (MacOS) Download and install [Xcode](https://developer.apple.com/xcode/resources/). We have tested using Xcode 12.
 - (Windows) Download and install [Visual Studio 2019](https://visualstudio.microsoft.com/vs/). Community Edition is enough!
 - (Linux/Unix-like) use `make`, often already on your system. For debian based systems `sudo apt-get install build-essential`

## File structure

The source code of the application is in the `src/` directory. This directory should contain everything that you need to modify to build your application.

Some notable files/directories:

| Location                          | Explanation   |
| --------------------------------- | ------------- |
| export/                           | The directory into which you should export your RNBO code |
| src/                              | Source for the project - feel free to edit (includes sample UI) |
| build/RNBOApp_artefacts/          | Your built application will end up here |
| build/RNBOAudioPlugin_artefacts/  | Your built plugins will end up here |

## Using this Template

This Github repo is a template, which means you can use it to start your own git-based project using this repository as a starting point. The major difference between a template and a fork is that your new project won't include the commit history of this template--it will be an entirely new starting point. For more see [the official description](https://docs.github.com/en/repositories/creating-and-managing-repositories/creating-a-repository-from-a-template).

### Getting Started

To get started, first create a new repository to hold your project using this repository as a template. If you're viewing this repo on Github, you should see a button at the top of the page that says `Use this template`. 

![Use this template button](./img/use-this-template-button.png)

You can also follow [the official steps](https://docs.github.com/en/repositories/creating-and-managing-repositories/creating-a-repository-from-a-template) on Github for creating a new repository from a template.

Now you need to copy this repository locally. Follow [the official steps](https://docs.github.com/en/repositories/creating-and-managing-repositories/cloning-a-repository) to clone your repository. Once you've cloned your repository locally, you'll need to initialize the JUCE submodule.

```
cd your-project-folder
git submodule update --init --recursive --progress
```

If the above command doesn't work, check your version if git by runing `git --version`. The `--progress` flag wasn't introduced until git `2.11.0`, so if your version is earlier than this you won't have access to it. Strictly speaking you don't need that last `--progress` flag, but it's nice to have some progress indication, especially since installing the JUCE submodule can take a while. That's all you'll need to do to get set up! Now you can start exporting from RNBO and building your project.

### Working with RNBO and Building Your Project

Next, open the RNBO patcher you'd like to work with, and navigate to the export sidebar. Find "C++ Source Code Export" target.

![C++ source code export in the sidebar](./img/cpp-export-location.png)

Export your project, making sure to export into the `export` folder in this directory. Your export directory should look something like this:

```
export/
├─ rnbo/
├─ rnbo_source.cpp
├─ README.md
```

Whenever you make a change to your RNBO patch, remember to export the source code again to update this file. Now that you've exported your RNBO code, it's time to build. This project uses CMake, which gives us the flexibility of using whatever build system we want. Start by moving to the build directory.

```sh
cd build
```

Now you have a choice of what build system you want to use. Any one of the following will work:

- `cmake .. -G Xcode` (create an Xcode project)
- `cmake .. -G "Visual Studio 16"` (create a Visual Studio 2019 project)
- `cmake .. -G Ninja` (use Ninja to build)
- `cmake ..` (just use the default, which will be `make` on MacOS, Linux and other Unix-like platforms)

You might be wondering which on is "best". We say, if you're familiar with Xcode or Visual Studio or Ninja, just go with that. This might be a good time to get a snack, as CMake can take a few minutes to get everything ready, especially when generating the build files for the first time. You may also see a number of warnings in the console, which you can (probably) safely ignore.

Once CMake has finished generating your build system, you can finally build your project.

```
cmake --build .
```

Invoking `cmake` with the `--build` flag tells CMake to build your target, using whatever build tool you chose in the last step. After the build completes, you'll find the executable result in `build/RNBOApp_artefacts/Debug`, and you'll find plugins in `build/RNBOAudioPlugin_artefacts/Debug`.

If you're using the Xcode generator, but you don't have Xcode installed, you might see something like this when you try to build
```sh
% cmake --build .
xcode-select: error: tool 'xcodebuild' requires Xcode, but active developer directory '/Library/Developer/CommandLineTools' is a command line tools instance
```

This simply means that you need to install Xcode, and not just the command line tools.

## Additional Notes and Troubleshooting

### Building Plugins on M1 Macs
When building for M1 Macs, you will want to enable universal builds, so that your target can be used on both Intel and M1 macs. `CMakeLists.txt` has a line you can uncomment to enable universal builds.

### Help! My DAW Won't Load My Plugin
After building your plugin, you may find that it loads in some DAWs but not others. On MacOS, the problem is sometimes code signing. JUCE may incorrectly code sign your VST3 bundle. If you use the `codesign` tool to verify your VST3 bundle

```
codesign --verify --verbose RNBOAudioPlugin_artefacts/Release/VST3/MyPlugin.vst3
```

and you see an error like this:

```
RNBOAudioPlugin_artefacts/Release/VST3/MyPlugin.vst3: code has no resources but signature indicates they must be present
```

you're seeing the issue. Fortunately, you can give the plugin a new, ad-hoc code signature with the following command

```
codesign --force --deep -s - RNBOAudioPlugin_artefacts/Release/VST3/MyPlugin.vst3
```

You'll see a message like:

```
RNBOAudioPlugin_artefacts/Release/VST3/MyPlugin.vst3: replacing existing signature
```

Hopefully, this will resolve the issue.

### MIDI CC and VST3
VST3 introduced some changes to the way plugins handle MIDI data. One way to make newer VST3 plugins behave more like VST2 is to create Parameters for each MIDI CC value on each MIDI channel. You can dip your toes into the [full discussion](https://forums.steinberg.net/t/vst3-and-midi-cc-pitfall/201879/11) if you want, but we disable this behavior by default. If you really want it, you can enable it by commenting out the appropriate line in `CMakeLists.txt`.

### Working with your RNBO Plugin in Unity
You can build a dedicated audio plugin for Unity using our [RNBO Unity Audio Plugin repository](https://github.com/Cycling74/rnbo.unity.audioplugin), which also provides an API that facilitates working with your RNBO export in your C# scripting. Check out that repository for more information.

## Customizing the Project

This project is based on the [JUCE Framework](http://www.juce.com/). Please refer to tutorials from JUCE on building UIs, for instance.

There are details that you might want to change in `App.cmake` for Applications and in `Plugin.cmake` for Plugins.

If you're not interested in the Application or Plugin parts of this project you can remove the associated *include* lines from the `CMakeLists.txt` file.
