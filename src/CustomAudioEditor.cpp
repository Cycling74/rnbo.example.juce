#include "CustomAudioEditor.h"

CustomAudioEditor::CustomAudioEditor (RNBO::JuceAudioProcessor* const p, RNBO::CoreObject& rnboObject)
    : AudioProcessorEditor (p)
    , _rnboObject(rnboObject)
    , _parameterInterface(_rnboObject.createParameterInterface(RNBO::ParameterEventInterface::SingleProducer, this))
{
    p->AudioProcessor::addListener(this);

    _label.setText("Hi I'm Custom Interface", NotificationType::dontSendNotification);
    _label.setBounds(0, 0, 400, 300);
    _label.setColour(Label::textColourId, Colours::black);
    addAndMakeVisible(_label);
    setSize (_label.getWidth(), _label.getHeight());
}

void CustomAudioEditor::paint (Graphics& g)
{
    g.fillAll(Colours::white);
}

void CustomAudioEditor::audioProcessorParameterChanged (AudioProcessor*, int parameterIndex, float value)
{
    // Handle parameter changes here
}
