#include "PluginProcessor.h"

juce::AudioProcessorValueTreeState::ParameterLayout SpatialLabProcessor::makeParameters()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"room", 1}, "Room",
        juce::StringArray{"Living Room", "Kitchen", "Bathroom"}, 0));
    const auto add = [&layout](const char* id, const char* name, float lo, float hi, float initial)
    {
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{id, 1}, name,
            juce::NormalisableRange<float>{lo, hi}, initial));
    };
    add("earlyLate", "Early / Late Balance", 0.0f, 1.0f, 0.5f);
    add("decay", "Decay multiplier", 0.5f, 2.0f, 1.0f);
    add("damping", "Damping", 0.0f, 1.0f, 0.5f);
    add("width", "Stereo width", 0.0f, 1.0f, 1.0f);
    add("output", "Wet Output Level (dB)", -24.0f, 6.0f, 0.0f);
    return layout;
}

SpatialLabProcessor::SpatialLabProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "SpatialLabState", makeParameters()),
      room(parameters.getRawParameterValue("room")), earlyLate(parameters.getRawParameterValue("earlyLate")),
      decay(parameters.getRawParameterValue("decay")), damping(parameters.getRawParameterValue("damping")),
      width(parameters.getRawParameterValue("width")), output(parameters.getRawParameterValue("output"))
{
    static_assert(std::atomic<float>::is_always_lock_free, "Audio parameters require lock-free floats");
}

bool SpatialLabProcessor::isBusesLayoutSupported(const BusesLayout& layout) const
{
    const auto in = layout.getMainInputChannelSet(), out = layout.getMainOutputChannelSet();
    return (in == juce::AudioChannelSet::mono() && (out == juce::AudioChannelSet::mono() || out == juce::AudioChannelSet::stereo()))
        || (in == juce::AudioChannelSet::stereo() && out == juce::AudioChannelSet::stereo());
}

void SpatialLabProcessor::prepareToPlay(double rate, int)
{
    engine.prepare(rate, juce::roundToInt(room->load()), earlyLate->load(), decay->load(), damping->load(),
        width->load(), juce::Decibels::decibelsToGain(output->load()));
    setLatencySamples(0);
}

void SpatialLabProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    engine.set(juce::roundToInt(room->load()), earlyLate->load(), decay->load(), damping->load(), width->load(),
        juce::Decibels::decibelsToGain(output->load()));
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
}

void SpatialLabProcessor::processBlockBypassed(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    // A send must never add another direct signal when the host invokes plugin bypass.
    processBlock(buffer, midi);
    buffer.clear();
}

juce::AudioProcessorEditor* SpatialLabProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}

void SpatialLabProcessor::getStateInformation(juce::MemoryBlock& destination)
{
    auto state = parameters.copyState();
    state.setProperty("schemaVersion", 2, nullptr);
    if (auto xml = state.createXml()) copyXmlToBinary(*xml, destination);
}

void SpatialLabProcessor::setStateInformation(const void* data, int size)
{
    auto xml = getXmlFromBinary(data, size);
    if (xml == nullptr || !xml->hasTagName(parameters.state.getType())) return;
    const int version = xml->getIntAttribute("schemaVersion");
    if (version != 1 && version != 2) return;
    auto state = juce::ValueTree::fromXml(*xml);
    if (version == 1)
    {
        // Preserve existing wet controls, discard the obsolete insert mix value.
        for (int i = state.getNumChildren(); --i >= 0;)
            if (state.getChild(i).getProperty("id").toString() == "mix")
                state.removeChild(i, nullptr);
        juce::ValueTree balanceState("PARAM");
        balanceState.setProperty("id", "earlyLate", nullptr);
        balanceState.setProperty("value", 0.5f, nullptr);
        state.appendChild(balanceState, nullptr);
        state.setProperty("schemaVersion", 2, nullptr);
    }
    parameters.replaceState(state);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new SpatialLabProcessor(); }
