#include "PluginEditor.h"
#include "SpatialLabArtwork.h"

namespace
{
constexpr float panelWidth = 1672, panelHeight = 941;
const juce::Colour amber(0xffffad19), dark(0xff100b05);
const std::array<juce::String, 3> names {"Wohnzimmer", juce::String::fromUTF8("Küche"), "Badezimmer"};
void text(juce::Graphics& g, const juce::String& value, float x, float y, float w, float h,
          float height, juce::Colour colour, int justification = juce::Justification::centredLeft)
{
    g.setColour(colour);
    g.setFont(juce::FontOptions(height, juce::Font::bold));
    g.drawText(value, juce::Rectangle<float>(x, y, w, h), justification, false);
}
}

SpatialLabEditor::SpatialLabEditor(SpatialLabProcessor& p, const juce::File& settingsFile) : AudioProcessorEditor(p), processor(p)
{
    episodeI = juce::ImageCache::getFromMemory(SpatialLabArtwork::SpatialLab_Episode_I_png,
                                              SpatialLabArtwork::SpatialLab_Episode_I_pngSize);
    episodeIV = juce::ImageCache::getFromMemory(SpatialLabArtwork::SpatialLab_Episode_IV_png,
                                               SpatialLabArtwork::SpatialLab_Episode_IV_pngSize);
    juce::PropertiesFile::Options options;
    options.applicationName = "SpatialLab";
    options.filenameSuffix = ".gui.settings";
    options.folderName = "Stromschlag Productions";
    options.osxLibrarySubFolder = "Application Support";
    options.millisecondsBeforeSaving = 0;
    guiSettings = settingsFile == juce::File()
        ? std::make_unique<juce::PropertiesFile>(options)
        : std::make_unique<juce::PropertiesFile>(settingsFile, options);
    const auto button = [this](juce::TextButton& control, const juce::String& name, std::function<void()> action)
    {
        control.setButtonText(name); control.setName(name); control.setTooltip(name);
        control.setLookAndFeel(this); control.onClick = std::move(action);
        addAndMakeVisible(control);
    };
    for (int i = 0; i < 3; ++i)
        button(presets[static_cast<std::size_t>(i)], names[static_cast<std::size_t>(i)], [this, i] { selectRoom(i); });
    button(episodeSwitch, "Episode", [this] { setEpisodeIV(episodeSwitch.getToggleState()); });
    episodeSwitch.setClickingTogglesState(true);
    episodeSwitch.setTooltip({});
    setEpisodeIV(guiSettings->getBoolValue("episodeIV", true), false);
    button(listUp, "Raum nach oben", [this] { selectRoom((selectedRoom() + 2) % 3); });
    button(listDown, "Raum nach unten", [this] { selectRoom((selectedRoom() + 1) % 3); });
    const auto knob = [this](juce::Slider& control, const juce::String& name, const juce::String& help)
    {
        control.setName(name); control.setTooltip(help);
        control.setRange(0, 1, 0);
        control.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        control.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        control.setDoubleClickReturnValue(true, 0.5);
        control.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
        control.setLookAndFeel(this); control.setWantsKeyboardFocus(true);
        addAndMakeVisible(control);
    };
    knob(size, juce::String::fromUTF8("Größe"), juce::String::fromUTF8("Kleiner ↔ größer · Doppelklick: neutral"));
    knob(tone, "Klang", juce::String::fromUTF8("Dunkler ↔ heller · Doppelklick: neutral"));
    sizeAttachment = std::make_unique<juce::SliderParameterAttachment>(*processor.parameter("size"), size);
    toneAttachment = std::make_unique<juce::SliderParameterAttachment>(*processor.parameter("tone"), tone);
    setResizable(true, true);
    setResizeLimits(1003, 565, 1672, 941);
    getConstrainer()->setFixedAspectRatio(panelWidth / panelHeight);
    setSize(1003, 565);
    processor.gui.peak.store(0, std::memory_order_relaxed);
    processor.gui.observing.store(true, std::memory_order_relaxed);
    timerCallback(); startTimerHz(30);
}

SpatialLabEditor::~SpatialLabEditor()
{
    stopTimer();
    processor.gui.observing.store(false, std::memory_order_relaxed);
    for (auto& button : presets) button.setLookAndFeel(nullptr);
    for (auto* button : {&episodeSwitch, &listUp, &listDown}) button->setLookAndFeel(nullptr);
    size.setLookAndFeel(nullptr); tone.setLookAndFeel(nullptr);
}

