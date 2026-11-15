# SpatialLab

**Spatial Environments for Audio Drama**  
Stromschlag Audio / Stromschlag Productions

SpatialLab is a free, noncommercially motivated VST3 spatial-effects plugin for audio-drama production. Development prioritizes sound quality, reliable real-time DSP, and fast operation.

## Planned development

- Windows VST3, C++17, JUCE 8, CMake and GitHub Actions.
- Initial test rooms: Living Room, Kitchen and Bathroom; more indoor environments later.
- Dedicated outdoor reflection/echo engine in a later phase.
- Light, Dark and weathered Retro hardware GUI skins. Weathering changes appearance only, not sound.
- Support for 100% wet FX-bus workflows, automation, session recall and smooth parameter changes.

## Status

**Early development.** This README establishes the project. No compiled plugin or verified DSP build is included yet.

## Quality rules

Changes should be reviewed and tested; no audio-thread allocation, blocking or locks; verify mono/stereo behavior, sample-rate changes, stability and audible artifacts. Document known limitations. No release before successful builds and listening tests.

## License

To be determined before publication. Do not assume third-party DSP code or assets can be redistributed without reviewing their licenses.
