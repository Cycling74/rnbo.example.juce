#include <RNBO_JuceAudioProcessor.h>
#include <json/json.hpp>
// #include "CustomAudioProcessor.h"

#ifdef RNBO_INCLUDE_DESCRIPTION_FILE
#include <rnbo_description.h>
#endif

//This creates new instances of your plugin, change RNBO::JuceAudioProcessor()
//to your own class if you create a derived plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	nlohmann::json patcher_desc, presets;

#ifdef RNBO_INCLUDE_DESCRIPTION_FILE
	patcher_desc = RNBO::patcher_description;
	presets = RNBO::patcher_presets;
#endif

	return new RNBO::JuceAudioProcessor(patcher_desc, presets);
}
