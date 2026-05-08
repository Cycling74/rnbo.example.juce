#pragma once

#include "JuceHeader.h"
#include "RNBO.h"
#include "RNBO_JuceAudioProcessor.h"

class CustomAudioEditor : public AudioProcessorEditor
{
public:
    CustomAudioEditor (RNBO::JuceAudioProcessor* const p, RNBO::CoreObject& rnboObject);
    ~CustomAudioEditor() override;
    void paint (Graphics& g) override;
    void resized() override;

private:
    RNBO::JuceAudioProcessor* _audioProcessor;
    RNBO::CoreObject&         _rnboObject;

    // Sliders must be declared before attachments; SliderParameterAttachment
    // registers with the slider on construction.
    Slider _kink1Slider, _kink2Slider, _kink3Slider;
    Label  _kink1Label,  _kink2Label,  _kink3Label;

    SliderParameterAttachment _kink1Attachment;
    SliderParameterAttachment _kink2Attachment;
    SliderParameterAttachment _kink3Attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomAudioEditor)
};