void SpatialLabEditor::setEpisodeIV(bool selected, bool persist)
{
    episodeSwitch.setToggleState(selected, juce::dontSendNotification);
    panel = selected ? episodeIV : episodeI;
    if (persist)
    {
        guiSettings->setValue("episodeIV", selected);
        guiSettings->saveIfNeeded();
    }
    repaint();
}

int SpatialLabEditor::selectedRoom() const
{
    auto* parameter = processor.parameter("room");
    return juce::jlimit(0, 2, juce::roundToInt(parameter->convertFrom0to1(parameter->getValue())));
}

void SpatialLabEditor::selectRoom(int index)
{
    auto* parameter = processor.parameter("room");
    parameter->beginChangeGesture();
    parameter->setValueNotifyingHost(parameter->convertTo0to1(static_cast<float>(index)));
    parameter->endChangeGesture();
    timerCallback();
}

juce::Rectangle<int> SpatialLabEditor::panelBounds(float x, float y, float w, float h) const
{
    return juce::Rectangle<float>(offsetX + x * scale, offsetY + y * scale, w * scale, h * scale).toNearestInt();
}

void SpatialLabEditor::resized()
{
    scale = std::min(getWidth() / panelWidth, getHeight() / panelHeight);
    offsetX = (getWidth() - panelWidth * scale) * 0.5f;
    offsetY = (getHeight() - panelHeight * scale) * 0.5f;
    for (int i = 0; i < 3; ++i)
        presets[static_cast<std::size_t>(i)].setBounds(panelBounds(148, 294.0f + i * 47.0f, 229, 43));
    size.setBounds(panelBounds(201, 524, 114, 114));
    tone.setBounds(panelBounds(201, 717, 114, 114));
    episodeSwitch.setBounds(panelBounds(1230, 746, 273, 67));
    listUp.setBounds(panelBounds(383, 294, 34, 53));
    listDown.setBounds(panelBounds(383, 396, 34, 49));
}

void SpatialLabEditor::timerCallback()
{
    size.setValue(processor.parameter("size")->getValue(), juce::dontSendNotification);
    tone.setValue(processor.parameter("tone")->getValue(), juce::dontSendNotification);
    const auto block = processor.gui.blocks.load(std::memory_order_relaxed);
    activity = block != lastBlock ? 1.0f : activity * 0.9f;
    lastBlock = block;
    const float peak = processor.gui.peak.exchange(0, std::memory_order_relaxed);
    signal = std::max(juce::jlimit(0.0f, 1.0f, std::sqrt(peak)), signal * 0.87f);
    phase += 0.18f;
    const float rate = processor.gui.sampleRate.load(std::memory_order_relaxed);
    ready = rate > 0;
    const int inputs = processor.gui.inputs.load(std::memory_order_relaxed);
    const int outputs = processor.gui.outputs.load(std::memory_order_relaxed);
    const auto previousConfig = configuration;
    configuration = !ready ? "AUDIO BEREITSTELLEN" :
        (inputs == 1 && outputs == 2 ? juce::String::fromUTF8("MONO → STEREO") : outputs == 1 ? "MONO" : "STEREO")
        + juce::String::fromUTF8(" · ") + juce::String(rate / 1000.0f, rate == 44100 ? 1 : 0) + " kHz";
    const int current = selectedRoom();
    if (current != displayedRoom || configuration != previousConfig)
    {
        displayedRoom = current;
        repaint();
    }
    else
    {
        repaint(panelBounds(510, 300, 503, 336));
        repaint(panelBounds(488, 744, 217, 49));
        repaint(panelBounds(1205, 121, 270, 113));
    }
}

void SpatialLabEditor::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                            const juce::Colour&, bool hover, bool down)
{
    if (&button == &episodeSwitch) { drawEpisodeSwitch(g, button, hover); return; }
    if (hover || down || button.hasKeyboardFocus(true))
    {
        g.setColour(amber.withAlpha(down ? 0.25f : 0.1f));
        g.fillRoundedRectangle(button.getLocalBounds().toFloat().reduced(1), 3);
        g.setColour(amber.withAlpha(0.8f));
        g.drawRoundedRectangle(button.getLocalBounds().toFloat().reduced(1), 3, 1);
    }
}

