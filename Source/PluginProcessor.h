#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "RoomEngine.h"
#include "RoomCatalog.h"
#include "GuiTelemetry.h"

// Changes to room reset macros immediately without audio-thread host notifications.
class RoomParameter final : public juce::RangedAudioParameter
{
public:
    RoomParameter(juce::AudioParameterFloat& s,juce::AudioParameterFloat& t,std::atomic<unsigned>& r)
      : RangedAudioParameter(juce::ParameterID{"room",1},"Raum"),size(s),tone(t),revision(r),
        range(0.0f,25.0f,[](float,float,float n){return static_cast<float>(spatiallab::normalizedRoom(n));},
            [](float,float,float i){return spatiallab::roomNormalized(juce::jlimit(0,25,juce::roundToInt(i)));},
            [](float,float,float i){return static_cast<float>(juce::jlimit(0,25,juce::roundToInt(i)));}) {}
    int getIndex() const noexcept {return index.load();}
    const juce::NormalisableRange<float>& getNormalisableRange() const override {return range;}
    float getValue() const override {return spatiallab::roomNormalized(getIndex());}
    void setValue(float n) override
    {
        if(!std::isfinite(n))return;
        const int next=spatiallab::normalizedRoom(n);
        if(index.exchange(next)==next)return;
        static_cast<juce::AudioProcessorParameter&>(size).setValue(.5f);
        static_cast<juce::AudioProcessorParameter&>(tone).setValue(.5f);
        revision.fetch_add(1,std::memory_order_release);
    }
    float getDefaultValue() const override {return 0;}
    int getNumSteps() const override {return 26;}
    bool isDiscrete() const override {return true;}
    juce::String getText(float n,int) const override
    {return juce::String::fromUTF8(spatiallab::roomNames[spatiallab::normalizedRoom(n)]);}
    float getValueForText(const juce::String& text) const override
    {for(int i=0;i<spatiallab::roomCount;++i)if(text==juce::String::fromUTF8(spatiallab::roomNames[i]))return spatiallab::roomNormalized(i);return 0;}

private:
    juce::AudioParameterFloat &size,&tone;
    std::atomic<unsigned>& revision;
    std::atomic<int> index {0};
    juce::NormalisableRange<float> range;
};

class SpatialLabProcessor final : public juce::AudioProcessor, private juce::Timer
{
public:
    SpatialLabProcessor();
    ~SpatialLabProcessor() override { stopTimer(); }
    void prepareToPlay(double, int) override;
    void releaseResources() override { gui.sampleRate.store(0, std::memory_order_relaxed); }
    void reset() override { engine.reset(); }
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void processBlockBypassed(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    bool isBusesLayoutSupported(const BusesLayout&) const override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "SpatialLab"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 24.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;
    GuiTelemetry gui;
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
