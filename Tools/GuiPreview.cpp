#include "PluginEditor.h"
#if JUCE_WINDOWS
 #ifndef NOMINMAX
  #define NOMINMAX
 #endif
 #include <windows.h>
#endif

class PreviewWindow final : public juce::DocumentWindow
{
public:
    PreviewWindow() : DocumentWindow(juce::String::fromUTF8("SPATIAL LAB – Retro GUI"), juce::Colours::black, closeButton) {}
    void closeButtonPressed() override { juce::JUCEApplication::getInstance()->systemRequestedQuit(); }
};

// Opens the actual production editor. Synthetic audio exists only in this preview host.
class Preview final : public juce::JUCEApplication, private juce::Timer
{
public:
    const juce::String getApplicationName() override { return "SpatialLab GUI Preview"; }
    const juce::String getApplicationVersion() override { return "0.1.0"; }
    void initialise(const juce::String& args) override
    {
#if JUCE_WINDOWS
        // Prevent a long CI compilation followed by capture from hitting display idle timeout.
        SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);
#endif
        bool episodeIV = true;
        int roomIndex = 0;
        float knob = 0.5f;
        int width = 1504, height = 846;
        const auto tokens = juce::StringArray::fromTokens(args, true);
        for (const auto& token : tokens)
        {
            if (token.startsWith("--knob=")) knob = token.fromFirstOccurrenceOf("=",false,false).getFloatValue();
            if (token == "--allow-render-only") allowRenderOnly = true;
            if (token == "--silent") silent = true;
            if (token == "--sequence") sequence = true;
            if (token.startsWith("--size="))
            {
                const auto dimensions = juce::StringArray::fromTokens(token.fromFirstOccurrenceOf("=",false,false),"x","");
                if (dimensions.size() == 2) { width = dimensions[0].getIntValue(); height = dimensions[1].getIntValue(); }
            }
            if (token == "--episode=I") episodeIV = false;
            if (token.startsWith("--room=")) roomIndex = juce::jlimit(0,25,token.fromFirstOccurrenceOf("=",false,false).getIntValue());
            if (token.startsWith("--output=")) output = juce::File::getCurrentWorkingDirectory().getChildFile(token.fromFirstOccurrenceOf("=", false, false).unquoted());
        }
        processor = std::make_unique<SpatialLabProcessor>();
        auto* room = processor->parameter("room");
        room->setValueNotifyingHost(room->convertTo0to1(static_cast<float>(roomIndex)));
        processor->parameter("size")->setValueNotifyingHost(knob);
        processor->parameter("tone")->setValueNotifyingHost(knob);
        processor->prepareToPlay(48000, 1600);
#if JUCE_WINDOWS
        // CI's desktop must contain the entire visible editor; never capture clipped/off-screen pixels.
        if (GetSystemMetrics(SM_CXSCREEN) < 2300 || GetSystemMetrics(SM_CYSCREEN) < 1300)
        {
            DEVMODE mode {}; mode.dmSize = sizeof(mode);
            if (EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &mode))
            {
                mode.dmPelsWidth = 2560; mode.dmPelsHeight = 1440;
                mode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
                const auto result = ChangeDisplaySettings(&mode, 0);
                if (result != DISP_CHANGE_SUCCESSFUL) {
                    mode.dmPelsWidth=1920; mode.dmPelsHeight=1080;
                    ChangeDisplaySettings(&mode,0);
                }
                if(output!=juce::File()) {output.createDirectory();output.getChildFile("Display-Capabilities.txt").replaceWithText(
                    "Requested 2560x1440 result="+juce::String(result)+"; actual="+juce::String(GetSystemMetrics(SM_CXSCREEN))+"x"+juce::String(GetSystemMetrics(SM_CYSCREEN)));}
            }
        }
