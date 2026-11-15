# SpatialLab

**Spatial Environments for Audio Drama** · Stromschlag Productions

An initial Windows x64 VST3 room effect built with C++17, JUCE **8.0.6** and CMake. Living Room, Kitchen and Bathroom combine distinct early reflections with a damped eight-line feedback delay network. Supports mono → mono, mono → stereo and stereo → stereo, host automation, session recall and 100% wet sends.

This is a development prototype, not a release. See [validation](docs/VALIDATION.md) for tested behavior and remaining listening/host checks. Work takes place on development branches and pull requests; do not merge without review.

## Build on Windows

Install Visual Studio 2022 with **Desktop development with C++**, a Windows SDK, Git and CMake 3.31.6 (minimum supported: 3.22).

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DBUILD_TESTING=ON
cmake --build build --config Release --parallel 2
ctest --test-dir build -C Release --output-on-failure
```

The first configure downloads JUCE at the immutable commit in `CMakeLists.txt`. The VST3 SDK is supplied by JUCE; no separate SDK checkout is needed. The plugin bundle is `build/SpatialLab_artefacts/Release/VST3/SpatialLab.vst3`. Copy the **entire bundle directory** to `C:\Program Files\Common Files\VST3\` and rescan the DAW. Builds never install automatically.

GitHub Actions builds on `windows-2022` with VS 2022 x64, runs DSP and processor tests, and uploads the complete VST3 bundle plus build provenance. The Linux job runs the independent DSP tests under AddressSanitizer and UndefinedBehaviorSanitizer. JUCE and Actions are pinned to commit hashes; CMake is version-pinned. Hosted runner/SDK updates mean this is a repeatable build recipe, not a promise of byte-identical binaries. No signing, installer or automatic release is configured.

For DSP-only development without JUCE or OS audio/GUI dependencies:

```sh
cmake -S . -B build-dsp -DSPATIALLAB_BUILD_PLUGIN=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build-dsp --parallel 2
ctest --test-dir build-dsp --output-on-failure
```

## Operation

The first UI uses JUCE's generic parameter editor. Select a room, then adjust Dry / Wet (0–1), Decay multiplier (0.5–2), Damping (0–1), Stereo width (0–1) and Output (-24 to +6 dB). For a send bus use Dry / Wet = **1**. Increasing damping reduces high-frequency decay. Width affects only the wet signal; zero folds the wet field to mono. Output affects both dry and wet. No limiter or automatic gain normalization is applied: leave headroom.

Living Room starts with a short, dark tail (nominal low-frequency RT60 0.48 s), Kitchen is brighter (0.85 s), Bathroom has a longer reflective tail (1.35 s). These are designed room impressions, not measured acoustic models. The reported host tail is conservatively 6 s; latency is zero. Read [DSP and parameter design](docs/DESIGN.md) before extending the engine or changing parameter IDs.

## Roadmap and licensing

Custom Light/Dark/Retro skins, additional interiors and a separate outdoor reflection engine are deferred. Appearance must never change audio behavior.

The project's distribution license remains undecided. JUCE is third-party software with its own AGPLv3/commercial licensing requirements; review the JUCE license and bundled VST3 SDK notices before distributing any binary. No third-party DSP code, impulse responses or artwork have been copied into the engine.
