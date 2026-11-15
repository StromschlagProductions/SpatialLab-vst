#include "PluginEditor.h"
#include "SpatialLabArtwork.h"
#include "GuiPreferences.h"
#include "RetroMotion.h"

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
    guiSettings = spatiallab::gui::openPreferences(settingsFile);
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
    const int savedWidth = guiSettings->getIntValue("windowWidth", 1504);
    const int savedHeight = guiSettings->getIntValue("windowHeight", 846);
    const bool validSize = savedWidth >= 1003 && savedWidth <= 1672 && savedHeight >= 565 && savedHeight <= 941
        && std::abs(savedWidth / static_cast<double>(savedHeight) - panelWidth / panelHeight) < 0.004;
    setSize(validSize ? savedWidth : 1504, validSize ? savedHeight : 846);
    processor.gui.peak.store(0, std::memory_order_relaxed);
    processor.gui.observing.store(true, std::memory_order_relaxed);
    timerCallback(); startTimerHz(30);
}

SpatialLabEditor::~SpatialLabEditor()
{
    stopTimer();
    guiSettings->setValue("windowWidth", getWidth());
    guiSettings->setValue("windowHeight", getHeight());
    guiSettings->saveIfNeeded();
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
    animationSeconds = juce::Time::getMillisecondCounterHiRes() * 0.001 - animationStarted;
    phase = static_cast<float>(std::fmod(animationSeconds * 5.4, 6.28318530718));
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
        repaint(panelBounds(926, 184, 112, 45));
        repaint(panelBounds(793, 732, 216, 54));
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
    // Clear only the glass region. The immutable artwork still supplies the bezel.
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff201405), 755, 461, dark, 516, 310, true));
    g.fillAll();
    const int room = selectedRoom();
    drawRoomGeometry(g, room);
    text(g, names[static_cast<std::size_t>(room)].toUpperCase(), 647, 314, 222, 25, 17, amber, juce::Justification::centred);
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

