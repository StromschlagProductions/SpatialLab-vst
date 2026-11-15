#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "RoomEngine.h"

// Changes to room reset macros immediately without audio-thread host notifications.
class RoomParameter final : public juce::AudioParameterChoice
{
public:
    RoomParameter(juce::AudioParameterFloat& sizeControl, juce::AudioParameterFloat& toneControl,
                  std::atomic<unsigned>& revision)
        : AudioParameterChoice(juce::ParameterID{"room", 1}, "Raum",
            juce::StringArray{"Living Room", "Kitchen", "Bathroom"}, 0),
          size(sizeControl), tone(toneControl), resetRevision(revision) {}
private:
    void valueChanged(int index) override
    {
        if (lastRoom.exchange(index) == index) return;
        static_cast<juce::AudioProcessorParameter&>(size).setValue(0.5f);
        static_cast<juce::AudioProcessorParameter&>(tone).setValue(0.5f);
        resetRevision.fetch_add(1, std::memory_order_release);
    }
    juce::AudioParameterFloat& size;
    juce::AudioParameterFloat& tone;
    std::atomic<unsigned>& resetRevision;
    std::atomic<int> lastRoom {0};
};

class SpatialLabProcessor final : public juce::AudioProcessor, private juce::Timer
{
public:
    SpatialLabProcessor();
    ~SpatialLabProcessor() override { stopTimer(); }
    void prepareToPlay(double, int) override;
    void releaseResources() override {}
    void reset() override { engine.reset(); }
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void processBlockBypassed(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
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
    juce::RangedAudioParameter* parameter(const juce::String& id) const;
private:
    void timerCallback() override;
    spatiallab::Engine engine;
    std::atomic<unsigned> resetRevision {0};
    unsigned notifiedRevision = 0, audioRevision = 0;
    juce::AudioParameterFloat *size = nullptr, *tone = nullptr;
    RoomParameter* room = nullptr;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpatialLabProcessor)
};
