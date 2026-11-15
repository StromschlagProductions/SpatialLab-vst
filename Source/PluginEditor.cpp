#include "PluginEditor.h"

SpatialLabEditor::SpatialLabEditor(SpatialLabProcessor& p) : AudioProcessorEditor(p), processor(p)
{
    room.addItemList({"Living Room", "Kitchen", "Bathroom"}, 1);
    room.onChange = [this]
    {
        auto* param = processor.parameter("room");
        param->beginChangeGesture();
        param->setValueNotifyingHost(param->convertTo0to1(static_cast<float>(room.getSelectedId() - 1)));
        param->endChangeGesture();
        timerCallback();
    };
    const auto setup = [this](juce::Slider& slider)
    {
        slider.setRange(0, 1, 0);
        slider.setSliderStyle(juce::Slider::LinearHorizontal);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.setDoubleClickReturnValue(true, 0.5);
        slider.setPopupDisplayEnabled(false, false, nullptr);
        addAndMakeVisible(slider);
    };
    setup(size); setup(tone);
    sizeAttachment = std::make_unique<juce::SliderParameterAttachment>(*processor.parameter("size"), size);
    toneAttachment = std::make_unique<juce::SliderParameterAttachment>(*processor.parameter("tone"), tone);
    roomLabel.setText("Raum", juce::dontSendNotification);
    sizeLabel.setText(juce::String::fromUTF8("Größe   ·   kleiner — neutral — größer"), juce::dontSendNotification);
    toneLabel.setText(juce::String::fromUTF8("Klang   ·   dunkler — neutral — heller"), juce::dontSendNotification);
    for (auto* label : {&roomLabel, &sizeLabel, &toneLabel}) addAndMakeVisible(*label);
    addAndMakeVisible(room);
    setSize(420, 250);
    timerCallback(); startTimerHz(30);
}
void SpatialLabEditor::timerCallback()
{
    room.setSelectedId(1 + juce::roundToInt(processor.parameter("room")->convertFrom0to1(processor.parameter("room")->getValue())), juce::dontSendNotification);
    size.setValue(processor.parameter("size")->getValue(), juce::dontSendNotification);
    tone.setValue(processor.parameter("tone")->getValue(), juce::dontSendNotification);
}
void SpatialLabEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff20262b));
    g.setColour(juce::Colours::white); g.setFont(20.0f);
    g.drawText("SPATIAL LAB", 20, 12, getWidth() - 40, 28, juce::Justification::centredLeft);
}
void SpatialLabEditor::resized()
{
    roomLabel.setBounds(20, 52, 70, 28); room.setBounds(90, 52, getWidth() - 110, 28);
    sizeLabel.setBounds(20, 96, getWidth() - 40, 24); size.setBounds(20, 122, getWidth() - 40, 28);
    toneLabel.setBounds(20, 162, getWidth() - 40, 24); tone.setBounds(20, 188, getWidth() - 40, 28);
}