void SpatialLabEditor::drawRoomGeometry(juce::Graphics& g, int room)
{
    const auto line = [&g](float x, float y, float x2, float y2, float brightness = 0.78f)
    { g.setColour(amber.withAlpha(brightness)); g.drawLine(x,y,x2,y2,1.15f); };
    const auto polygon = [&g](std::initializer_list<juce::Point<float>> points)
    {
        juce::Path path; auto it = points.begin(); path.startNewSubPath(*it++);
        while (it != points.end()) path.lineTo(*it++);
        path.closeSubPath(); g.setColour(amber.withAlpha(0.85f)); g.strokePath(path,juce::PathStrokeType(1.4f));
    };
    // Original, fixed perspective; no camera motion or physical scene simulation.
    polygon({{558,350},{958,350},{958,526},{558,526}});
    polygon({{668,392},{839,392},{839,470},{668,470}});
    line(558,350,668,392); line(958,350,839,392);
    line(558,526,668,470); line(958,526,839,470);
    for (int i = 1; i < 8; ++i)
    {
        const float f = i / 8.0f;
        line(558 + 400*f,526,668 + 171*f,470,0.25f);
        line(558 + 400*f,350,668 + 171*f,392,0.16f);
    }
    for (float y : {486.0f,505.0f}) line(558,y,958,y,0.20f);
    for (float y : {405.0f,430.0f,454.0f})
    { line(558,y,668,392+(y-350)*78/176,0.15f); line(839,392+(y-350)*78/176,958,y,0.15f); }
    if (room == 0)
    {
        // Sofa: back, seat, arms and feet; floor lamp with a tapered shade.
        g.setColour(amber.withAlpha(0.9f)); g.drawRoundedRectangle(710,430,100,31,4,1.5f);
        polygon({{710,449},{810,449},{824,465},{698,465}});
        polygon({{698,446},{710,449},{710,470},{698,467}});
        polygon({{810,449},{824,446},{824,467},{810,470}});
        line(698,467,824,467); line(706,469,706,478); line(816,469,816,478);
        line(760,431,760,450,0.65f); line(713,458,809,458,0.4f);
        polygon({{886,421},{895,398},{913,398},{923,421}});
        line(904,421,904,492); line(890,493,918,493); line(904,491,890,493); line(904,491,918,493);
        polygon({{584,444},{614,447},{614,494},{584,500}});
    }
    else if (room == 1)
    {
        // Compact cabinets and worktop, sink/spout, side fridge.
        polygon({{684,442},{821,442},{834,450},{691,450}});
        polygon({{691,450},{834,450},{834,481},{691,481}});
        for (float x : {719.0f,747.0f,775.0f,804.0f})
        { line(x,450,x,480); line(x-6,459,x-3,459); }
        polygon({{684,398},{828,398},{828,426},{684,426}});
        for (float x : {732.0f,780.0f}) line(x,399,x,425);
        line(723,410,723,418); line(771,410,771,418); line(818,410,818,418);
        g.setColour(amber.withAlpha(0.9f)); g.drawEllipse(702,441,30,7,1.3f);
        line(718,440,718,428); line(718,428,728,428); line(728,428,728,433);
        for (float x : {777.0f,791.0f})
        { g.setColour(amber.withAlpha(0.8f)); g.drawEllipse(x,442,7,4,1); }
        polygon({{858,410},{882,402},{903,412},{903,487},{858,487}});
        line(882,402,882,482); line(858,435,882,435); line(878,442,878,453);
    }
    else
    {
        // Tiled walls, bathtub with inset rim, pedestal basin, tap and mirror.
        for (float x : {692.0f,716.0f,740.0f,764.0f,788.0f,812.0f}) line(x,393,x,470,0.20f);
        for (float y : {410.0f,429.0f,448.0f}) line(668,y,839,y,0.20f);
        polygon({{686,452},{775,452},{806,470},{714,470}});
        polygon({{691,456},{773,456},{797,467},{716,467}});
        polygon({{686,452},{714,470},{714,488},{689,472}});
        polygon({{714,470},{806,470},{801,488},{714,488}});
        line(699,452,699,438); line(699,438,711,438); line(711,438,711,442);
        line(719,488,719,494); line(793,488,793,494);
        g.setColour(amber.withAlpha(0.85f)); g.drawRoundedRectangle(851,399,44,32,3,1.5f);
        line(856,427,887,403,0.23f); line(888,426,892,422,0.23f);
        g.setColour(amber.withAlpha(0.9f)); g.drawEllipse(848,443,48,10,1.4f);
        polygon({{849,449},{861,461},{883,461},{895,449}});
        line(869,461,869,490); line(876,461,876,490); line(862,490,883,490);
        line(871,442,871,434); line(871,434,879,434); line(879,434,879,438);
    }
    // Sparse target-computer registration ticks; all remain amber monochrome.
    for (float x : {542.0f,977.0f}) for (float y : {374.0f,431.0f,488.0f}) line(x,y,x+7,y,0.35f);
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
    drawLighting(g);
}

