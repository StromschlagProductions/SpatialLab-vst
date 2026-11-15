#pragma once
#include "PluginProcessor.h"

class SpatialLabEditor final : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit SpatialLabEditor(SpatialLabProcessor&);
    ~SpatialLabEditor() override { stopTimer(); }
    void resized() override;
    void paint(juce::Graphics&) override;
private:
    void timerCallback() override;
    SpatialLabProcessor& processor;
    juce::ComboBox room;
    juce::Slider size, tone;
    juce::Label roomLabel, sizeLabel, toneLabel;
    std::unique_ptr<juce::SliderParameterAttachment> sizeAttachment, toneAttachment;
};
