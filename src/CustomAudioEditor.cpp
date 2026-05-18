#include "CustomAudioEditor.h"

static juce::RangedAudioParameter&
findParameter(RNBO::JuceAudioProcessor* p, const juce::String& name)
{
    for (auto* param : p->getParameters())
    {
        if (param->getName(128) == name)
            return static_cast<juce::RangedAudioParameter&>(*param);
    }

    throw std::runtime_error("Parameter not found: " + name.toStdString());
}

CustomAudioEditor::CustomAudioEditor (RNBO::JuceAudioProcessor* const p,
                                      RNBO::CoreObject& rnboObject)
    : AudioProcessorEditor (p)
    , _audioProcessor (p)
    , _rnboObject (rnboObject)
    , _kink1Attachment(findParameter(p, "kink1"), _kink1Slider)
    , _kink2Attachment(findParameter(p, "kink2"), _kink2Slider)
    , _kink3Attachment(findParameter(p, "kink3"), _kink3Slider)
{
    for (auto* s : { &_kink1Slider, &_kink2Slider, &_kink3Slider })
    {
        s->setSliderStyle (Slider::LinearHorizontal);
        s->setTextBoxStyle (Slider::TextBoxRight, false, 60, 20);
        addAndMakeVisible (s);
    }

    auto setupLabel = [this] (Label& label, const char* text)
    {
        label.setText (text, dontSendNotification);
        label.setJustificationType (Justification::centredLeft);
        addAndMakeVisible (label);
    };

    setupLabel (_kink1Label, "Kink 1");
    setupLabel (_kink2Label, "Kink 2");
    setupLabel (_kink3Label, "Kink 3");

    setSize (400, 160);
}

CustomAudioEditor::~CustomAudioEditor() = default;

void CustomAudioEditor::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void CustomAudioEditor::resized()
{
    auto area = getLocalBounds().reduced (16);
    const int rowHeight = area.getHeight() / 3;

    auto layoutRow = [&] (Label& label, Slider& slider)
    {
        auto row = area.removeFromTop (rowHeight);
        label.setBounds (row.removeFromLeft (60));
        slider.setBounds (row);
    };

    layoutRow (_kink1Label, _kink1Slider);
    layoutRow (_kink2Label, _kink2Slider);
    layoutRow (_kink3Label, _kink3Slider);
}
