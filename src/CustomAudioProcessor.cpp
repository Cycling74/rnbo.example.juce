#include "CustomAudioProcessor.h"

AudioProcessorEditor* createCustomEditor(AudioProcessor * const &p, RNBO::CoreObject &c);

CustomAudioProcessor::CustomAudioProcessor() {}
CustomAudioProcessor::~CustomAudioProcessor() {}

AudioProcessorEditor* CustomAudioProcessor::createEditor()
{
    return createCustomEditor(this, this->_rnboObject);
}
