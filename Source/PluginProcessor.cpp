#include "PluginProcessor.h"
#include "PluginEditor.h"

SpatialLabProcessor::SpatialLabProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                     .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    static_assert(std::atomic<float>::is_always_lock_free && std::atomic<unsigned>::is_always_lock_free
                  && std::atomic<int>::is_always_lock_free, "Audio controls require lock-free atomics");
    size = new juce::AudioParameterFloat(juce::ParameterID{"size", 1}, juce::String::fromUTF8("Größe"), juce::NormalisableRange<float>{0.0f, 1.0f}, 0.5f);
    tone = new juce::AudioParameterFloat(juce::ParameterID{"tone", 1}, "Klang", juce::NormalisableRange<float>{0.0f, 1.0f}, 0.5f);
    room = new RoomParameter(*size, *tone, resetRevision);
    addParameter(room); addParameter(size); addParameter(tone);
    // Polling avoids AsyncUpdater/OS-message calls from an automated room change.
    startTimerHz(30);
}

juce::RangedAudioParameter* SpatialLabProcessor::parameter(const juce::String& id) const
{
    if (id == "room") return room;
    if (id == "size") return size;
    if (id == "tone") return tone;
    return nullptr;
}

void SpatialLabProcessor::timerCallback()
{
    const auto revision = resetRevision.load(std::memory_order_acquire);
    if (revision == notifiedRevision) return;
    notifiedRevision = revision;
    // Report current values, not a queued neutral value that could overwrite later automation.
    size->sendValueChangedMessageToListeners(size->get());
    tone->sendValueChangedMessageToListeners(tone->get());
}

bool SpatialLabProcessor::isBusesLayoutSupported(const BusesLayout& layout) const
{
    const auto in = layout.getMainInputChannelSet(), out = layout.getMainOutputChannelSet();
    return (in == juce::AudioChannelSet::mono() && (out == juce::AudioChannelSet::mono() || out == juce::AudioChannelSet::stereo()))
        || (in == juce::AudioChannelSet::stereo() && out == juce::AudioChannelSet::stereo());
}

void SpatialLabProcessor::prepareToPlay(double rate, int)
{
    engine.prepare(rate, room->getIndex(), size->get(), tone->get());
    audioRevision = resetRevision.load(std::memory_order_acquire);
    gui.sampleRate.store(static_cast<float>(rate), std::memory_order_relaxed);
    gui.inputs.store(getTotalNumInputChannels(), std::memory_order_relaxed);
    gui.outputs.store(getTotalNumOutputChannels(), std::memory_order_relaxed);
    setLatencySamples(0);
}

void SpatialLabProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const auto revision = resetRevision.load(std::memory_order_acquire);
    engine.set(room->getIndex(), size->get(), tone->get(), revision != audioRevision);
    audioRevision = revision;
    const int inputs = getTotalNumInputChannels(), outputs = getTotalNumOutputChannels();
    auto* left = buffer.getWritePointer(0);
    auto* right = outputs == 2 ? buffer.getWritePointer(1) : nullptr;
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        const float l = left[sample];
        const float r = inputs == 2 ? right[sample] : l;
        const auto value = engine.process(l, r);
        left[sample] = outputs == 1 ? (value.left + value.right) * 0.5f : value.left;
        if (right != nullptr) right[sample] = value.right;
    }
    if (gui.observing.load(std::memory_order_relaxed))
        gui.publish(buffer.getMagnitude(0, buffer.getNumSamples()), inputs, outputs);
}

void SpatialLabProcessor::processBlockBypassed(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    processBlock(buffer, midi);
    buffer.clear();
}

juce::AudioProcessorEditor* SpatialLabProcessor::createEditor() { return new SpatialLabEditor(*this); }

void SpatialLabProcessor::getStateInformation(juce::MemoryBlock& destination)
{
    juce::XmlElement state("SpatialLabState");
    state.setAttribute("schemaVersion", 5);
    state.setAttribute("room", room->getIndex());
    state.setAttribute("environmentKey",spatiallab::roomKeys[static_cast<std::size_t>(room->getIndex())]);
    state.setAttribute("size", static_cast<double>(size->get()));
    state.setAttribute("tone", static_cast<double>(tone->get()));
    copyXmlToBinary(state, destination);
}

void SpatialLabProcessor::setStateInformation(const void* data, int length)
{
    auto xml = getXmlFromBinary(data, length);
    if (xml == nullptr || !xml->hasTagName("SpatialLabState")) return;
    const int version = xml->getIntAttribute("schemaVersion");
    if (version != 5) return; // Development reset: previous test sessions intentionally unsupported.
    int roomIndex=-1;
    const auto key=xml->getStringAttribute("environmentKey");
    for(int i=0;i<spatiallab::roomCount;++i)if(key==spatiallab::roomKeys[i])roomIndex=i;
    const double sizeValue=xml->getDoubleAttribute("size",.5),toneValue=xml->getDoubleAttribute("tone",.5);
    if(roomIndex<0 || !std::isfinite(sizeValue)||!std::isfinite(toneValue)||sizeValue<0||sizeValue>1||toneValue<0||toneValue>1)return;
    // Restore room first (which resets macros), then the saved session offsets.
    room->setValueNotifyingHost(room->convertTo0to1(static_cast<float>(roomIndex)));
    size->setValueNotifyingHost(static_cast<float>(sizeValue));
    tone->setValueNotifyingHost(static_cast<float>(toneValue));
    resetRevision.fetch_add(1, std::memory_order_release);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new SpatialLabProcessor(); }
