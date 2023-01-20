#include "CustomAudioProcessor.h"
#include "CustomAudioEditor.h"

CustomAudioProcessor::CustomAudioProcessor() : RNBO::JuceAudioProcessor() {}

AudioProcessorEditor* CustomAudioProcessor::createEditor()
{
    return new CustomAudioEditor (this, this->_rnboObject);
}
