#include "JuceHeader.h"
#include "RNBO.h"
#include "RNBO_JuceAudioProcessor.h"

class CustomAudioEditor : public AudioProcessorEditor, private AudioProcessorListener
{
public:
    CustomAudioEditor(RNBO::JuceAudioProcessor* const p, RNBO::CoreObject& rnboObject);
    ~CustomAudioEditor() override;
    void paint (Graphics& g) override;

private:
    void audioProcessorChanged (AudioProcessor*, const ChangeDetails&) override { }
    void audioProcessorParameterChanged(AudioProcessor*, int parameterIndex, float) override;

protected:
    AudioProcessor                              *_audioProcessor;
    RNBO::CoreObject&                           _rnboObject; 
    Label                                       _label;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomAudioEditor)
};
