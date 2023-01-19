#include "RNBO.h"
#include "RNBO_Utils.h"
#include "RNBO_JuceAudioProcessor.h"

class CustomAudioProcessor : public RNBO::JuceAudioProcessor {
public:
    CustomAudioProcessor();
    ~CustomAudioProcessor() override;
    AudioProcessorEditor* createEditor() override;
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomAudioProcessor)
};

