#include "PluginEditor.h"
#include "SpatialLabArtwork.h"
#include "GuiPreferences.h"
#include "RetroMotion.h"

namespace
{
constexpr float panelWidth = 1672, panelHeight = 941;
const juce::Colour amber(0xffffad19), dark(0xff100b05);
const auto names=[] {std::array<juce::String,19> result;for(int i=0;i<19;++i)result[static_cast<std::size_t>(i)]=juce::String::fromUTF8(spatiallab::roomNames[static_cast<std::size_t>(i)]);return result;}();
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
    // Colour-select only existing warm luminous pixels; grille, lettering and metal stay intact.
    const std::array<juce::Rectangle<int>,2> emberBounds {{{710,35,245,50},{1469,559,60,66}}};
    for (int skin=0;skin<2;++skin) for (int light=0;light<2;++light)
    {
        const auto area=emberBounds[static_cast<std::size_t>(light)];
        auto& mask=emberMasks[static_cast<std::size_t>(skin)][static_cast<std::size_t>(light)];
        mask=juce::Image(juce::Image::ARGB,area.getWidth(),area.getHeight(),true);
        const auto& original=skin==0?episodeI:episodeIV;
        for(int y=0;y<area.getHeight();++y) for(int x=0;x<area.getWidth();++x)
        {
            const auto c=original.getPixelAt((area.getX()+x)*2,(area.getY()+y)*2);
            const float nx=static_cast<float>(area.getX()+x),ny=static_cast<float>(area.getY()+y);
            const bool inside=light==0 ? ((ny>=37 && ny<=48) || (ny>=59 && ny<=73))
                : std::pow((nx-1498)/28,2)+std::pow((ny-592)/29,2)<1;
            if(inside && c.getRed()>180 && c.getGreen()>60 && c.getGreen()<c.getRed()*0.85f && c.getBlue()<c.getGreen()*0.70f)
                mask.setPixelAt(x,y,juce::Colours::black.withAlpha(0.85f));
        }
    }
    guiSettings = spatiallab::gui::openPreferences(settingsFile);
    const auto button = [this](juce::TextButton& control, const juce::String& name, std::function<void()> action)
    {
        control.setButtonText(name); control.setName(name); control.setTooltip(name);
        control.setLookAndFeel(this); control.onClick = std::move(action);
        addAndMakeVisible(control);
    };
    for (int i = 0; i < 3; ++i)
        button(presets[static_cast<std::size_t>(i)], names[static_cast<std::size_t>(i)], [this, i] { const int n=spatiallab::pageStart(selectedRoom())+i;if(n<spatiallab::categoryEnd(selectedRoom()))selectRoom(n); });
    button(episodeSwitch, "Episode", [this] { setEpisodeIV(episodeSwitch.getToggleState()); });
    episodeSwitch.setClickingTogglesState(true);
    episodeSwitch.setTooltip({});
    setEpisodeIV(guiSettings->getBoolValue("episodeIV", true), false);
    button(listUp, "Raum nach oben", [this] { selectRoom((selectedRoom() + 18) % 19); });
    button(listDown, "Raum nach unten", [this] { selectRoom((selectedRoom() + 1) % 19); });
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
    timerCallback();
    visibilityChanged();
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
    return juce::jlimit(0, 18, juce::roundToInt(parameter->convertFrom0to1(parameter->getValue())));
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
        presets[static_cast<std::size_t>(i)].setBounds(panelBounds(148, 313.0f + i * 42.0f, 229, 39));
    size.setBounds(panelBounds(201, 524, 114, 114));
    tone.setBounds(panelBounds(201, 717, 114, 114));
    episodeSwitch.setBounds(panelBounds(1230, 746, 273, 67));
    listUp.setBounds(panelBounds(383, 294, 34, 53));
    listDown.setBounds(panelBounds(383, 396, 34, 49));
}

void SpatialLabEditor::visibilityChanged()
{
    if (isShowing())
    {
        processor.gui.observing.store(true, std::memory_order_relaxed);
        timerCallback(); startTimerHz(30);
    }
    else
    {
        stopTimer();
        processor.gui.observing.store(false, std::memory_order_relaxed);
    }
}
void SpatialLabEditor::parentHierarchyChanged() { visibilityChanged(); }