#endif
        window = std::make_unique<PreviewWindow>();
        window->setUsingNativeTitleBar(true);
        auto* editor = output == juce::File() ? new SpatialLabEditor(*processor)
            : new SpatialLabEditor(*processor, output.getChildFile("preview.gui.settings"));
        if (output != juce::File()) editor->setEpisodeIV(episodeIV, false);
        editor->setSize(width,height);
        window->setContentOwned(editor, true);
        window->setTopLeftPosition(0, 0);
        window->setAlwaysOnTop(true);
        window->setVisible(true);
        window->toFront(true);
        // CI screen capture must not depend on a remote desktop's Direct2D swap chain.
        // This selects JUCE's real software window renderer, not an off-screen substitute.
        if (auto* peer = window->getPeer()) peer->setCurrentRenderingEngine(0);
        startTimerHz(30);
    }
    void shutdown() override
    {
        stopTimer(); window.reset(); processor.reset();
#if JUCE_WINDOWS
        SetThreadExecutionState(ES_CONTINUOUS);
#endif
    }
    void systemRequestedQuit() override { quit(); }
private:
    void writeImage(const juce::Image& image, const juce::String& name)
    {
        if (!image.isValid()) { setApplicationReturnValue(1); return; }
        juce::File file = output.getChildFile(name);
        juce::FileOutputStream stream(file);
        if (!stream.openedOk()) { setApplicationReturnValue(1); return; }
        stream.setPosition(0); stream.truncate();
        if (!juce::PNGImageFormat().writeImageToStream(image, stream)) setApplicationReturnValue(1);
    }
    void timerCallback() override
    {
        if (capturing) return;
        juce::AudioBuffer<float> buffer(2, 1600);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            const float sample = std::sin(static_cast<float>(samples++) * 0.037f) * (silent ? 0.0f : 0.16f);
            buffer.setSample(0, i, sample); buffer.setSample(1, i, sample * 0.73f);
        }
        juce::MidiBuffer midi; processor->processBlock(buffer, midi);
        if (output == juce::File() || ++frames < 24) return;
        if (sequence && (frames-24) % 10 != 0) return;
        capturing = true;
        output.createDirectory();
        auto* editor = window->getContentComponent();
        if (captures == 0)
            writeImage(editor->createComponentSnapshot(editor->getLocalBounds()), "JUCE-Editor-Render-100.png");
