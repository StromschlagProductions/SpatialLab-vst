#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

// Palette reference: StromschlagProductions/Locations-vst, PluginEditor.cpp.
struct ClassicTheme
{
    juce::Colour background, panel, accent, text, dim, line;
    static ClassicTheme forLight(bool light)
    {
        return light ? ClassicTheme{juce::Colour(0xffd5dbda),juce::Colour(0xffe8eae9),juce::Colour(0xfff5781a),juce::Colour(0xff233539),juce::Colour(0xff5d6e72),juce::Colour(0xffb5bfbe)}
                     : ClassicTheme{juce::Colour(0xff182326),juce::Colour(0xff233539),juce::Colour(0xfff5781a),juce::Colour(0xffe8ecec),juce::Colour(0xff8fa1a5),juce::Colour(0xff35494e)};
    }
};
