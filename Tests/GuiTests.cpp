#include "PluginProcessor.h"
#include <iostream>
#include <stdexcept>

namespace
{
void require(bool condition, const char* message) { if (!condition) throw std::runtime_error(message); }
void set(SpatialLabProcessor& p, const char* id, float value)
{
    auto* parameter = p.parameter(id);
    parameter->setValueNotifyingHost(parameter->convertTo0to1(value));
}
}
int main()
{
    juce::ScopedJuceInitialiser_GUI initialise;
    try
    {
        for (double rate : {44100.0, 48000.0})
        {
            SpatialLabProcessor reference, observed;
            reference.prepareToPlay(rate, 64); observed.prepareToPlay(rate, 64);
            observed.gui.observing.store(true);
            juce::MidiBuffer midi;
            for (int block = 0; block < 100; ++block)
            {
                if (block % 25 == 0)
                {
                    for (auto* processor : {&reference, &observed})
                    { set(*processor, "room", static_cast<float>((block / 25) % 3)); set(*processor, "size", 0.37f); set(*processor, "tone", 0.64f); }
                }
                juce::AudioBuffer<float> a(2, 127), b(2, 127);
                for (int channel = 0; channel < 2; ++channel)
                    for (int i = 0; i < 127; ++i)
                    {
                        const float x = std::sin(static_cast<float>(block * 127 + i) * 0.09f) * (channel == 0 ? 0.3f : -0.21f);
                        a.setSample(channel, i, x); b.setSample(channel, i, x);
                    }
                reference.processBlock(a, midi); observed.processBlock(b, midi);
                for (int channel = 0; channel < 2; ++channel)
                    for (int i = 0; i < 127; ++i)
                        require(a.getSample(channel, i) == b.getSample(channel, i), "GUI observation changed audio bits");
            }
            require(observed.gui.peak.load() > 0 && observed.gui.blocks.load() == 100, "Missing telemetry");
            require(observed.gui.sampleRate.load() == static_cast<float>(rate), "Wrong sample-rate telemetry");
            require(observed.gui.inputs.load() == 2 && observed.gui.outputs.load() == 2, "Wrong channel telemetry");
            juce::MemoryBlock before, after;
            observed.getStateInformation(before);
            {
                std::unique_ptr<juce::AudioProcessorEditor> editor(observed.createEditor());
                require(editor->getWidth() == 1003 && editor->getHeight() == 565, "Wrong reference aspect/default size");
                int knobs = 0;
                for (auto* child : editor->getChildren()) if (dynamic_cast<juce::Slider*>(child)) ++knobs;
                require(knobs == 2 && observed.getParameters().size() == 3, "Extra audio control");
                observed.getStateInformation(after);
                require(before == after, "Opening editor changed session state");
                for (const auto& name : {juce::String::fromUTF8("Küche"), juce::String("Vorheriger Raum")})
                {
                    set(observed, "size", 0.9f); set(observed, "tone", 0.1f);
                    bool found = false;
                    for (auto* child : editor->getChildren())
                        if (auto* button = dynamic_cast<juce::TextButton*>(child); button != nullptr && button->getName() == name)
                        { button->onClick(); found = true; }
                    require(found && observed.parameter("size")->getValue() == 0.5f && observed.parameter("tone")->getValue() == 0.5f,
                            "Preset path not synchronized with existing room reset");
                }
            }
            require(!observed.gui.observing.load(), "Closed editor still observes audio");
            const auto blocks = observed.gui.blocks.load();
            juce::AudioBuffer<float> silence(2, 64); silence.clear(); observed.processBlock(silence, midi);
            require(observed.gui.blocks.load() == blocks, "Telemetry still updated with closed editor");
            observed.releaseResources(); require(observed.gui.sampleRate.load() == 0, "Stale ready status");
        }
        std::cout << "PASS retro controls, shared preset paths, unchanged state/audio, lock-free telemetry lifecycle\n";
    }
    catch (const std::exception& e) { std::cerr << e.what() << '\n'; return 1; }
}
