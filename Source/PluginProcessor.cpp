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
    add("mix", "Dry / Wet", 0.0f, 1.0f, 0.25f);
    add("decay", "Decay multiplier", 0.5f, 2.0f, 1.0f);
    add("damping", "Damping", 0.0f, 1.0f, 0.5f);
    add("width", "Stereo width", 0.0f, 1.0f, 1.0f);
    add("output", "Output (dB)", -24.0f, 6.0f, 0.0f);
    return layout;
}

SpatialLabProcessor::SpatialLabProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "SpatialLabState", makeParameters()),
      room(parameters.getRawParameterValue("room")), mix(parameters.getRawParameterValue("mix")),
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
    engine.prepare(rate, juce::roundToInt(room->load()), mix->load(), decay->load(), damping->load(),
        width->load(), juce::Decibels::decibelsToGain(output->load()));
    setLatencySamples(0);
}

void SpatialLabProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    engine.set(juce::roundToInt(room->load()), mix->load(), decay->load(), damping->load(), width->load(),
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

juce::AudioProcessorEditor* SpatialLabProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}

void SpatialLabProcessor::getStateInformation(juce::MemoryBlock& destination)
{
    auto state = parameters.copyState();
    state.setProperty("schemaVersion", 1, nullptr);
    if (auto xml = state.createXml()) copyXmlToBinary(*xml, destination);
}

void SpatialLabProcessor::setStateInformation(const void* data, int size)
{
    if (auto xml = getXmlFromBinary(data, size))
        if (xml->hasTagName(parameters.state.getType()) && xml->getIntAttribute("schemaVersion") == 1)
            parameters.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new SpatialLabProcessor(); }
