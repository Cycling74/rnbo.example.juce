#pragma once

#include "JuceHeader.h"
#include "RNBO.h"
#include "RNBO_JuceAudioProcessor.h"

class WebBrowserAudioEditor : public AudioProcessorEditor,
                              private AudioProcessorListener
{
public:
    WebBrowserAudioEditor (RNBO::JuceAudioProcessor* const p, RNBO::CoreObject& rnboObject);
    ~WebBrowserAudioEditor() override;
    void paint (Graphics& g) override;
    void resized() override;

private:
    void audioProcessorChanged (AudioProcessor*, const ChangeDetails&) override {}
    void audioProcessorParameterChanged (AudioProcessor*, int, float) override {}

    std::optional<WebBrowserComponent::Resource> getResource (const String& url);

    // Relays must be declared before _webComponent so they are initialized first.
    RNBO::JuceAudioProcessor* _audioProcessor;
    RNBO::CoreObject&         _rnboObject;

    WebSliderRelay _kink1Relay { "kink1" };
    WebSliderRelay _kink2Relay { "kink2" };
    WebSliderRelay _kink3Relay { "kink3" };

    // Defined in the .cpp so pageAboutToLoad/pageLoadHadNetworkError can reference
    // the kDevServerAddress constant without it being visible in this header.
    struct SinglePageBrowser : WebBrowserComponent
    {
        using WebBrowserComponent::WebBrowserComponent;
        bool pageAboutToLoad (const String& newURL) override;
        bool pageLoadHadNetworkError (const String& errorInfo) override;
    private:
        bool _devServerFailed = false;
    };

    SinglePageBrowser _webComponent {
        WebBrowserComponent::Options{}
            .withBackend (WebBrowserComponent::Options::Backend::webview2)
            .withWinWebView2Options (WebBrowserComponent::Options::WinWebView2{}
                .withUserDataFolder (File::getSpecialLocation (
                    File::SpecialLocationType::tempDirectory)))
            .withNativeIntegrationEnabled()
            .withOptionsFrom (_kink1Relay)
            .withOptionsFrom (_kink2Relay)
            .withOptionsFrom (_kink3Relay)
            .withResourceProvider ([this] (const auto& url) { return getResource (url); })
    };

    // Attachments link each relay to the corresponding RNBO RangedAudioParameter.
    // Declared after _webComponent; initialized in the constructor initializer list.
    WebSliderParameterAttachment _kink1Attachment;
    WebSliderParameterAttachment _kink2Attachment;
    WebSliderParameterAttachment _kink3Attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WebBrowserAudioEditor)
};
