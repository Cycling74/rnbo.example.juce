#include "CustomAudioProcessor.h"
#include "CustomAudioEditor.h"

CustomAudioProcessor::CustomAudioProcessor(const nlohmann::json& patcher_desc, const nlohmann::json& presets) : RNBO::JuceAudioProcessor(patcher_desc, presets) {}

AudioProcessorEditor* CustomAudioProcessor::createEditor()
{
    return new CustomAudioEditor (this, this->_rnboObject);
}
