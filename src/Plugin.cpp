#include <RNBO_JuceAudioProcessor.h>

//This creates new instances of your plugin, change RNBO::JuceAudioProcessor()
//to your own class if you create a derived plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new RNBO::JuceAudioProcessor();
}
