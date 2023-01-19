#include "JuceHeader.h"
#include "RNBO.h"
#include "RootComponent.h"

class CustomAudioEditor : public AudioProcessorEditor, public RNBO::EventHandler
{
public:
    CustomAudioEditor(AudioProcessor* const p, RNBO::CoreObject& rnboObject)
        : AudioProcessorEditor (p)
        , _rnboObject(rnboObject)
        , _parameterInterface(_rnboObject.createParameterInterface(RNBO::ParameterEventInterface::SingleProducer, this))
    {
        addAndMakeVisible(_rootComponent);

        setSize (_rootComponent.getWidth(), _rootComponent.getHeight());
    }

    void paint (Graphics& g)
    {
        g.fillAll (Colours::white);
    }

    void eventsAvailable() override
    {
        // Handle RNBO events somehow
    }

protected:
    RNBO::CoreObject&                           _rnboObject; 
    RNBO::ParameterEventInterfaceUniquePtr      _parameterInterface; 
    RootComponent                               _rootComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomAudioEditor)
};

AudioProcessorEditor* createCustomEditor(AudioProcessor * const &p, RNBO::CoreObject &c)
{
    return new CustomAudioEditor(p, c);
}