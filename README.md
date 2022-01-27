# RNBO JUCE Examples

So you want to build your own DAW or a Plugin? This template should get you started with your own Standalone Desktop application and Audio Plugin, using the source code export feature of RNBO, part of [Max 8](https://cycling74.com/max8/) made by [Cycling '74](https://cycling74.com/). 

This project is based on the cross-platform JUCE framework for handling audio processing. You have the option of using JUCE to manage your UI as well. Please be aware that the JUCE has its own license terms (mostly GPL with the availability of commercial licenses). See their [website](http://www.juce.com/) for further details.

## Prerequisites

- Download and install [CMake](https://cmake.org/download/). Version 3.18 or higher is required.
- Download and install [git](https://git-scm.com/downloads).
- Some kind of build system and compiler. You have options here.
 - (MacOS) Install Xcode command line tools by running `sudo xcode-select --install` on the command line. You'll use `make` to compile your application.
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

Strictly speaking you don't need that last `--progress` flag, but it's nice to have some progress indication. That's all you'll need to do to get set up! Now you can start exporting from RNBO and building your project.

### Working with RNBO and Building Your Project

Next, open the RNBO patcher you'd like to work with, and navigate to the export sidebar. Find "C++ Source Code Export" target.

![C++ source code export in the sidebar](./img/cpp-export-location.png)

Export your project, making sure to export into the `export` folder in this directory. Your export directory should look something like this:

```
export/
├─ code/
|  ├─ rnbo/
|  ├─ rnbo_source.cpp
├─ README.md
```

Whenever you make a change to your RNBO patch, remember to export the source code again to update this file. Now that you've exported your RNBO code, it's time to build. This project uses CMake, which gives us the flexibility of using whatever build system we want. Start by moving to the build directory.

```
cd build
```

Now you have a choice of what build system you want to use. Any one of the following will work

- `cmake .. -G Xcode` (create an Xcode project)
- `cmake .. -G "Visual Studio 16"` (create a Visual Studio 2019 project)
- `cmake .. -G Ninja` (use Ninja to build)
- `cmake ..` (just use the default, which will be `make` on MacOS, Linux and other Unix-like platforms)

Finally, build your project.

```
cmake --build .
```

You'll find the executable result in `build/RNBOApp_artefacts/Debug`
You'll find plugins in `build/RNBOAudioPlugin_artefacts/Debug`

## Customizing the Project

This project is based on the [JUCE Framework](http://www.juce.com/). Please refer to tutorials from JUCE on building UIs, for instance.

There are details that you might want to change in `App.cmake` for Applications and in `Plugin.cmake` for Plugins.

If you're not interested in the Application or Plugin parts of this project you can remove the associated *include* lines from the `CMakeLists.txt` file.