void SpatialLabEditor::timerCallback()
{
    // A host can hide an ancestor without hiding this component itself.
    // Only a cheap visibility probe remains in that case; no animation, telemetry or repaint.
    if (!isShowing())
    {
        processor.gui.observing.store(false, std::memory_order_relaxed);
        startTimer(250); return;
    }
    processor.gui.observing.store(true, std::memory_order_relaxed);
    if (getTimerInterval() != 33) startTimerHz(30);
    size.setValue(processor.parameter("size")->getValue(), juce::dontSendNotification);
    tone.setValue(processor.parameter("tone")->getValue(), juce::dontSendNotification);
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
        for(int row=0;row<3;++row)
        {
            const int n=spatiallab::pageStart(current)+row;
            auto& button=presets[static_cast<std::size_t>(row)];
            button.setVisible(n<spatiallab::categoryEnd(current));
            if(n<19){button.setName(names[static_cast<std::size_t>(n)]);button.setTooltip(names[static_cast<std::size_t>(n)]);}
        }
        repaint();
    }
    else
    {
        repaint(panelBounds(510, 300, 503, 336));
        repaint(panelBounds(1205, 121, 270, 113));
        repaint(panelBounds(710,35,245,50));
        repaint(panelBounds(1469,559,60,66));
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
    const auto imageCentre = bounds.getCentre();
    const auto centre = imageCentre.translated(diameter / 228.0f, &control == &size ? diameter * 11.0f / 228.0f : 0.0f);
    // The outer bevel carries reflections of the stationary scale: keep that material fixed.
    // Rotate only the inset brushed face and its original pointer, from the matching skin.
    const float face = diameter * 0.69f;
    juce::Path mask; mask.addEllipse(centre.x - face * 0.5f, centre.y - face * 0.5f, face, face);
    g.saveState(); g.reduceClipRegion(mask);
    // Rotate the supplied knob pixels (including its original pointer), not a second drawn knob.
    g.addTransform(juce::AffineTransform::rotation((position - 0.5f) * juce::MathConstants<float>::pi * 1.5f, centre.x, centre.y));
    g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
    g.drawImage(panel, static_cast<int>(imageCentre.x - diameter * 0.5f), static_cast<int>(imageCentre.y - diameter * 0.5f),
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
    if(room>=3){drawExtendedRoom(g,room);return;}
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
    text(g,juce::String::fromUTF8(spatiallab::categoryName(room))+" "+juce::String(room+1)+"/19",151,290,224,22,13,amber);
    for(int row=0;row<3;++row)
    {
        const int i=spatiallab::pageStart(room)+row;
        if(i>=spatiallab::categoryEnd(room))break;
        const float y=314.0f+row*42.0f;
        if(i==room){g.setColour(amber);g.fillRoundedRectangle(151,y,221,35,3);}
        text(g,(i==room?juce::String::fromUTF8("› "):juce::String())+names[static_cast<std::size_t>(i)],158,y,209,35,18,i==room?dark:amber);
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
    const std::array<float, 4> tubeCentres {1235, 1307, 1376, 1441};
    const std::array<float, 4> tubeWidths {58, 54, 49, 54};
    for (unsigned i = 0; i < tubeCentres.size(); ++i)
    {
        juce::Graphics::ScopedSaveState state(g);
        const float centre = tubeCentres[i], half = tubeWidths[i] * 0.5f;
        // Entire glass envelope, including the baked-in bright lower glow; stop above the metal base.
        juce::Path glass;
        glass.startNewSubPath(centre-half,208);
        glass.lineTo(centre-half,153);
        glass.cubicTo(centre-half,117,centre+half,117,centre+half,153);
        glass.lineTo(centre+half,208);
        glass.quadraticTo(centre+half,218,centre+half-8,218);
        glass.lineTo(centre-half+8,218);
        glass.quadraticTo(centre-half,218,centre-half,208);
        glass.closeSubPath();
        g.reduceClipRegion(glass);
        const float strength = spatiallab::retro::tubeGlow(animationSeconds, i, aged);
        const float spare = i == 2 ? 0.65f : 1.0f;
        // Darken baked-in light as well as add light: a bright raster cannot visibly dim otherwise.
        g.setColour(juce::Colours::black.withAlpha((1 - strength * spare) * 0.97f)); g.fillPath(glass);
        g.setGradientFill(juce::ColourGradient(juce::Colour(0xffff9219).withAlpha(strength * 0.64f * spare),
            centre, 195, juce::Colour(0xffc1510b).withAlpha(0.02f), centre, 130, false));
        g.fillPath(glass);
        for (float dx : {-8.0f, 8.0f})
        {
            g.setColour(juce::Colour(0xffff901f).withAlpha(strength * 0.65f * spare));
            g.drawLine(centre+dx,171,centre+dx,201,4.5f);
            g.setColour(juce::Colour(0xffffb444).withAlpha(strength * 0.95f * spare));
            g.drawLine(centre+dx,173,centre+dx,200,1.25f);
        }
    }
    const std::array<juce::Point<int>,2> emberOrigins {{{710,35},{1469,559}}};
    for(unsigned i=0;i<2;++i)
    {
        juce::Graphics::ScopedSaveState state(g);
        g.setOpacity((1.0f-spatiallab::retro::emberGlow(animationSeconds,i))*0.85f);
        g.drawImageAt(emberMasks[isEpisodeIV()?1u:0u][i],emberOrigins[i].x,emberOrigins[i].y);
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

void SpatialLabEditor::drawExtendedRoom(juce::Graphics& g,int room)
{
    const auto line=[&](float x,float y,float xx,float yy,float alpha=.85f){g.setColour(amber.withAlpha(alpha));g.drawLine(x,y,xx,yy,1.4f);};
    const auto poly=[&](std::initializer_list<juce::Point<float>> points){juce::Path p;auto it=points.begin();p.startNewSubPath(*it++);while(it!=points.end())p.lineTo(*it++);p.closeSubPath();g.setColour(amber.withAlpha(.85f));g.strokePath(p,juce::PathStrokeType(1.4f));};
    const auto rect=[&](float x,float y,float w,float h){g.setColour(amber.withAlpha(.85f));g.drawRect(x,y,w,h,1.3f);};
    const auto ellipse=[&](float x,float y,float w,float h){g.setColour(amber.withAlpha(.8f));g.drawEllipse(x,y,w,h,1.4f);};
    if(room>=9 && room<=12)
    {
        const float spread=static_cast<float>((room-9)*8);
        poly({{575,370},{941,370},{958,491},{558,491}});
        poly({{609-spread,391},{907+spread,391},{925,460},{590,460}});
        line(575,370,609-spread,391);line(941,370,907+spread,391);
        poly({{590,466},{925,466},{946,515},{567,515}});
        ellipse(635,463,63,41);ellipse(648,471,37,23);line(667,480,667,501);
        rect(740,475,115,20);for(int i=0;i<room-7;++i)rect(748+i*18.0f,481,9,7);
        if(room==9){line(758,391,758,459);rect(780,409,39,18);}
        if(room==10){rect(711,379,91,9);line(575,404,603,418);line(941,404,914,418);}
        if(room==11){line(621,391,621,457);rect(816,399,56,44);line(816,421,872,421);}
        if(room==12){rect(585,378,42,20);rect(890,378,42,20);line(758,391,758,462);ellipse(850,471,18,18);}
        return;
    }
    if(room>=16)
    {
        // Authored schematic cockpit silhouettes, no film stills or imported models.
        poly({{565,354},{950,354},{969,516},{551,516}});
        if(room==16)
        {
            poly({{581,365},{688,396},{688,473},{570,505}});poly({{936,365},{827,396},{827,473},{950,505}});
            rect(696,398,123,73);rect(747,429,23,19);
            for(int i=0;i<5;++i){float y=401+i*13.0f;line(583,y,688,425+(y-435)*.32f,.5f);line(827,425+(y-435)*.32f,937,y,.5f);}
            line(688,396,747,429);line(827,396,770,429);line(688,473,747,448);line(827,473,770,448);
            rect(681,488,153,26);line(758,475,758,519);
        }
        else if(room==17)
        {
            poly({{582,367},{665,397},{665,475},{570,497}});poly({{935,367},{848,397},{848,475},{952,497}});
            ellipse(711,377,94,94);line(715,424,801,424,.5f);ellipse(771,395,16,16);
            for(int i=0;i<6;++i){float x=695+i*16.0f;line(x,386,x+12,458,.22f);}
            rect(690,483,137,29);ellipse(727,487,22,16);ellipse(775,487,22,16);
            for(auto point: {juce::Point<float>{693,368},{839,380},{818,462},{680,446},{865,420}}){line(point.x-2,point.y,point.x+2,point.y);}
        }
        else
        {
            poly({{585,366},{930,366},{943,479},{572,479}});line(700,367,722,479);line(816,367,794,479);
            poly({{739,409},{796,409},{810,430},{731,430}});poly({{810,413},{832,413},{840,424},{810,427}});
            line(742,430,736,453);line(736,453,725,466);line(758,430,763,451);line(763,451,752,468);
            line(785,430,788,454);line(788,454,801,467);line(797,430,809,450);line(809,450,821,466);
            for(float x:{721.0f,749.0f,798.0f,819.0f})line(x,468,x+12,468);
            line(583,473,938,473,.3f);poly({{586,488},{930,488},{945,519},{571,519}});rect(697,493,123,18);
        }
        return;
    }
    if(room==15)
    {
        poly({{553,515},{571,411},{611,372},{663,385},{710,349},{768,370},{812,355},{856,387},{913,374},{958,427},{974,518}});
        poly({{641,496},{663,427},{704,401},{752,417},{797,399},{839,438},{856,497}});
        line(571,411,663,427,.4f);line(913,374,839,438,.4f);line(553,515,641,496);line(974,518,856,497);
        for(int i=0;i<6;++i)line(603+i*57.0f,517,682+i*27.0f,493,.25f);
        return;
    }
    if(room==13 || room==14)
    {
        const int arches=room==14?4:2;
        for(int i=0;i<arches;++i)
        {
            const float inset=i*26.0f;
            poly({{564+inset,514},{564+inset,415},{652+inset*.6f,360+inset*.4f},{758,344+inset*.6f},{866-inset*.6f,360+inset*.4f},{952-inset,415},{952-inset,514}});
        }
        rect(735,424,46,69);if(room==14){ellipse(730,369,56,42);line(758,371,758,411);line(733,389,783,389);}
        for(int i=0;i<4;++i){float y=466+i*14.0f;line(601-i*7.0f,y,719,y);line(796,y,915+i*7.0f,y);}
        line(724,516,739,492,.4f);line(792,516,777,492,.4f);return;
    }
    // Shared fixed perspective, individual authored furnishings/structural geometry.
    poly({{559,359},{958,359},{958,520},{559,520}});rect(669,396,168,78);
    line(559,359,669,396);line(958,359,837,396);line(559,520,669,474);line(958,520,837,474);
    if(room==3){poly({{688,438},{777,438},{817,493},{705,493}});rect(689,431,87,10);line(705,493,705,506);line(813,493,813,505);rect(856,437,43,53);ellipse(865,412,24,16);line(877,429,877,437);}
    if(room==4){rect(721,398,63,76);for(float x:{578.0f,872.0f}){rect(x,405,42,96);line(x+32,450,x+37,450);}line(559,520,721,474);line(958,520,784,474);}
    if(room==5){for(int i=0;i<8;++i){float y=514-i*13.0f;float x=593+i*9.0f;line(x,y,790+i*4.0f,y);line(x,y,x,y-9);line(x,y-9,790+i*4.0f,y-9);}line(580,485,670,378);line(848,492,807,378);}
    if(room==6){for(float x:{581.0f,863.0f}){rect(x,401,58,107);for(float y:{423.0f,451.0f,481.0f})line(x,y,x+58,y);}ellipse(716,443,39,49);ellipse(775,450,36,43);line(564,380,931,380);line(610,380,610,406);}
    if(room==7){poly({{558,410},{758,345},{958,410},{958,520},{558,520}});line(758,345,758,396);for(float x:{602.0f,655.0f,863.0f,914.0f})line(x,395,x,518,.45f);rect(703,443,85,43);poly({{800,467},{846,467},{875,499},{817,499}});}
    if(room==8){rect(687,402,137,67);for(int i=0;i<4;++i)line(693+i*32.0f,403,693+i*32.0f,469);poly({{582,460},{704,460},{740,479},{602,479}});line(603,479,603,514);line(728,479,728,511);rect(856,432,61,62);ellipse(865,485,11,11);ellipse(897,485,11,11);}
}