#if JUCE_WINDOWS
        window->toFront(true);
        editor->repaint();
        if (auto* peer = window->getPeer()) peer->performAnyPendingRepaintsNow();
        const auto reference = editor->createComponentSnapshot(editor->getLocalBounds());
        // Actual desktop pixels of the visible production editor, not a concept render.
        const auto bounds = editor->getScreenBounds();
        const int w = bounds.getWidth(), h = bounds.getHeight();
        if (bounds.getX()<0 || bounds.getY()<0 || bounds.getRight()>GetSystemMetrics(SM_CXSCREEN)
            || bounds.getBottom()>GetSystemMetrics(SM_CYSCREEN))
        { if(allowRenderOnly) {output.getChildFile("NO-DESKTOP-CAPTURE.txt").replaceWithText("CI desktop cannot contain this editor. JUCE-Editor-Render-100.png is a native component render, NOT a desktop screenshot.");}
          else setApplicationReturnValue(1); quit(); return; }
        HDC screen = GetDC(nullptr), memory = CreateCompatibleDC(screen);
        HBITMAP bitmap = CreateCompatibleBitmap(screen, w, h);
        auto old = SelectObject(memory, bitmap);
        if (BitBlt(memory, 0, 0, w, h, screen, bounds.getX(), bounds.getY(), SRCCOPY | CAPTUREBLT))
        {
            BITMAPINFO info {}; info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            info.bmiHeader.biWidth = w; info.bmiHeader.biHeight = -h;
            info.bmiHeader.biPlanes = 1; info.bmiHeader.biBitCount = 32; info.bmiHeader.biCompression = BI_RGB;
            std::vector<unsigned char> pixels(static_cast<std::size_t>(w * h * 4));
            SelectObject(memory, old);
            if (GetDIBits(screen, bitmap, 0, static_cast<UINT>(h), pixels.data(), &info, DIB_RGB_COLORS))
            {
                juce::Image screenshot(juce::Image::RGB, w, h, false, juce::SoftwareImageType());
                {
                juce::Image::BitmapData destination(screenshot,juce::Image::BitmapData::writeOnly);
                for (int y = 0; y < h; ++y)
                    for (int x = 0; x < w; ++x)
                    {
                        const auto offset = static_cast<std::size_t>((y * w + x) * 4);
                        destination.setPixelColour(x, y, juce::Colour(pixels[offset + 2], pixels[offset + 1], pixels[offset]));
                    }
                } // Release the write mapping before any PNG read or clipped-image view.
                // A busy/covered CI desktop occasionally returns a black rectangle although
                // the editor is correct. Check stationary metal, never animated glass/CRT.
                // Reject that acquisition and retry later; never replace desktop pixels
                // with the component render or weaken the lighting assertions.
                bool obscured = false;
                const float anchorScale = std::min(w/1672.0f,h/941.0f);
                for (const auto anchor : {juce::Point<float>{1080,280},{1450,460},{1100,530}})
                {
                    const int x = juce::roundToInt((w-1672*anchorScale)*0.5f+anchor.x*anchorScale);
                    const int y = juce::roundToInt((h-941*anchorScale)*0.5f+anchor.y*anchorScale);
                    const auto actual = screenshot.getPixelAt(x,y), expected = reference.getPixelAt(x,y);
                    obscured = obscured || std::abs(int(actual.getRed())-int(expected.getRed()))
                        + std::abs(int(actual.getGreen())-int(expected.getGreen()))
                        + std::abs(int(actual.getBlue())-int(expected.getBlue())) > 96;
                }
                if (obscured)
                {
                    DeleteObject(bitmap); DeleteDC(memory); ReleaseDC(nullptr,screen);
                    capturing = false;
                    if (++captureRetries > 10) { setApplicationReturnValue(1); quit(); }
                    return;
                }
                captureRetries = 0;
                if (captures >= 46)
                {
                    writeImage(screenshot,"Late-Desktop-"+juce::String(captures)+".png");
                    writeImage(editor->createComponentSnapshot(editor->getLocalBounds()),"Late-Component-"+juce::String(captures)+".png");
                }
                if (captures == 0) writeImage(screenshot, "Windows-Editor-Screenshot.png");
                if (sequence)
                {
                    captureTimes += juce::String(captures) + "," + juce::String(juce::Time::getMillisecondCounterHiRes()*0.001,4) + "\n";
                    if (captures < 16) writeImage(screenshot, "Frame-" + juce::String(captures).paddedLeft('0',3) + ".png");
                    const float factor = std::min(w/1672.0f,h/941.0f);
                    const auto tubeBounds = juce::Rectangle<float>((w-1672*factor)*0.5f+1199*factor,
                        (h-941*factor)*0.5f+116*factor,282*factor,132*factor).toNearestInt();
                    writeImage(screenshot.getClippedImage(tubeBounds), "Tubes-" + juce::String(captures).paddedLeft('0',3) + ".png");
                }
            }
            else setApplicationReturnValue(1);
        }
        else { SelectObject(memory, old); setApplicationReturnValue(1); }
        DeleteObject(bitmap); DeleteDC(memory); ReleaseDC(nullptr, screen);
#endif
        ++captures;
        capturing = false;
        if (!sequence || captures >= (silent ? 48 : 16) || getApplicationReturnValue() != 0)
        {
            output.getChildFile("Capture-Times.csv").replaceWithText("frame,seconds_monotonic\n"+captureTimes);
            stopTimer(); quit();
        }
    }
    juce::String captureTimes;
    juce::File output;
    std::unique_ptr<SpatialLabProcessor> processor;
    std::unique_ptr<juce::DocumentWindow> window;
    int frames = 0, samples = 0;
    int captures = 0;
    int captureRetries = 0;
    bool capturing = false, silent = false, sequence = false, allowRenderOnly = false;
};
START_JUCE_APPLICATION(Preview)
