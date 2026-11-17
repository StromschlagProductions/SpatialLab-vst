#pragma once
#include "PluginProcessor.h"
#include "PresetBrowser.h"

// The supplied raster remains the front panel; controls are aligned to its native coordinates.
class SpatialLabEditor final : public juce::AudioProcessorEditor, private juce::Timer,
                               private juce::LookAndFeel_V4, private juce::AudioProcessorParameter::Listener
{
public:
    explicit SpatialLabEditor(SpatialLabProcessor&, const juce::File& guiSettingsFile = {});
    void setLook(int look, bool persist = true);
    int getLook() const noexcept { return currentLook; }
    void setEpisodeIV(bool selected, bool persist = true);
    bool isEpisodeIV() const noexcept { return episodeSwitch.getToggleState(); }
    ~SpatialLabEditor() override;
    void resized() override;
    void visibilityChanged() override;
    void parentHierarchyChanged() override;
    void paint(juce::Graphics&) override;
private:
    void timerCallback() override;
    void parameterValueChanged(int, float) override { roomNotification.store(true, std::memory_order_release); }
    void parameterGestureChanged(int, bool) override {}
    void selectRoom(int);
    int selectedRoom() const;
    void drawRotarySlider(juce::Graphics&, int, int, int, int, float, float, float, juce::Slider&) override;
    void drawButtonBackground(juce::Graphics&, juce::Button&, const juce::Colour&, bool, bool) override;
    void drawButtonText(juce::Graphics&, juce::TextButton&, bool, bool) override {}
    void drawClassic(juce::Graphics&);
    void drawCrt(juce::Graphics&);
    void drawSmallRoom(juce::Graphics&);
    void drawRoomGeometry(juce::Graphics&, int room);
    void drawOutdoorGeometry(juce::Graphics&,int);
    void drawExtendedRoom(juce::Graphics&,int);
    void drawLighting(juce::Graphics&);
    void drawEpisodeSwitch(juce::Graphics&, juce::Button&, bool hover);
    juce::Rectangle<int> panelBounds(float x, float y, float width, float height) const;
    SpatialLabProcessor& processor;
    juce::Image panel, episodeI, episodeIV, logo;
    std::array<std::array<juce::Image,2>,2> emberMasks;
    std::unique_ptr<juce::PropertiesFile> guiSettings;
    juce::Slider size, tone;
    PresetBrowser browser;
    std::atomic<bool> roomNotification {true};
    juce::TextButton episodeSwitch, listUp, listDown, lookGear;
    juce::ComboBox lookSelector;
    int currentLook=0;
    std::unique_ptr<juce::SliderParameterAttachment> sizeAttachment, toneAttachment;
    float scale = 1, offsetX = 0, offsetY = 0, signal = 0, phase = 0;
    double animationStarted = juce::Time::getMillisecondCounterHiRes() * 0.001, animationSeconds = 0;
    bool ready = false;
    int displayedRoom = -1;
    juce::String configuration;
    juce::TooltipWindow tooltips {this, 650};
};
