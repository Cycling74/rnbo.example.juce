#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "JuceHeader.h"
#include <json/json.hpp>

#include "RNBO.h"
#include "RNBO_Utils.h"
#include "CustomAudioProcessor.h"

#include <array>

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
	, _deviceSelectorComponent(_deviceManager,
		0,     // minimum input channels
		256,   // maximum input channels
		0,     // minimum output channels
		256,   // maximum output channels
		false, // ability to select midi inputs
		false, // ability to select midi output device
		false, // treat channels as stereo pairs
		false) // hide advanced options
    {
		loadRNBOAudioProcessor();

		RNBO::CoreObject& rnboObject = _audioProcessor->getRnboObject();

		_deviceManager.initialiseWithDefaultDevices(rnboObject.getNumInputChannels(), rnboObject.getNumOutputChannels());

		// setup our buffer size
		AudioDeviceManager::AudioDeviceSetup setup;
		_deviceManager.getAudioDeviceSetup(setup);
		setup.bufferSize = 128;
		_deviceManager.setAudioDeviceSetup(setup, false);

		_deviceManager.addAudioCallback(&_audioProcessorPlayer);

		// let's listen to all midi inputs
		// enable all midi inputs
		StringArray midiInputDevices = MidiInput::getDevices();
		for (const auto& input : midiInputDevices) {
			_deviceManager.setMidiInputEnabled(input, true);
		}
		_deviceManager.addMidiInputCallback("", &_audioProcessorPlayer);

		// setup the midi keyboard
		_midiKeyboardState.addListener(&_audioProcessorPlayer.getMidiMessageCollector());
		addAndMakeVisible(&_midiKeyboardComponent);

		// Only add the device selector if we're running as a standalone application
		if (JUCEApplicationBase::isStandaloneApp()) {
			addAndMakeVisible (_deviceSelectorComponent);
			_includesDeviceSelector = true;
		}

		_keyboardFocusGrabber = RNBO::make_unique<GrabFocusWhenShownComponentMovementWatcher>(&_midiKeyboardComponent);

		setSize (800, 494);
    }

	void patcherChanged() override
	{
		// we can't unload in the middle of the notification
		triggerAsyncUpdate();
	}

	void handleAsyncUpdate() override
	{
		// reload the processor
		loadRNBOAudioProcessor();
	}

	void loadRNBOAudioProcessor()
	{
		unloadRNBOAudioProcessor();

		jassert(_audioProcessor.get() == nullptr);

		_audioProcessor = std::unique_ptr<CustomAudioProcessor>(CustomAudioProcessor::CreateDefault());
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
        g.fillAll (JUCEApplicationBase::isStandaloneApp() ? Colours::darkgrey : Colours::white);

        // You can add your drawing code here!
    }

    void resized() override
    {
		const int keysHeight = 60;
		const int selectorWidth = 328;
		int usedSelectorWidth = 0;

		if (_includesDeviceSelector) {
			usedSelectorWidth = std::min(getWidth(), selectorWidth);
			_deviceSelectorComponent.setBounds(0, 0, usedSelectorWidth, getHeight());
		}

		if (_audioProcessorEditor) {
			_audioProcessorEditor->setBounds(usedSelectorWidth, 0, getWidth() - usedSelectorWidth, getHeight() - keysHeight);
		}
		_midiKeyboardComponent.setBounds(usedSelectorWidth, getHeight() - keysHeight, getWidth() - usedSelectorWidth, keysHeight);
    }

private:
    //==============================================================================

	AudioDeviceManager		_deviceManager;
	AudioProcessorPlayer	_audioProcessorPlayer;

	std::unique_ptr<GrabFocusWhenShownComponentMovementWatcher> _keyboardFocusGrabber;

	std::unique_ptr<CustomAudioProcessor>		_audioProcessor;
	std::unique_ptr<AudioProcessorEditor>		_audioProcessorEditor;

	// midi keyboard stuff
	MidiKeyboardState		_midiKeyboardState;
	MidiKeyboardComponent	_midiKeyboardComponent;

	// Audio device chooser
	AudioDeviceSelectorComponent _deviceSelectorComponent;
	bool _includesDeviceSelector = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED
