/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "JuceHeader.h"

#include "RNBO.h"
#include "RNBO_Utils.h"
#include "RNBO_JuceAudioProcessor.h"

#include "RNBO.cpp"

#include <array>

using namespace RNBO;

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/

class GrabFocusWhenShownComponentMovementWatcher : public ComponentMovementWatcher, public Timer
{
public:
	GrabFocusWhenShownComponentMovementWatcher(Component *componentToWatch)
	: ComponentMovementWatcher(componentToWatch)
	{
	}

	void componentMovedOrResized (bool wasMoved, bool wasResized) override
	{
		ignoreUnused(wasMoved);
		ignoreUnused(wasResized);
	}

	/** This callback happens when the component's top-level peer is changed. */
	void componentPeerChanged() override
	{
	}

	/** This callback happens when the component's visibility state changes, possibly due to
	 one of its parents being made visible or invisible.
	 */
	void componentVisibilityChanged() override
	{
		if (getComponent()->isShowing()) {
			startTimer(100);
		}
	}

	void timerCallback() override
	{
		if (!getComponent()->hasKeyboardFocus(true)) {
			getComponent()->grabKeyboardFocus();
		}
		else {
			stopTimer();
		}
	}
};

class MainContentComponent   : public Component, public RNBO::PatcherChangedHandler, public AsyncUpdater
{
public:

	void setMouseClickGrabsKeyboardFocusOnAllChildren(Component* c, bool way)
	{
		int numChildren = c->getNumChildComponents();
		for (int i=0; i < numChildren; i++) {
			Component *child = c->getChildComponent(i);
			child->setMouseClickGrabsKeyboardFocus(way);
			setMouseClickGrabsKeyboardFocusOnAllChildren(child, way);
		}
	}

	//==============================================================================
	MainContentComponent()
	: _midiKeyboardComponent(_midiKeyboardState, MidiKeyboardComponent::horizontalKeyboard)
    {
		const int channelCount = 2;
		_deviceManager.initialiseWithDefaultDevices(channelCount, channelCount);

		// setup our buffer size
		AudioDeviceManager::AudioDeviceSetup setup;
		_deviceManager.getAudioDeviceSetup(setup);
		//setup.bufferSize = 1024;
		setup.bufferSize = 128;
		//setup.bufferSize = 256;
		_deviceManager.setAudioDeviceSetup(setup, false);

		_deviceManager.addAudioCallback(&_audioProcessorPlayer);

#if 1
		// let's listen to all midi inputs
		// enable all midi inputs
		StringArray midiInputDevices = MidiInput::getDevices();
		for (const auto& input : midiInputDevices) {
			_deviceManager.setMidiInputEnabled(input, true);
		}
		_deviceManager.addMidiInputCallback("", &_audioProcessorPlayer);
#else
		// listen to the default MIDI input
		_deviceManager.setMidiInputEnabled(MidiInput::getDevices()[MidiInput::getDefaultDeviceIndex()], true);
		_deviceManager.addMidiInputCallback(MidiInput::getDevices()[MidiInput::getDefaultDeviceIndex()], &_audioProcessorPlayer);
#endif

		// setup the midi keyboard
		_midiKeyboardState.addListener(&_audioProcessorPlayer.getMidiMessageCollector());
		addAndMakeVisible(&_midiKeyboardComponent);

		_keyboardFocusGrabber = make_unique<GrabFocusWhenShownComponentMovementWatcher>(&_midiKeyboardComponent);

		loadRNBOAudioProcessor();

		setSize (800, 600);
    }

	void patcherChanged() override
	{
		// we can't unload in the middle of the notification
		triggerAsyncUpdate();
	}

	void handleAsyncUpdate() override
	{
		// reload the processor
		unloadRNBOAudioProcessor();
		loadRNBOAudioProcessor();
	}

	void loadRNBOAudioProcessor()
	{
		unloadRNBOAudioProcessor();

		jassert(_audioProcessor.get() == nullptr);

		_audioProcessor = make_unique<JuceAudioProcessor>();
		RNBO::CoreObject& rnboObject = _audioProcessor->getRnboObject();
		rnboObject.setPatcherChangedHandler(this);

		_audioProcessorPlayer.setProcessor(_audioProcessor.get());
		_audioProcessorEditor.reset(_audioProcessor->createEditorIfNeeded());
		if (_audioProcessorEditor) {
			addAndMakeVisible(_audioProcessorEditor.get());
			resized();  // set up the sizes

			// we don't want mouse clicks to move focus away from midi keyboard
			// so that we can use "midi keys" while using mouse on parameter sliders
			setMouseClickGrabsKeyboardFocusOnAllChildren(_audioProcessorEditor.get(), false);
		}
	}

	void unloadRNBOAudioProcessor()
	{
		if (_audioProcessor) {
			_audioProcessorPlayer.setProcessor(nullptr);
			if (_audioProcessorEditor) {
				_audioProcessor->editorBeingDeleted(_audioProcessorEditor.get());
			}
			_audioProcessorEditor.reset();
			_audioProcessor.reset();
		}
	}

	void shutdownAudio()
	{
		unloadRNBOAudioProcessor();
		_deviceManager.removeAudioCallback(&_audioProcessorPlayer);
		_deviceManager.closeAudioDevice();
	}

	~MainContentComponent()
    {
		shutdownAudio();
    }


    //=======================================================================
    void paint (Graphics& g) override
    {
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        g.fillAll (Colours::white);

        // You can add your drawing code here!
    }

    void resized() override
    {
		const int keysHeight = 60;
		if (_audioProcessorEditor) {
			_audioProcessorEditor->setBounds(0, 0, getWidth(), getHeight() - keysHeight);
		}
		_midiKeyboardComponent.setBounds(0, getHeight() - keysHeight, getWidth(), keysHeight);
    }


private:
    //==============================================================================

	AudioDeviceManager		_deviceManager;
	AudioProcessorPlayer	_audioProcessorPlayer;

	std::unique_ptr<GrabFocusWhenShownComponentMovementWatcher> _keyboardFocusGrabber;

	std::unique_ptr<JuceAudioProcessor>		_audioProcessor;
	std::unique_ptr<AudioProcessorEditor>	_audioProcessorEditor;

	// midi keyboard stuff
	MidiKeyboardState		_midiKeyboardState;
	MidiKeyboardComponent	_midiKeyboardComponent;

	std::array<float, 1024>	_testAudioBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED
