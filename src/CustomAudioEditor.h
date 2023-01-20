#include "JuceHeader.h"
#include "RNBO.h"
#include "RNBO_JuceAudioProcessor.h"

class CustomAudioEditor : public AudioProcessorEditor, public RNBO::EventHandler, private AudioProcessorListener
{
public:
    CustomAudioEditor(RNBO::JuceAudioProcessor* const p, RNBO::CoreObject& rnboObject);
    void paint (Graphics& g) override;
    void eventsAvailable() override {}
    void handleParameterEvent(const RNBO::ParameterEvent&) override {}

private:
    void audioProcessorChanged (AudioProcessor*, const ChangeDetails&) override { }
    void audioProcessorParameterChanged(AudioProcessor*, int parameterIndex, float) override;

protected:
    RNBO::CoreObject&                           _rnboObject; 
    RNBO::ParameterEventInterfaceUniquePtr      _parameterInterface; 
    Label                                       _label;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomAudioEditor)
};
