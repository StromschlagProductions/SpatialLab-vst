#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "RoomEngine.h"

class SpatialLabProcessor final : public juce::AudioProcessor
{
public:
    SpatialLabProcessor();
    void prepareToPlay(double, int) override;
    void releaseResources() override {}
    void reset() override { engine.reset(); }
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    bool isBusesLayoutSupported(const BusesLayout&) const override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "SpatialLab"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 6.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;
    juce::AudioProcessorValueTreeState parameters;
private:
    static juce::AudioProcessorValueTreeState::ParameterLayout makeParameters();
    spatiallab::Engine engine;
    std::atomic<float> *room, *mix, *decay, *damping, *width, *output;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpatialLabProcessor)
};
