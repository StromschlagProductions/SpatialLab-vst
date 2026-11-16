#pragma once
#include "PluginProcessor.h"

// The supplied raster remains the front panel; controls are aligned to its native coordinates.
class SpatialLabEditor final : public juce::AudioProcessorEditor, private juce::Timer,
                               private juce::LookAndFeel_V4
{
public:
    explicit SpatialLabEditor(SpatialLabProcessor&, const juce::File& guiSettingsFile = {});
    void setEpisodeIV(bool selected, bool persist = true);
    bool isEpisodeIV() const noexcept { return episodeSwitch.getToggleState(); }
    ~SpatialLabEditor() override;
    void resized() override;
    void visibilityChanged() override;
    void parentHierarchyChanged() override;
    void paint(juce::Graphics&) override;
private:
    void timerCallback() override;
    void selectRoom(int);
    int selectedRoom() const;
    void drawRotarySlider(juce::Graphics&, int, int, int, int, float, float, float, juce::Slider&) override;
    void drawButtonBackground(juce::Graphics&, juce::Button&, const juce::Colour&, bool, bool) override;
    void drawButtonText(juce::Graphics&, juce::TextButton&, bool, bool) override {}
    void drawCrt(juce::Graphics&);
    void drawRoomGeometry(juce::Graphics&, int room);
    void drawLighting(juce::Graphics&);
    void drawEpisodeSwitch(juce::Graphics&, juce::Button&, bool hover);
    juce::Rectangle<int> panelBounds(float x, float y, float width, float height) const;
    SpatialLabProcessor& processor;
    juce::Image panel, episodeI, episodeIV;
    std::array<std::array<juce::Image,2>,2> emberMasks;
    std::unique_ptr<juce::PropertiesFile> guiSettings;
    juce::Slider size, tone;
    std::array<juce::TextButton, 3> presets;
    juce::TextButton episodeSwitch, listUp, listDown;
    std::unique_ptr<juce::SliderParameterAttachment> sizeAttachment, toneAttachment;
    float scale = 1, offsetX = 0, offsetY = 0, signal = 0, phase = 0;
    double animationStarted = juce::Time::getMillisecondCounterHiRes() * 0.001, animationSeconds = 0;
    bool ready = false;
    int displayedRoom = -1;
    juce::String configuration;
    juce::TooltipWindow tooltips {this, 650};
};
