#include "WebBrowserAudioEditor.h"
#include "BinaryData.h"

// The dev server address. When a server is listening here, the browser loads from it
// instead of the built-in resource provider, so you can iterate on src/webui/ without
// recompiling. Use a server that disables caching and (optionally) auto-reloads:
//
//   # No-cache static server (manual page reload required after edits):
//   python3 -c "
//   import http.server
//   class H(http.server.SimpleHTTPRequestHandler):
//       def end_headers(self):
//           self.send_header('Cache-Control', 'no-store')
//           super().end_headers()
//   http.server.test(HandlerClass=H, port=3000)
//   "
//
//   # Hot-reload (auto-reloads browser on file change, requires Node):
//   npx live-server --port=3000
//
// pageLoadHadNetworkError falls back to the resource provider when no server is running.
#if JUCE_ANDROID
static const juce::String kDevServerAddress = "http://10.0.2.2:3000/";
#else
static const juce::String kDevServerAddress = "http://localhost:3000/";
#endif

//==============================================================================
// SinglePageBrowser implementation

bool WebBrowserAudioEditor::SinglePageBrowser::pageAboutToLoad (const String& newURL)
{
    // Allow the dev server and the JUCE resource provider root; block everything else
    // so the single-page UI can't accidentally navigate away.
    return newURL.startsWith (kDevServerAddress)
        || newURL == getResourceProviderRoot();
}

bool WebBrowserAudioEditor::SinglePageBrowser::pageLoadHadNetworkError (const String& /*errorInfo*/)
{
    // Dev server is not reachable — fall back to serving the file via the resource provider.
    // The flag prevents an infinite loop if the resource provider also somehow fails.
    if (! _devServerFailed)
    {
        _devServerFailed = true;
        goToURL (getResourceProviderRoot());
        return false;  // we handled it; don't show the browser's error page
    }

    return true;  // let the browser show its error page
}

//==============================================================================
namespace
{
    static const char* getMimeForExtension (const String& ext)
    {
        if (ext == "html" || ext == "htm") return "text/html";
        if (ext == "css")                  return "text/css";
        if (ext == "js")                   return "text/javascript";
        if (ext == "json")                 return "application/json";
        if (ext == "svg")                  return "image/svg+xml";
        if (ext == "png")                  return "image/png";
        if (ext == "jpg" || ext == "jpeg") return "image/jpeg";
        if (ext == "ico")                  return "image/vnd.microsoft.icon";
        if (ext == "woff2")                return "font/woff2";
        return "application/octet-stream";
    }

    static std::vector<std::byte> stringToBytes (const String& s)
    {
        const auto* ptr = s.toRawUTF8();
        const auto  len = s.getNumBytesAsUTF8();
        std::vector<std::byte> result (len);
        std::memcpy (result.data(), ptr, len);
        return result;
    }
}

//==============================================================================
// Members are initialized in declaration order (see WebBrowserAudioEditor.h).
// Relays and _webComponent use default member initializers; the
// WebSliderParameterAttachment members need the processor's parameters so they
// are initialized here.
WebBrowserAudioEditor::WebBrowserAudioEditor (RNBO::JuceAudioProcessor* const p,
                                              RNBO::CoreObject& rnboObject)
    : AudioProcessorEditor (p)
    , _audioProcessor (p)
    , _rnboObject (rnboObject)
    , _kink1Attachment (static_cast<RangedAudioParameter&> (*p->getParameters()[0]),
                        _kink1Relay, nullptr)
    , _kink2Attachment (static_cast<RangedAudioParameter&> (*p->getParameters()[1]),
                        _kink2Relay, nullptr)
    , _kink3Attachment (static_cast<RangedAudioParameter&> (*p->getParameters()[2]),
                        _kink3Relay, nullptr)
{
    addAndMakeVisible (_webComponent);

    // Try the dev server first. If nothing is listening on that port,
    // pageLoadHadNetworkError fires quickly and redirects to getResourceProviderRoot().
    _webComponent.goToURL (kDevServerAddress);

    setSize (400, 320);
}

WebBrowserAudioEditor::~WebBrowserAudioEditor()
{
    _audioProcessor->AudioProcessor::removeListener (this);
}

void WebBrowserAudioEditor::paint (Graphics& g)
{
    g.fillAll (Colour (0xff16161e));
}

void WebBrowserAudioEditor::resized()
{
    _webComponent.setBounds (getLocalBounds());
}

std::optional<WebBrowserComponent::Resource>
WebBrowserAudioEditor::getResource (const String& url)
{
    // Map "/" or "" to index.html; strip any leading slash for other paths.
    const auto filename = (url == "/" || url.isEmpty())
                            ? String ("index.html")
                            : url.fromFirstOccurrenceOf ("/", false, false);

    const auto ext  = filename.fromLastOccurrenceOf (".", false, false).toLowerCase();
    const auto mime = getMimeForExtension (ext);

    // Production: serve from binary data compiled into the binary.
    // Mangle the filename the same way juce_add_binary_data does (non-alphanumeric → '_').
    String resourceName;
    for (auto c : filename)
        resourceName += (CharacterFunctions::isLetterOrDigit (c) || c == '_') ? c : juce_wchar ('_');

    int dataSize = 0;
    const char* data = RNBOUIData::getNamedResource (resourceName.toRawUTF8(), dataSize);
    if (data != nullptr)
    {
        const auto* begin = reinterpret_cast<const std::byte*> (data);
        return WebBrowserComponent::Resource { std::vector<std::byte> (begin, begin + dataSize), mime };
    }

    return std::nullopt;
}
