#pragma once
#include <juce_data_structures/juce_data_structures.h>

namespace spatiallab::gui
{
inline juce::PropertiesFile::Options preferenceOptions()
{
    juce::PropertiesFile::Options options;
    options.applicationName = "SpatialLab";
    options.filenameSuffix = ".gui.settings";
    options.folderName = "Stromschlag Audio";
    options.osxLibrarySubFolder = "Application Support";
    options.millisecondsBeforeSaving = 0;
    return options;
}
inline void migrateEpisode(juce::PropertiesFile& current, const juce::File& legacyFile)
{
    if (current.containsKey("episodeIV") || !legacyFile.existsAsFile()) return;
    juce::PropertiesFile legacy(legacyFile, preferenceOptions());
    if (legacy.containsKey("episodeIV"))
    {
        current.setValue("episodeIV", legacy.getBoolValue("episodeIV", true));
        current.saveIfNeeded();
    }
}
inline std::unique_ptr<juce::PropertiesFile> openPreferences(const juce::File& overrideFile = {})
{
    const auto options = preferenceOptions();
    auto current = overrideFile == juce::File() ? std::make_unique<juce::PropertiesFile>(options)
        : std::make_unique<juce::PropertiesFile>(overrideFile, options);
    if (overrideFile == juce::File())
    {
        auto legacyOptions = options;
        // Historical path only: never rename identity codes or overwrite legacy preferences.
        legacyOptions.folderName = "Stromschlag Productions";
        migrateEpisode(*current, legacyOptions.getDefaultFile());
    }
    return current;
}
}