void SpatialLabEditor::drawRotarySlider(juce::Graphics& g, int x, int y, int w, int h,
                                        float position, float, float, juce::Slider& control)
{
    const auto bounds = juce::Rectangle<float>(static_cast<float>(x), static_cast<float>(y),
                                              static_cast<float>(w), static_cast<float>(h));
    const float diameter = std::min(bounds.getWidth(), bounds.getHeight());
    const auto centre = bounds.getCentre();
    juce::Path mask; mask.addEllipse(centre.x - diameter * 0.5f, centre.y - diameter * 0.5f, diameter, diameter);
    g.saveState(); g.reduceClipRegion(mask);
    // Rotate the supplied knob pixels (including its original pointer), not a second drawn knob.
    g.addTransform(juce::AffineTransform::rotation((position - 0.5f) * juce::MathConstants<float>::pi * 1.5f, centre.x, centre.y));
    g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
    g.drawImage(panel, static_cast<int>(centre.x - diameter * 0.5f), static_cast<int>(centre.y - diameter * 0.5f),
                static_cast<int>(diameter), static_cast<int>(diameter), 402, &control == &size ? 1048 : 1434, 228, 228);
    g.restoreState();
    if (control.isMouseOverOrDragging() || control.hasKeyboardFocus(true))
    {
        g.setColour(amber.withAlpha(0.8f)); g.strokePath(mask, juce::PathStrokeType(1.5f));
    }
}

void SpatialLabEditor::drawCrt(juce::Graphics& g)
{
    juce::Path clip; clip.addRoundedRectangle(514, 306, 491, 324, 48);
    g.saveState(); g.reduceClipRegion(clip);
    // Keep the accepted wireframe/furniture pixels exactly as supplied. Only the old
    // baked-in trace is replaced; this is a stylised display, not a physical room model.
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff170e04), 755, 515, dark, 755, 622, false));
    g.fillRect(545, 514, 435, 108);
    const int room = selectedRoom();
    g.setColour(dark.withAlpha(0.86f)); g.fillRoundedRectangle(641, 313, 234, 25, 3);
    text(g, names[static_cast<std::size_t>(room)].toUpperCase(), 647, 313, 222, 25, 17, amber, juce::Justification::centred);
    // A deliberately stylised signal trace, driven only by the observed wet level.
    juce::Path trace; trace.startNewSubPath(554, 574);
    for (int x = 554; x <= 967; x += 3)
    {
        const float t = static_cast<float>(x - 554);
        const float envelope = std::exp(-t * 0.008f);
        const float y = 574 + signal * envelope * 36.0f * std::sin(t * 0.49f + phase) * std::cos(t * 0.071f);
        trace.lineTo(static_cast<float>(x), y);
    }
    g.setColour(amber.withAlpha(0.12f)); g.strokePath(trace, juce::PathStrokeType(7));
    g.setColour(amber); g.strokePath(trace, juce::PathStrokeType(1.5f));
    for (int y = 309; y < 632; y += 4)
    { g.setColour(juce::Colours::black.withAlpha(0.14f)); g.drawHorizontalLine(y, 514, 1005); }
    g.restoreState();
}

void SpatialLabEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    g.addTransform(juce::AffineTransform::scale(scale).translated(offsetX, offsetY));
    g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
    g.drawImage(panel, juce::Rectangle<float>(0, 0, panelWidth, panelHeight));
    const int room = selectedRoom();
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff261706), 145, 290, dark, 375, 446, false));
    g.fillRoundedRectangle(145, 290, 233, 155, 3);
    for (int i = 0; i < 3; ++i)
    {
        const float y = 296.0f + i * 47.0f;
        if (i == room)
        {
            g.setColour(amber); g.fillRoundedRectangle(151, y, 221, 39, 3);
            text(g, juce::String::fromUTF8("› ") + names[static_cast<std::size_t>(i)], 158, y, 207, 39, 24, dark);
        }
        else text(g, names[static_cast<std::size_t>(i)], 175, y, 195, 39, 24, amber);
    }
    // Neutralise the baked-in STEREO/48 kHz claim; show atomically published host data.
    g.setGradientFill(juce::ColourGradient(panel.getPixelAt(2276, 1242), 1138, 621,
                                         panel.getPixelAt(2780, 1300), 1390, 651, false));
    g.fillRect(1138, 621, 252, 30);
    text(g, configuration, 1144, 623, 244, 26, 19, juce::Colour(0xff1b1b16));
    drawCrt(g);
    const auto lamp = [&g](float x, juce::Colour colour, float amount)
    {
        g.setColour(juce::Colour(0xff1b160e)); g.fillEllipse(x - 12, 753, 24, 24);
        g.setColour(colour.withAlpha(0.12f + amount * 0.25f)); g.fillEllipse(x - 17, 748, 34, 34);
        g.setColour(colour.withMultipliedBrightness(0.22f + amount * 0.78f)); g.fillEllipse(x - 7, 758, 14, 14);
        if (amount > 0.1f) { g.setColour(juce::Colours::white.withAlpha(amount * 0.7f)); g.fillEllipse(x - 3, 760, 5, 5); }
    };
    lamp(521, juce::Colour(0xff9dff2a), ready ? 1.0f : 0.0f);
    lamp(605, juce::Colour(0xffff9811), activity);
    lamp(688, juce::Colour(0xff38bfff), signal);
    if (signal > 0.02f)
    {
        g.setColour(amber.withAlpha(signal * 0.13f));
        for (float x : {1232.0f, 1306.0f, 1440.0f}) g.fillEllipse(x - 20, 139, 40, 79);
    }
}

void SpatialLabEditor::drawEpisodeSwitch(juce::Graphics& graphics, juce::Button& button, bool hover)
{
    juce::Graphics::ScopedSaveState state(graphics);
    auto& g = graphics;
    g.addTransform(juce::AffineTransform::scale(button.getWidth() / 273.0f, button.getHeight() / 67.0f));
    const bool right = button.getToggleState();
    const auto label = juce::Colour(0xff35271e);
    text(g, "EPISODE I", 0, 19, 91, 29, 17.5f, label, juce::Justification::centred);
    text(g, "EPISODE IV", 183, 19, 90, 29, 17.5f, label, juce::Justification::centred);
    // Recessed socket, cast shadow and concentric machined/chrome surfaces.
    g.setColour(juce::Colours::black.withAlpha(0.32f)); g.fillRoundedRectangle(100, 10, 77, 53, 25);
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xffe3d8c5), 130, 5,
                                         juce::Colour(0xff403c35), 144, 59, false));
    g.fillRoundedRectangle(99, 5, 76, 53, 25);
    g.setColour(juce::Colour(0xff4b453b)); g.drawRoundedRectangle(99, 5, 76, 53, 25, 1.3f);
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff33312b), 137, 12,
                                         juce::Colour(0xffaaa392), 137, 53, false));
    g.fillRoundedRectangle(105, 11, 64, 41, 20);
    g.setColour(juce::Colour(0xff12120f)); g.fillEllipse(121, 18, 32, 29);
    const float tip = right ? 157.0f : 117.0f;
    juce::Path stem;
    stem.startNewSubPath(133, 27); stem.lineTo(tip, 20); stem.lineTo(tip + (right ? 3 : -3), 36);
    stem.lineTo(141, 40); stem.closeSubPath();
    juce::DropShadow(juce::Colours::black.withAlpha(0.65f), 4, {0, 4}).drawForPath(g, stem);
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xfff3e8d6), 136, 21,
                                         juce::Colour(0xff68665e), 136, 40, false));
    g.fillPath(stem); g.setColour(juce::Colour(0xff4b4a42)); g.strokePath(stem, juce::PathStrokeType(1));
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xfff8eedb), tip - 6, 17,
                                         juce::Colour(0xff79786d), tip + 7, 41, false));
    g.fillRoundedRectangle(tip - 9, 16, 18, 28, 7);
    g.setColour(juce::Colour(0xff4b4840)); g.drawRoundedRectangle(tip - 9, 16, 18, 28, 7, 1);
    g.setColour(juce::Colours::white.withAlpha(0.8f)); g.drawLine(tip - 5, 20, tip - 5, 32, 1.5f);
    if (hover || button.hasKeyboardFocus(true))
    {
        g.setColour(amber.withAlpha(0.8f)); g.drawRoundedRectangle(97, 3, 80, 57, 26, 1);
    }
}