void SpatialLabEditor::drawLighting(juce::Graphics& g)
{
    const bool aged = isEpisodeIV();
    const std::array<float, 4> tubeX {1216, 1290, 1361, 1424};
    for (unsigned i = 0; i < tubeX.size(); ++i)
    {
        juce::Graphics::ScopedSaveState state(g);
        juce::Path glass; glass.addRoundedRectangle(tubeX[i], 133, 32, 80, 13);
        g.reduceClipRegion(glass);
        const float strength = spatiallab::retro::light(animationSeconds, i, i == 1 ? signal : 0, aged);
        const float spare = i == 2 ? 0.55f : 1.0f;
        // Darken baked-in light as well as add light: a bright raster cannot visibly dim otherwise.
        g.setColour(juce::Colours::black.withAlpha((1 - strength * spare) * 0.76f)); g.fillPath(glass);
        g.setGradientFill(juce::ColourGradient(juce::Colour(0xffffa52b).withAlpha(strength * 0.64f * spare),
            tubeX[i] + 16, 195, juce::Colour(0xffc1510b).withAlpha(0.02f), tubeX[i] + 16, 137, false));
        g.fillPath(glass);
        for (float dx : {11.0f, 22.0f})
        {
            g.setColour(juce::Colour(0xffff901f).withAlpha(strength * 0.65f * spare));
            g.drawLine(tubeX[i]+dx,171,tubeX[i]+dx,201,4.5f);
            g.setColour(juce::Colour(0xffffdf91).withAlpha(strength * 0.95f * spare));
            g.drawLine(tubeX[i]+dx,173,tubeX[i]+dx,200,1.25f);
        }
    }
    const std::array<juce::Colour,3> lensColours {juce::Colour(0xffb2ef48),juce::Colour(0xffffba39),juce::Colour(0xff84d6ff)};
    const std::array<float,3> centres {521,605,688};
    // Activity is based on measured wet level, not simply on the host calling processBlock.
    const std::array<float,3> levels {ready ? 1.0f : 0.0f, std::sqrt(signal), signal};
    for (unsigned i = 0; i < 3; ++i)
    {
        const float rhythm = spatiallab::retro::light(animationSeconds,i + 4,levels[i],aged);
        juce::Graphics::ScopedSaveState state(g);
        juce::Path lens; lens.addEllipse(centres[i] - 6,758,12,13); g.reduceClipRegion(lens);
        const float level = levels[i] * rhythm;
        g.setColour(juce::Colours::black.withAlpha(0.92f * (1-level))); g.fillPath(lens);
        g.setGradientFill(juce::ColourGradient(lensColours[i].brighter(0.5f).withAlpha(level * 0.85f),
            centres[i]-1,762,lensColours[i].withAlpha(level * 0.18f),centres[i]+6,769,true));
        g.fillPath(lens);
    }
    const std::array<juce::Rectangle<float>,7> lenses {{{934,190,33,30},{1001,190,31,30},
        {800,740,25,30},{845,740,23,30},{880,740,23,30},{925,740,23,30},{978,740,23,30}}};
    const std::array<juce::Colour,7> lensTints {juce::Colour(0xff8ddfff),juce::Colour(0xffff5732),
        juce::Colour(0xffffecd1),juce::Colour(0xffffc42b),juce::Colour(0xffffa21d),
        juce::Colour(0xff78b4df),juce::Colour(0xffff4926)};
    for (unsigned i = 0; i < lenses.size(); ++i)
    {
        juce::Graphics::ScopedSaveState state(g);
        juce::Path lens; lens.addRoundedRectangle(lenses[i],3); g.reduceClipRegion(lens);
        float rhythm = spatiallab::retro::light(animationSeconds,i+9,i % 3 == 0 ? signal : 0,aged);
        if (i == 1 || i == 5) rhythm *= spatiallab::retro::indicatorGate(animationSeconds,i);
        g.setColour(juce::Colours::black.withAlpha((1-rhythm)*0.85f)); g.fillPath(lens);
        const auto centre = lenses[i].getCentre();
        g.setGradientFill(juce::ColourGradient(lensTints[i].withAlpha(rhythm*0.72f),centre.x,centre.y,
            lensTints[i].withAlpha(0.02f),centre.x,lenses[i].getBottom(),true));
        g.fillPath(lens);
    }
}

