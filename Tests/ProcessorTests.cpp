#include "PluginProcessor.h"
#include <iostream>
#include <stdexcept>

static void require(bool value, const char* message) { if (!value) throw std::runtime_error(message); }
static void parameter(SpatialLabProcessor& p, const char* id, float value)
{
    auto* v = p.parameters.getParameter(id);
    v->setValueNotifyingHost(v->convertTo0to1(value));
}
int main()
{
    try
    {
        SpatialLabProcessor processor;
        auto layout = processor.getBusesLayout();
        for (const auto channels : {std::pair<int, int>{1, 1}, {1, 2}, {2, 2}})
        {
            layout.inputBuses.set(0, juce::AudioChannelSet::canonicalChannelSet(channels.first));
            layout.outputBuses.set(0, juce::AudioChannelSet::canonicalChannelSet(channels.second));
            require(processor.setBusesLayout(layout), "Supported layout rejected");
            parameter(processor, "mix", 0);
            for (double rate : {44100.0, 48000.0, 96000.0})
            {
                processor.prepareToPlay(rate, 64);
                for (int count : {0, 1, 7, 64, 511, 2048})
                {
                    juce::AudioBuffer<float> buffer(channels.second, count);
                    buffer.clear();
                    for (int i = 0; i < count; ++i)
                    {
                        buffer.setSample(0, i, 0.25f);
                        if (channels.second == 2) buffer.setSample(1, i, channels.first == 2 ? -0.5f : 99.0f);
                    }
                    juce::MidiBuffer midi;
                    processor.processBlock(buffer, midi);
                    for (int i = 0; i < count; ++i)
                    {
                        require(buffer.getSample(0, i) == 0.25f, "Left/mono dry routing failed");
                        if (channels.second == 2)
                            require(buffer.getSample(1, i) == (channels.first == 2 ? -0.5f : 0.25f), "Stereo/mono expansion failed");
                    }
                }
                processor.releaseResources();
            }
        }
        layout.inputBuses.set(0, juce::AudioChannelSet::stereo());
        layout.outputBuses.set(0, juce::AudioChannelSet::mono());
        require(!processor.isBusesLayoutSupported(layout), "Stereo-to-mono layout accepted");
        layout.inputBuses.set(0, juce::AudioChannelSet::discreteChannels(6));
        layout.outputBuses.set(0, juce::AudioChannelSet::discreteChannels(6));
        require(!processor.isBusesLayoutSupported(layout), "Surround layout accepted");
        for (const auto* id : {"room", "mix", "decay", "damping", "width", "output"})
            parameter(processor, id, processor.parameters.getParameter(id)->convertFrom0to1(0.73f));
        juce::MemoryBlock state;
        processor.getStateInformation(state);
        SpatialLabProcessor restored;
        restored.setStateInformation(state.getData(), static_cast<int>(state.getSize()));
        for (const auto* id : {"room", "mix", "decay", "damping", "width", "output"})
            require(std::abs(processor.parameters.getRawParameterValue(id)->load()
                - restored.parameters.getRawParameterValue(id)->load()) < 1e-5f, "Session recall mismatch");
        const auto before = restored.parameters.getRawParameterValue("mix")->load();
        parameter(processor, "mix", 0.1f);
        auto future = processor.parameters.copyState();
        future.setProperty("schemaVersion", 99, nullptr);
        juce::MemoryBlock futureState;
        if (auto xml = future.createXml()) juce::AudioProcessor::copyXmlToBinary(*xml, futureState);
        restored.setStateInformation(futureState.getData(), static_cast<int>(futureState.getSize()));
        require(restored.parameters.getRawParameterValue("mix")->load() == before, "Unknown state schema changed parameters");
        const char invalid[] = "not a plugin state";
        restored.setStateInformation(invalid, sizeof(invalid));
        require(restored.parameters.getRawParameterValue("mix")->load() == before, "Invalid state changed parameters");
        std::cout << "PASS bus layouts, mono/stereo routing, zero/oversized blocks, sample rates, session recall\n";
    }
    catch (const std::exception& e) { std::cerr << e.what() << '\n'; return 1; }
}
