#include "PluginProcessor.h"
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
        const auto tokens = juce::StringArray::fromTokens(args, true);
        for (const auto& token : tokens)
            if (token.startsWith("--output=")) output = juce::File::getCurrentWorkingDirectory().getChildFile(token.fromFirstOccurrenceOf("=", false, false).unquoted());
        processor = std::make_unique<SpatialLabProcessor>();
        processor->prepareToPlay(48000, 1600);
        window = std::make_unique<PreviewWindow>();
        window->setUsingNativeTitleBar(true);
        window->setContentOwned(processor->createEditor(), true);
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
            const float sample = std::sin(static_cast<float>(samples++) * 0.037f) * 0.16f;
            buffer.setSample(0, i, sample); buffer.setSample(1, i, sample * 0.73f);
        }
        juce::MidiBuffer midi; processor->processBlock(buffer, midi);
        if (output == juce::File() || ++frames < 24) return;
        capturing = true; stopTimer();
        output.createDirectory();
        auto* editor = window->getContentComponent();
        for (float factor : {1.0f, 1.25f, 1.5f})
            writeImage(editor->createComponentSnapshot(editor->getLocalBounds(), true, factor),
                "JUCE-Editor-Render-" + juce::String(juce::roundToInt(factor * 100)) + ".png");
#if JUCE_WINDOWS
        // Actual desktop pixels of the visible production editor, not a concept render.
        const auto bounds = editor->getScreenBounds();
        const int w = bounds.getWidth(), h = bounds.getHeight();
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
                for (int y = 0; y < h; ++y)
                    for (int x = 0; x < w; ++x)
                    {
                        const auto offset = static_cast<std::size_t>((y * w + x) * 4);
                        screenshot.setPixelAt(x, y, juce::Colour(pixels[offset + 2], pixels[offset + 1], pixels[offset]));
                    }
                writeImage(screenshot, "Windows-Editor-Screenshot.png");
            }
            else setApplicationReturnValue(1);
        }
        else { SelectObject(memory, old); setApplicationReturnValue(1); }
        DeleteObject(bitmap); DeleteDC(memory); ReleaseDC(nullptr, screen);
#endif
        quit();
    }
    juce::File output;
    std::unique_ptr<SpatialLabProcessor> processor;
    std::unique_ptr<juce::DocumentWindow> window;
    int frames = 0, samples = 0;
    bool capturing = false;
};
START_JUCE_APPLICATION(Preview)
