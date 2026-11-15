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

SpatialLabEditor::SpatialLabEditor(SpatialLabProcessor& p) : AudioProcessorEditor(p), processor(p)
{
    panel = juce::ImageCache::getFromMemory(SpatialLabArtwork::SpatialLab_Retro_Frontplatte_Referenz_png,
                                           SpatialLabArtwork::SpatialLab_Retro_Frontplatte_Referenz_pngSize);
    logo = juce::ImageCache::getFromMemory(SpatialLabArtwork::Stromschlag_Audio_Original_Logo_png,
                                          SpatialLabArtwork::Stromschlag_Audio_Original_Logo_pngSize);
    const auto button = [this](juce::TextButton& control, const juce::String& name, std::function<void()> action)
    {
        control.setButtonText(name); control.setName(name); control.setTooltip(name);
        control.setLookAndFeel(this); control.onClick = std::move(action);
        addAndMakeVisible(control);
    };
    for (int i = 0; i < 3; ++i)
        button(presets[static_cast<std::size_t>(i)], names[static_cast<std::size_t>(i)], [this, i] { selectRoom(i); });
    button(previous, "Vorheriger Raum", [this] { selectRoom((selectedRoom() + 2) % 3); });
    button(next, juce::String::fromUTF8("Nächster Raum"), [this] { selectRoom((selectedRoom() + 1) % 3); });
    button(listUp, "Raum nach oben", [this] { selectRoom((selectedRoom() + 2) % 3); });
    button(listDown, "Raum nach unten", [this] { selectRoom((selectedRoom() + 1) % 3); });
    const auto knob = [this](juce::Slider& control, const juce::String& name, const juce::String& help)
    {
        control.setName(name); control.setTooltip(help);
        control.setRange(0, 1, 0);
        control.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        control.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        control.setDoubleClickReturnValue(true, 0.5);
        control.setRotaryParameters(-juce::MathConstants<float>::pi * 0.75f, juce::MathConstants<float>::pi * 0.75f, true);
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
    for (auto* button : {&previous, &next, &listUp, &listDown}) button->setLookAndFeel(nullptr);
    size.setLookAndFeel(nullptr); tone.setLookAndFeel(nullptr);
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
    previous.setBounds(panelBounds(1235, 765, 40, 43));
    next.setBounds(panelBounds(1450, 765, 42, 43));
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
                static_cast<int>(diameter), static_cast<int>(diameter), 201, &control == &size ? 524 : 717, 114, 114);
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
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff201306), 752, 460, dark, 510, 310, true));
    g.fillAll();
    const auto line = [&g](float x1, float y1, float x2, float y2, float strength = 0.5f)
    { g.setColour(amber.withAlpha(strength)); g.drawLine(x1, y1, x2, y2, 1.25f); };
    // Fixed room perspective. Macro size/tone are controls, not camera motion.
    for (int i = 0; i <= 8; ++i)
    {
        const float x = 558.0f + i * 48.0f;
        line(x, 502, 681 + (x - 558) * 0.39f, 455, 0.35f);
        line(x, 343, 681 + (x - 558) * 0.39f, 383, 0.27f);
    }
    for (float y : {466.0f, 481.0f, 502.0f}) line(558, y, 947, y, 0.38f);
    for (float y : {350.0f, 365.0f, 383.0f}) line(558, y, 947, y, 0.24f);
    line(558, 343, 947, 343, 0.7f); line(558, 343, 558, 502, 0.65f);
    line(947, 343, 947, 502, 0.65f); line(558, 502, 947, 502, 0.7f);
    line(681, 383, 833, 383); line(681, 383, 681, 455); line(833, 383, 833, 455);
    line(681, 455, 833, 455); line(558, 343, 681, 383); line(947, 343, 833, 383);
    const int room = selectedRoom();
    g.setColour(amber.withAlpha(0.9f));
    if (room == 0)
    {
        g.drawRoundedRectangle(714, 429, 89, 29, 4, 2);
        g.drawRoundedRectangle(703, 440, 112, 28, 4, 2);
        line(758, 430, 758, 454, 0.8f); line(711, 468, 711, 474); line(806, 468, 806, 474);
        line(905, 408, 905, 481, 0.9f); line(890, 427, 920, 427, 0.9f);
        line(890, 427, 898, 407, 0.9f); line(898, 407, 914, 407, 0.9f); line(914, 407, 920, 427, 0.9f);
    }
    else if (room == 1)
    {
        g.drawRect(697.0f, 424.0f, 129.0f, 39.0f, 1.5f);
        line(697, 438, 826, 438, 0.9f);
        for (float x : {729.0f, 761.0f, 793.0f}) line(x, 438, x, 462, 0.8f);
        g.drawEllipse(706, 425, 23, 9, 1.5f); line(718, 425, 718, 413); line(718, 413, 727, 413);
        g.drawRect(840.0f, 397.0f, 37.0f, 69.0f, 1.5f); line(840, 422, 877, 422);
    }
    else
    {
        g.drawRoundedRectangle(702, 443, 112, 25, 9, 2);
        line(709, 468, 709, 475); line(807, 468, 807, 475);
        line(711, 443, 711, 430); line(711, 430, 722, 430);
        g.drawEllipse(842, 429, 38, 12, 1.5f); line(861, 441, 861, 469);
        g.drawRoundedRectangle(842, 395, 38, 28, 3, 1.5f);
    }
    text(g, names[static_cast<std::size_t>(room)].toUpperCase(), 585, 314, 337, 24, 18, amber, juce::Justification::centred);
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
    g.drawImageAt(panel, 0, 0);
    // The reference's invented wordmark is completely covered by a local metal badge.
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xffe2c5a5), 136, 93,
                                         juce::Colour(0xffbda083), 350, 190, false));
    g.fillRoundedRectangle(131, 92, 223, 100, 5);
    g.setColour(juce::Colour(0xff715641)); g.drawRoundedRectangle(131, 92, 223, 100, 5, 1);
    g.drawImageWithin(logo, 194, 95, 96, 94, juce::RectanglePlacement::centred);
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
    g.setColour(dark); g.fillRoundedRectangle(1270, 768, 186, 38, 3);
    text(g, names[static_cast<std::size_t>(room)], 1275, 768, 176, 38, 22, amber, juce::Justification::centred);
    // Neutralise the baked-in STEREO/48 kHz claim; show atomically published host data.
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xffc6a991), 1138, 621,
                                         juce::Colour(0xffd3b49a), 1387, 649, false));
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