void SpatialLabEditor::drawEpisodeSwitch(juce::Graphics& graphics, juce::Button& button, bool hover)
{
    juce::Graphics::ScopedSaveState state(graphics);
    auto& g = graphics;
    g.addTransform(juce::AffineTransform::scale(button.getWidth() / 273.0f, button.getHeight() / 67.0f));
    const bool right = button.getToggleState();
    const auto metal = [right](juce::Colour clean, juce::Colour worn) { return right ? worn : clean; };
    const auto label = juce::Colour(0xff35271e);
    g.setColour(label);
    g.setFont(juce::FontOptions(17.5f, juce::Font::bold));
    // Fit both complete legends at small host sizes; never clip the final V.
    g.drawFittedText("EPISODE I", 0, 19, 91, 29, juce::Justification::centred, 1, 0.72f);
    g.drawFittedText("EPISODE IV", 183, 19, 90, 29, juce::Justification::centred, 1, 0.72f);
    // Recessed socket, cast shadow and concentric machined/chrome surfaces.
    g.setColour(juce::Colours::black.withAlpha(0.32f)); g.fillRoundedRectangle(100, 10, 77, 53, 25);
    g.setGradientFill(juce::ColourGradient(metal(juce::Colour(0xffe3d8c5), juce::Colour(0xff918273)), 130, 5,
                                         metal(juce::Colour(0xff403c35), juce::Colour(0xff302a24)), 144, 59, false));
    g.fillRoundedRectangle(99, 5, 76, 53, 25);
    g.setColour(metal(juce::Colour(0xff4b453b), juce::Colour(0xff3c3025))); g.drawRoundedRectangle(99, 5, 76, 53, 25, 1.3f);
    g.setGradientFill(juce::ColourGradient(metal(juce::Colour(0xff33312b), juce::Colour(0xff201c18)), 137, 12,
                                         metal(juce::Colour(0xffaaa392), juce::Colour(0xff65594a)), 137, 53, false));
    g.fillRoundedRectangle(105, 11, 64, 41, 20);
    g.setColour(juce::Colour(0xff12120f)); g.fillEllipse(121, 18, 32, 29);
    const float tip = right ? 157.0f : 117.0f;
    juce::Path stem;
    stem.startNewSubPath(133, 27); stem.lineTo(tip, 20); stem.lineTo(tip + (right ? 3 : -3), 36);
    stem.lineTo(141, 40); stem.closeSubPath();
    juce::DropShadow(juce::Colours::black.withAlpha(0.65f), 4, {0, 4}).drawForPath(g, stem);
    g.setGradientFill(juce::ColourGradient(metal(juce::Colour(0xfff3e8d6), juce::Colour(0xffa1947d)), 136, 21,
                                         metal(juce::Colour(0xff68665e), juce::Colour(0xff4d483e)), 136, 40, false));
    g.fillPath(stem); g.setColour(juce::Colour(0xff4b4a42)); g.strokePath(stem, juce::PathStrokeType(1));
    g.setGradientFill(juce::ColourGradient(metal(juce::Colour(0xfff8eedb), juce::Colour(0xffb2a489)), tip - 6, 17,
                                         metal(juce::Colour(0xff79786d), juce::Colour(0xff625344)), tip + 7, 41, false));
    g.fillRoundedRectangle(tip - 9, 16, 18, 28, 7);
    g.setColour(juce::Colour(0xff4b4840)); g.drawRoundedRectangle(tip - 9, 16, 18, 28, 7, 1);
    g.setColour(juce::Colours::white.withAlpha(right ? 0.27f : 0.8f)); g.drawLine(tip - 5, 20, tip - 5, 32, 1.5f);
    if (right)
    {
        // Deterministic material wear clipped to the complete mount and the lever surfaces.
        const auto wear = [&g](const juce::Path& surface, unsigned seed, int count)
        {
            juce::Graphics::ScopedSaveState clip(g); g.reduceClipRegion(surface);
            for (int i = 0; i < count; ++i)
            {
                const auto n = seed + static_cast<unsigned>(i) * 7u;
                const float x = 98 + spatiallab::retro::randomUnit(n) * 78;
                const float y = 5 + spatiallab::retro::randomUnit(n+1) * 54;
                g.setColour((i % 4 == 0 ? juce::Colour(0xff96522c) : juce::Colour(0xff211b14)).withAlpha(i % 4 == 0 ? 0.58f : 0.40f));
                g.fillEllipse(x,y,1.2f+spatiallab::retro::randomUnit(n+2)*2.6f,0.8f+spatiallab::retro::randomUnit(n+3)*1.5f);
                if (i % 3 == 0)
                {
                    g.setColour(juce::Colour(0xffd6c3a0).withAlpha(0.38f));
                    g.drawLine(x,y,x+2+spatiallab::retro::randomUnit(n+4)*5,y-1,0.65f);
                }
            }
        };
        juce::Path rim, inner, cap;
        rim.addRoundedRectangle(99,5,76,53,25); inner.addRoundedRectangle(106,12,62,39,19);
        rim.setUsingNonZeroWinding(false); rim.addPath(inner); wear(rim,41,115);
        wear(stem,357,50); cap.addRoundedRectangle(tip-9,16,18,28,7); wear(cap,719,65);
        g.setColour(juce::Colour(0xff31251b).withAlpha(0.7f));
        g.drawRoundedRectangle(101,7,72,49,24,1.7f);
        g.setColour(juce::Colour(0xff785034).withAlpha(0.6f));
        g.drawRoundedRectangle(105,11,64,41,20,1.2f);
    }
    if (hover || button.hasKeyboardFocus(true))
    {
        g.setColour(amber.withAlpha(0.8f)); g.drawRoundedRectangle(97, 3, 80, 57, 26, 1);
    }
}
