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
        bool episodeIV = true;
        int roomIndex = 0;
        float knob = 0.5f;
        int width = 1504, height = 846;
        const auto tokens = juce::StringArray::fromTokens(args, true);
        for (const auto& token : tokens)
        {
            if (token.startsWith("--knob=")) knob = token.fromFirstOccurrenceOf("=",false,false).getFloatValue();
            if (token == "--silent") silent = true;
            if (token == "--sequence") sequence = true;
            if (token.startsWith("--size="))
            {
                const auto dimensions = juce::StringArray::fromTokens(token.fromFirstOccurrenceOf("=",false,false),"x","");
                if (dimensions.size() == 2) { width = dimensions[0].getIntValue(); height = dimensions[1].getIntValue(); }
            }
            if (token == "--episode=I") episodeIV = false;
            if (token.startsWith("--room=")) roomIndex = juce::jlimit(0,2,token.fromFirstOccurrenceOf("=",false,false).getIntValue());
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
        if (GetSystemMetrics(SM_CXSCREEN) < 1800 || GetSystemMetrics(SM_CYSCREEN) < 1000)
        {
            DEVMODE mode {}; mode.dmSize = sizeof(mode);
            if (EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &mode))
            {
                mode.dmPelsWidth = 1920; mode.dmPelsHeight = 1080;
                mode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
                ChangeDisplaySettings(&mode, 0);
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
        window->setVisible(true);
        startTimerHz(30);
    }
    void shutdown() override
    {
        stopTimer(); window.reset(); processor.reset();
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
        // Actual desktop pixels of the visible production editor, not a concept render.
        const auto bounds = editor->getScreenBounds();
        const int w = bounds.getWidth(), h = bounds.getHeight();
        if (bounds.getX()<0 || bounds.getY()<0 || bounds.getRight()>GetSystemMetrics(SM_CXSCREEN)
            || bounds.getBottom()>GetSystemMetrics(SM_CYSCREEN))
        { setApplicationReturnValue(1); quit(); return; }
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
                juce::Image screenshot(juce::Image::RGB, w, h, false);
                juce::Image::BitmapData destination(screenshot,juce::Image::BitmapData::writeOnly);
                for (int y = 0; y < h; ++y)
                    for (int x = 0; x < w; ++x)
                    {
                        const auto offset = static_cast<std::size_t>((y * w + x) * 4);
                        destination.setPixelColour(x, y, juce::Colour(pixels[offset + 2], pixels[offset + 1], pixels[offset]));
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
    bool capturing = false, silent = false, sequence = false;
};
START_JUCE_APPLICATION(Preview)
