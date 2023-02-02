#include "RNBO.h"
#include "RNBO_Utils.h"
#include "RNBO_JuceAudioProcessor.h"
#include <json/json.hpp>

class CustomAudioProcessor : public RNBO::JuceAudioProcessor {
public:
    CustomAudioProcessor(const nlohmann::json& patcher_desc, const nlohmann::json& presets);
    juce::AudioProcessorEditor* createEditor() override;
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomAudioProcessor)
};

