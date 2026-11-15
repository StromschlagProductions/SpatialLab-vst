#include "PluginProcessor.h"
#include <iostream>
#include <stdexcept>
#include <thread>

static void require(bool value, const char* message) { if (!value) throw std::runtime_error(message); }
static void parameter(SpatialLabProcessor& p, const char* id, float value)
{
    auto* v = p.parameter(id);
    v->setValueNotifyingHost(v->convertTo0to1(value));
}
static void restoreXml(SpatialLabProcessor& p, const juce::XmlElement& xml)
{
    juce::MemoryBlock data;
    juce::AudioProcessor::copyXmlToBinary(xml, data);
    p.setStateInformation(data.getData(), static_cast<int>(data.getSize()));
}
int main()
{
    // Timers/editor and deferred host notifications use the JUCE message thread.
    juce::ScopedJuceInitialiser_GUI juce;
    try
    {
        SpatialLabProcessor processor;
        require(processor.getParameters().size() == 3, "Exactly three audio parameters required");
        for (const auto* id : {"mix", "earlyLate", "decay", "damping", "width", "output"})
            require(processor.parameter(id) == nullptr, "Obsolete control is exposed");
        auto layout = processor.getBusesLayout();
        for (const auto channels : {std::pair<int, int>{1, 1}, {1, 2}, {2, 2}})
        {
            layout.inputBuses.set(0, juce::AudioChannelSet::canonicalChannelSet(channels.first));
            layout.outputBuses.set(0, juce::AudioChannelSet::canonicalChannelSet(channels.second));
            require(processor.setBusesLayout(layout), "Supported layout rejected");
            for (double rate : {44100.0, 48000.0})
                for (int preset = 0; preset < 3; ++preset)
                {
                    parameter(processor, "room", static_cast<float>(preset));
                    processor.prepareToPlay(rate, 64);
                    spatiallab::Engine reference;
                    reference.prepare(rate, preset, 0.5f, 0.5f);
                    int frame = 0;
                    for (int count : {0, 1, 7, 64, 511, 2048})
                    {
                        juce::AudioBuffer<float> buffer(channels.second, count);
                        for (int i = 0; i < count; ++i)
                        {
                            buffer.setSample(0, i, frame % 97 == 0 ? 0.25f : 0);
                            if (channels.second == 2) buffer.setSample(1, i,
                                channels.first == 2 ? (frame % 83 == 0 ? -0.5f : 0) : 99.0f);
                            ++frame;
                        }
                        juce::AudioBuffer<float> input; input.makeCopyOf(buffer);
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
                            if (frame - count + i == 0) require(buffer.getSample(0, i) == 0, "Direct impulse leaked");
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
        require(!processor.isBusesLayoutSupported(layout), "Stereo-to-mono accepted");
        layout.inputBuses.set(0, juce::AudioChannelSet::discreteChannels(6));
        layout.outputBuses.set(0, juce::AudioChannelSet::discreteChannels(6));
        require(!processor.isBusesLayoutSupported(layout), "Surround accepted");
        // An actual host-thread parameter change must reset both macros immediately,
        // even without an open editor or message-loop dispatch.
        for (int preset : {0, 1, 2, 0})
        {
            parameter(processor, "size", 0.9f); parameter(processor, "tone", 0.1f);
            std::thread host([&] { parameter(processor, "room", static_cast<float>(preset)); });
            host.join();
            require(processor.parameter("size")->getValue() == 0.5f && processor.parameter("tone")->getValue() == 0.5f,
                "Host room change did not reset macros");
        }
        parameter(processor, "size", 0.73f); parameter(processor, "tone", 0.27f);
        parameter(processor, "room", 0); // Repeated same preset is not a preset change.
        require(processor.parameter("size")->getValue() == 0.73f, "Unchanged room reset automation");
        juce::MemoryBlock state;
        processor.getStateInformation(state);
        SpatialLabProcessor restored;
        parameter(restored, "room", 2);
        restored.setStateInformation(state.getData(), static_cast<int>(state.getSize()));
        for (const auto* id : {"room", "size", "tone"})
            require(processor.parameter(id)->getValue() == restored.parameter(id)->getValue(), "Session recall mismatch");
        // Old experimental controls never return, even when loading an old insert state.
        for (int version : {1, 2})
        {
            juce::XmlElement legacy("SpatialLabState"); legacy.setAttribute("schemaVersion", version);
            for (const auto* id : {"room", "mix", "earlyLate", "output", "width"})
            {
                auto* child = legacy.createNewChildElement("PARAM"); child->setAttribute("id", id);
                child->setAttribute("value", juce::String(id) == "room" ? 1.0 : 0.0);
            }
            restoreXml(restored, legacy);
            require(restored.parameter("room")->convertFrom0to1(restored.parameter("room")->getValue()) == 1
                && restored.parameter("size")->getValue() == 0.5f && restored.parameter("tone")->getValue() == 0.5f,
                "Legacy state did not migrate to neutral preset");
        }
        juce::XmlElement future("SpatialLabState"); future.setAttribute("schemaVersion", 99);
        restoreXml(restored, future);
        const char invalid[] = "not a plugin state";
        restored.setStateInformation(invalid, sizeof(invalid));
        require(restored.parameter("room")->convertFrom0to1(restored.parameter("room")->getValue()) == 1,
            "Rejected state changed room");
        juce::XmlElement invalidValue("SpatialLabState"); invalidValue.setAttribute("schemaVersion", 3);
        invalidValue.setAttribute("room", 0); invalidValue.setAttribute("size", 4.0);
        restoreXml(restored, invalidValue);
        require(restored.parameter("room")->convertFrom0to1(restored.parameter("room")->getValue()) == 1,
            "Invalid macro state changed room");
        // Editor smoke test: build and lay out the exact three-control surface.
        std::unique_ptr<juce::AudioProcessorEditor> editor(restored.createEditor());
        editor->setSize(420, 250);
        std::cout << "PASS three controls, preset reset, wet buses/blocks, 44.1/48 kHz, bypass, schema 1/2/3, editor\n";
    }
    catch (const std::exception& e) { std::cerr << e.what() << '\n'; return 1; }
}
