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
            for (double rate : {44100.0, 48000.0})
            {
                processor.prepareToPlay(rate, 64);
                spatiallab::Engine reference;
                reference.prepare(rate, 0, 0.5f, 1, 0.5f, 1, 1);
                int frame = 0;
                for (int count : {0, 1, 7, 64, 511, 2048})
                {
                    juce::AudioBuffer<float> buffer(channels.second, count);
                    buffer.clear();
                    for (int i = 0; i < count; ++i)
                    {
                        buffer.setSample(0, i, frame % 97 == 0 ? 0.25f : 0);
                        if (channels.second == 2) buffer.setSample(1, i, channels.first == 2 ? (frame % 83 == 0 ? -0.5f : 0) : 99.0f);
                        ++frame;
                    }
                    juce::AudioBuffer<float> input;
                    input.makeCopyOf(buffer);
                    juce::MidiBuffer midi;
                    processor.processBlock(buffer, midi);
                    for (int i = 0; i < count; ++i)
                    {
                        const auto expected = reference.process(input.getSample(0, i),
                            channels.first == 2 ? input.getSample(1, i) : input.getSample(0, i));
                        const float left = channels.second == 1 ? (expected.left + expected.right) * 0.5f : expected.left;
                        require(std::abs(buffer.getSample(0, i) - left) < 1e-6f, "Wet left/mono routing failed");
                        if (channels.second == 2)
                            require(std::abs(buffer.getSample(1, i) - expected.right) < 1e-6f, "Wet right/mono expansion failed");
                        if (frame - count + i == 0)
                            require(buffer.getSample(0, i) == 0, "Processor leaked direct impulse");
                    }
                }
                juce::AudioBuffer<float> bypass(channels.second, 64);
                for (int channel = 0; channel < channels.second; ++channel)
                    for (int i = 0; i < 64; ++i) bypass.setSample(channel, i, 0.5f);
                juce::MidiBuffer midi;
                processor.processBlockBypassed(bypass, midi);
                require(bypass.getMagnitude(0, 64) == 0, "Send bypass leaked audio");
                processor.releaseResources();
            }
        }
        layout.inputBuses.set(0, juce::AudioChannelSet::stereo());
        layout.outputBuses.set(0, juce::AudioChannelSet::mono());
        require(!processor.isBusesLayoutSupported(layout), "Stereo-to-mono layout accepted");
        layout.inputBuses.set(0, juce::AudioChannelSet::discreteChannels(6));
        layout.outputBuses.set(0, juce::AudioChannelSet::discreteChannels(6));
        require(!processor.isBusesLayoutSupported(layout), "Surround layout accepted");
        for (const auto* id : {"room", "earlyLate", "decay", "damping", "width", "output"})
            parameter(processor, id, processor.parameters.getParameter(id)->convertFrom0to1(0.73f));
        juce::MemoryBlock state;
        processor.getStateInformation(state);
        SpatialLabProcessor restored;
        restored.setStateInformation(state.getData(), static_cast<int>(state.getSize()));
        for (const auto* id : {"room", "earlyLate", "decay", "damping", "width", "output"})
            require(std::abs(processor.parameters.getRawParameterValue(id)->load()
                - restored.parameters.getRawParameterValue(id)->load()) < 1e-5f, "Session recall mismatch");
        require(restored.parameters.getParameter("mix") == nullptr, "Obsolete dry/wet parameter is exposed");
        // Schema 1 migration ignores insert mix but retains gain and other wet controls.
        auto legacy = processor.parameters.copyState();
        legacy.setProperty("schemaVersion", 1, nullptr);
        auto balanceChild = legacy.getChildWithProperty("id", "earlyLate");
        legacy.removeChild(balanceChild, nullptr);
        juce::ValueTree oldMix("PARAM");
        oldMix.setProperty("id", "mix", nullptr);
        oldMix.setProperty("value", 0.0f, nullptr);
        legacy.appendChild(oldMix, nullptr);
        juce::MemoryBlock legacyData;
        if (auto xml = legacy.createXml()) juce::AudioProcessor::copyXmlToBinary(*xml, legacyData);
        restored.setStateInformation(legacyData.getData(), static_cast<int>(legacyData.getSize()));
        require(restored.parameters.getRawParameterValue("earlyLate")->load() == 0.5f, "Legacy balance not initialized");
        require(restored.parameters.getRawParameterValue("output")->load()
            == processor.parameters.getRawParameterValue("output")->load(), "Legacy wet gain lost");
        restored.prepareToPlay(48000, 64);
        juce::AudioBuffer<float> impulse(2, 1);
        impulse.clear(); impulse.setSample(0, 0, 1);
        juce::MidiBuffer impulseMidi;
        restored.processBlock(impulse, impulseMidi);
        require(impulse.getMagnitude(0, 1) == 0, "Legacy mix=0 restored a dry path");
        const auto before = restored.parameters.getRawParameterValue("earlyLate")->load();
        parameter(processor, "earlyLate", 0.1f);
        auto future = processor.parameters.copyState();
        future.setProperty("schemaVersion", 99, nullptr);
        juce::MemoryBlock futureState;
        if (auto xml = future.createXml()) juce::AudioProcessor::copyXmlToBinary(*xml, futureState);
        restored.setStateInformation(futureState.getData(), static_cast<int>(futureState.getSize()));
        require(restored.parameters.getRawParameterValue("earlyLate")->load() == before, "Unknown state schema changed parameters");
        const char invalid[] = "not a plugin state";
        restored.setStateInformation(invalid, sizeof(invalid));
        require(restored.parameters.getRawParameterValue("earlyLate")->load() == before, "Invalid state changed parameters");
        std::cout << "PASS bus layouts, mono/stereo routing, zero/oversized blocks, 44.1/48 kHz, wet-only, send bypass, session recall and v1 migration\n";
    }
    catch (const std::exception& e) { std::cerr << e.what() << '\n'; return 1; }
}
