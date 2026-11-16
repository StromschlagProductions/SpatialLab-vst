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
juce::MouseEvent eventAt(juce::Component& component, juce::Point<float> point, bool pressed = false)
{
    const auto now = juce::Time::getCurrentTime();
    return {juce::Desktop::getInstance().getMainMouseSource(),point,
        juce::ModifierKeys(pressed ? juce::ModifierKeys::leftButtonModifier : 0),
        0,0,0,0,0,&component,&component,now,point,now,1,false};
}
void click(PresetBrowser& browser, juce::Rectangle<float> row)
{
    require(!row.isEmpty(),"Missing browser row");
    const auto event = eventAt(browser,row.getCentre(),true);
    browser.mouseDown(event); browser.mouseUp(event);
}
void wheel(PresetBrowser& browser, float delta)
{
    juce::MouseWheelDetails details {}; details.deltaY = delta;
    browser.mouseWheelMove(eventAt(browser,browser.getLocalBounds().toFloat().getCentre()),details);
}
void requireVisible(PresetBrowser& browser, int room)
{
    const auto row = browser.roomBounds(room);
    require(!row.isEmpty() && row.getY() >= -.01f && row.getBottom() <= browser.getHeight()+.01f,
            "Active preset not completely visible");
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
                editor->removeFromDesktop();
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
                auto* browser = dynamic_cast<PresetBrowser*>(editor->findChildWithID("browser"));
                // Find by component type/name, without adding plugin parameters or test-only controls.
                if (browser == nullptr) for (auto* child : editor->getChildren())
                    if (auto* candidate = dynamic_cast<PresetBrowser*>(child)) browser = candidate;
                require(browser != nullptr,"Central preset browser missing");
                editor->setSize(1504,846); editor->addToDesktop(0); editor->setVisible(true);
                editor->visibilityChanged();
                for (auto dimensions : {std::pair<int,int>{1003,565},{1504,846},{1672,941}})
                {
                    editor->setSize(dimensions.first,dimensions.second);
                    const float scale = dimensions.first/1672.0f;
                    require(std::abs(browser->getX()-542*scale)<2 && std::abs(browser->getY()-322*scale)<2,
                            "Browser escaped the central CRT");
                    for(int room=0;room<19;++room)
                    {
                        browser->revealRoom(room);
                        set(observed,"size",.9f); set(observed,"tone",.1f);
                        // Start the first loop from a different room, too.
                        if(browser->getSelectedRoom()==room) { set(observed,"room",float((room+1)%19)); editor->visibilityChanged(); browser->revealRoom(room); }
                        click(*browser,browser->roomBounds(room));
                        require(browser->getSelectedRoom()==room &&
                            juce::roundToInt(observed.parameter("room")->convertFrom0to1(observed.parameter("room")->getValue()))==room,
                            "Browser selected the wrong semantic room");
                        require(observed.parameter("size")->getValue()==.5f && observed.parameter("tone")->getValue()==.5f,
                            "Direct selection lost the existing neutral macro reset");
                        requireVisible(*browser,room);
                    }
                }
                const float retained = browser->getScrollOffset();
                click(*browser,browser->roomBounds(17));
                require(browser->getScrollOffset()==retained,"Nearby choice jumped to list start");
                set(observed,"size",.7f); set(observed,"tone",.3f);
                observed.getStateInformation(before);
                click(*browser,browser->roomBounds(17));
                observed.getStateInformation(after); require(before==after,"Repeated active-row click changed audio state");
                wheel(*browser,.1f);
                const float manual = browser->getScrollOffset(); editor->visibilityChanged();
                require(browser->getScrollOffset()==manual,"Unchanged room undid manual scrolling");
                observed.getStateInformation(after); require(before==after,"Browser wheel changed audio state");
                wheel(*browser,1000); require(browser->getScrollOffset()==0,"Scroll upper clamp failed");
                wheel(*browser,-1000); const float maximum = browser->getScrollOffset();
                require(maximum>0,"Wheel did not reach the remaining categories");
                wheel(*browser,1000);
                browser->mouseDown(eventAt(*browser,{float(browser->getWidth()-3),float(browser->getHeight()/2)},true));
                browser->mouseDrag(eventAt(*browser,{float(browser->getWidth()-3),float(browser->getHeight())},true));
                browser->mouseUp(eventAt(*browser,{float(browser->getWidth()-3),float(browser->getHeight())}));
                require(browser->getScrollOffset()>0 && browser->getScrollOffset()<=maximum,"Scrollbar drag out of range");
                wheel(*browser,1000);
                click(*browser,browser->categoryBounds(0));
                require(!browser->isExpanded(0) && browser->roomBounds(0).isEmpty(),"Category did not collapse");
                click(*browser,browser->categoryBounds(3));
                require(!browser->isExpanded(3),"Sci-Fi did not collapse");
                observed.getStateInformation(after); require(before==after,"Category click changed audio state");
                set(observed,"room",16); editor->visibilityChanged();
                require(browser->isExpanded(3) && browser->getSelectedRoom()==16,"Host automation did not reveal its category");
                requireVisible(*browser,16);
                observed.getStateInformation(before);
                click(*browser,browser->categoryBounds(3));
                observed.setStateInformation(before.getData(),static_cast<int>(before.getSize())); editor->visibilityChanged();
                require(browser->isExpanded(3),"Same-room session recall did not reopen category");
                requireVisible(*browser,16);
                // Scrolling over the browser reaches only the browser; knob wheels still work.
                auto point=browser->localPointToGlobal(browser->getLocalBounds().getCentre());
                require(editor->getComponentAt(editor->getLocalPoint(nullptr,point))==browser,"Browser hit target overlaps a knob");
                for(auto* child : editor->getChildren()) if(auto* slider=dynamic_cast<juce::Slider*>(child))
                {
                    const double previous=slider->getValue(); juce::MouseWheelDetails details {}; details.deltaY=.2f;
                    slider->mouseWheelMove(eventAt(*slider,slider->getLocalBounds().toFloat().getCentre()),details);
                    require(slider->getValue()!=previous,"Knob wheel stopped working");
                }
                for(auto* child:editor->getChildren()) if(auto* button=dynamic_cast<juce::TextButton*>(child))
                    if(button->getName()=="Raum nach oben") { button->onClick(); require(browser->getSelectedRoom()==15,"Alternative arrows lost room mapping"); }
                editor->setVisible(false); editor->removeFromDesktop();
                editor->setSize(1003,565);
                for (bool skin : {false,true})
                {
                    editor->setEpisodeIV(skin,false);
                    std::array<std::uint64_t,19> fingerprints {};
                    for (int roomIndex = 0; roomIndex < 19; ++roomIndex)
                    {
                        set(observed,"room",static_cast<float>(roomIndex));
                        const auto image = editor->createComponentSnapshot(editor->getLocalBounds());
                        auto hash = std::uint64_t{1469598103934665603ull};
                        // Furniture region only: exclude the changing room-name heading.
                        for (int y = 194; y < 251; ++y) for (int x = 92; x < 223; ++x)
                        { hash ^= image.getPixelAt(x,y).getARGB(); hash *= 1099511628211ull; }
                        fingerprints[static_cast<std::size_t>(roomIndex)] = hash;
                    }
                    for(int a=0;a<19;++a)for(int b=a+1;b<19;++b)require(fingerprints[a]!=fingerprints[b],"Duplicate CRT geometry");
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
        std::cout << "PASS central browser direct choice, category/wheel/host/recall/scale, small 19-room drawings, unchanged audio/lifecycle/skins\n";
    }
    catch (const std::exception& e) { std::cerr << e.what() << '\n'; return 1; }
}
