#include "PluginEditor.h"
#include "GuiPreferences.h"
#include <iostream>
#include <stdexcept>
#include <chrono>

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
        {
            juce::TemporaryFile oldFile(".gui.settings"), newFile(".gui.settings");
            juce::PropertiesFile legacy(oldFile.getFile(), spatiallab::gui::preferenceOptions());
            legacy.setValue("episodeIV", false); legacy.saveIfNeeded();
            const auto original = oldFile.getFile().loadFileAsString();
            juce::PropertiesFile migrated(newFile.getFile(), spatiallab::gui::preferenceOptions());
            spatiallab::gui::migrateEpisode(migrated, oldFile.getFile());
            require(migrated.containsKey("episodeIV") && !migrated.getBoolValue("episodeIV", true), "Legacy skin lost");
            migrated.setValue("episodeIV", true);
            spatiallab::gui::migrateEpisode(migrated, oldFile.getFile());
            require(migrated.getBoolValue("episodeIV", false), "Migration overwrote current preference");
            require(original == oldFile.getFile().loadFileAsString(), "Migration changed legacy file");
        }
        {
            std::vector<std::unique_ptr<SpatialLabProcessor>> processors;
            for (int i=0;i<32;++i) { auto p=std::make_unique<SpatialLabProcessor>(); p->prepareToPlay(48000,128); processors.push_back(std::move(p)); }
            juce::AudioBuffer<float> audio(2,128); juce::MidiBuffer midi;
            const auto start=std::chrono::steady_clock::now();
            for (int block=0;block<375;++block) for (auto& p:processors)
            { audio.clear(); audio.setSample(0,0,0.1f); p->processBlock(audio,midi); }
            for (const auto& p:processors) require(!p->gui.observing.load() && p->gui.blocks.load()==0,"Closed instances still publish GUI telemetry");
            std::cout << "32 closed instances / 1s audio each: " << std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count() << " CPU-wall seconds; GUI telemetry zero\n";
        }
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
            juce::TemporaryFile preferences(".gui.settings");
            {
                auto editor = std::make_unique<SpatialLabEditor>(observed, preferences.getFile());
                require(editor->isEpisodeIV(), "Fresh GUI must default to Episode IV");
                require(editor->getWidth() == 1504 && editor->getHeight() == 846, "Wrong reference aspect/default size");
                require(!observed.gui.observing.load(), "Invisible editor observes audio");
                editor->addToDesktop(0);
                editor->setVisible(true);
                require(observed.gui.observing.load(), "Shown editor did not resume");
                editor->setVisible(false);
                require(!observed.gui.observing.load(), "Hidden editor did not pause");
                int knobs = 0;
                for (auto* child : editor->getChildren()) if (dynamic_cast<juce::Slider*>(child)) ++knobs;
                require(knobs == 2 && observed.getParameters().size() == 3, "Extra audio control");
                observed.getStateInformation(after);
                require(before == after, "Opening editor changed session state");
                juce::TextButton* toggle = nullptr;
                for (auto* child : editor->getChildren())
                    if (auto* button = dynamic_cast<juce::TextButton*>(child))
                    {
                        require(button->getName() != "Vorheriger Raum" && button->getName() != juce::String::fromUTF8("Nächster Raum"),
                                "Old lower preset path still exists");
                        if (button->getName() == "Episode") toggle = button;
                    }
                require(toggle != nullptr, "Episode toggle missing");
                for (bool episodeIV : {false, true, false})
                {
                    toggle->setToggleState(episodeIV, juce::dontSendNotification); toggle->onClick();
                    require(editor->isEpisodeIV() == episodeIV, "Toggle direction/state wrong");
                    observed.getStateInformation(after);
                    require(before == after, "Cosmetic toggle changed audio state");
                    juce::AudioBuffer<float> a(2, 64), b(2, 64); a.clear(); b.clear();
                    reference.processBlock(a, midi); observed.processBlock(b, midi);
                    for (int c = 0; c < 2; ++c) for (int i = 0; i < 64; ++i)
                        require(a.getSample(c, i) == b.getSample(c, i), "Episode selection changed audio");
                }
                for (auto dimensions : {std::pair<int,int>{1003,565}, {1504,846}, {1672,941}})
                {
                    editor->setSize(dimensions.first, dimensions.second);
                    const auto control = toggle->getBounds().toFloat();
                    const float scale = std::min(dimensions.first / 1672.0f, dimensions.second / 941.0f);
                    const float ox = (dimensions.first - scale * 1672) / 2, oy = (dimensions.second - scale * 941) / 2;
                    require(control.getX() >= ox + 1228 * scale && control.getRight() <= ox + 1505 * scale
                         && control.getY() >= oy + 744 * scale && control.getBottom() <= oy + 815 * scale,
                            "Switch escapes clear metal surface/rivets");
                }
                for (const auto& name : {juce::String::fromUTF8("Küche"), juce::String("Raum nach oben")})
                {
                    set(observed, "size", 0.9f); set(observed, "tone", 0.1f);
                    bool found = false;
                    for (auto* child : editor->getChildren())
                        if (auto* button = dynamic_cast<juce::TextButton*>(child); button != nullptr && button->getName() == name)
                        { button->onClick(); found = true; }
                    require(found && observed.parameter("size")->getValue() == 0.5f && observed.parameter("tone")->getValue() == 0.5f,
                            "Preset path not synchronized with existing room reset");
                }
                editor->setSize(1003,565);
                for (bool skin : {false,true})
                {
                    editor->setEpisodeIV(skin,false);
                    std::array<std::uint64_t,3> fingerprints {};
                    for (int roomIndex = 0; roomIndex < 3; ++roomIndex)
                    {
                        set(observed,"room",static_cast<float>(roomIndex));
                        const auto image = editor->createComponentSnapshot(editor->getLocalBounds());
                        auto hash = std::uint64_t{1469598103934665603ull};
                        // Furniture region only: exclude the changing room-name heading.
                        for (int y = 238; y < 315; y += 2) for (int x = 337; x < 574; x += 2)
                        { hash ^= image.getPixelAt(x,y).getARGB(); hash *= 1099511628211ull; }
                        fingerprints[static_cast<std::size_t>(roomIndex)] = hash;
                    }
                    require(fingerprints[0] != fingerprints[1] && fingerprints[1] != fingerprints[2]
                        && fingerprints[0] != fingerprints[2], "CRT room geometry not distinct in both skins");
                }
            }
            {
                SpatialLabEditor reopened(observed, preferences.getFile());
                require(reopened.getWidth() == 1003 && reopened.getHeight() == 565, "Window size not restored");
                require(!reopened.isEpisodeIV(), "Separate GUI preference not restored");
            }
            {
                juce::PropertiesFile bad(preferences.getFile(), spatiallab::gui::preferenceOptions());
                bad.setValue("windowWidth", 50000); bad.setValue("windowHeight", -1); bad.saveIfNeeded();
                SpatialLabEditor safe(observed, preferences.getFile());
                require(safe.getWidth() == 1504 && safe.getHeight() == 846, "Invalid size not rejected");
                observed.getStateInformation(after);
                // State may have changed only through the explicit preset tests above.
                juce::MemoryBlock stable; observed.getStateInformation(stable);
                safe.setSize(1672,941); safe.setEpisodeIV(true,false);
                observed.getStateInformation(after); require(after == stable,"Resize altered audio state");
            }
            require(!observed.gui.observing.load(), "Closed editor still observes audio");
            const auto blocks = observed.gui.blocks.load();
            juce::AudioBuffer<float> silence(2, 64); silence.clear(); observed.processBlock(silence, midi);
            require(observed.gui.blocks.load() == blocks, "Telemetry still updated with closed editor");
            observed.releaseResources(); require(observed.gui.sampleRate.load() == 0, "Stale ready status");
        }
        std::cout << "PASS retro controls, shared preset paths, unchanged state/audio, lock-free telemetry lifecycle, Episode I/IV switch/bounds/audio/persistence\n";
    }
    catch (const std::exception& e) { std::cerr << e.what() << '\n'; return 1; }
}
