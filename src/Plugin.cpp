#include "CustomAudioProcessor.h"

//This creates new instances of your plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
  return CustomAudioProcessor::CreateDefault();
}
