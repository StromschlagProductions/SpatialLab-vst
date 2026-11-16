# SpatialLab

**Spatial Environments for Audio Drama** · Stromschlag Audio

An initial Windows x64 VST3 **send-only** room effect built with C++17, JUCE **8.0.6** and CMake. Living Room, Kitchen and Bathroom combine distinct early reflections with a damped eight-line feedback delay network. The output is always **100% wet**. Supports mono → mono, mono → stereo and stereo → stereo, host automation and session recall. Development prioritizes **48 kHz for audio drama** and **44.1 kHz for music**. Higher rates have basic correctness coverage but are not tuning or CPU targets. No oversampling is used.

This is a development prototype, not a release. See [validation](docs/VALIDATION.md) for tested behavior and remaining listening/host checks. Work takes place on development branches and pull requests; do not merge without review.

## Build on Windows

Install Visual Studio 2022 with **Desktop development with C++**, a Windows SDK, Git and CMake 3.31.6 (minimum supported: 3.22).

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DBUILD_TESTING=ON
cmake --build build --config Release --parallel 2
ctest --test-dir build -C Release --output-on-failure
```

The first configure downloads JUCE at the immutable commit in `CMakeLists.txt`. The VST3 SDK is supplied by JUCE; no separate SDK checkout is needed. The plugin bundle is `build/SpatialLab_artefacts/Release/VST3/SpatialLab.vst3`. Copy the **entire bundle directory** to `C:\Program Files\Common Files\VST3\` and rescan the DAW. Builds never install automatically.

GitHub Actions builds on `windows-2022` with VS 2022 x64, runs DSP and processor tests, and uploads the complete VST3 bundle, build provenance and an informational CPU benchmark at both primary rates. The Linux job runs the independent DSP tests under AddressSanitizer and UndefinedBehaviorSanitizer. JUCE and Actions are pinned to commit hashes; CMake is version-pinned. Hosted runner/SDK updates mean this is a repeatable build recipe, not a promise of byte-identical binaries. No signing, installer or automatic release is configured.

For DSP-only development without JUCE or OS audio/GUI dependencies:

```sh
cmake -S . -B build-dsp -DSPATIALLAB_BUILD_PLUGIN=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build-dsp --parallel 2
ctest --test-dir build-dsp --output-on-failure
./build-dsp/SpatialLabDspBenchmark
```

## Operation

The editor exposes exactly **Raum**, **Größe** (smaller → neutral → larger) and **Klang** (darker → neutral → brighter). Select a preset and set its send level in the DAW. Changing presets resets both macros to their neutral midpoint, including through host automation. Double-click either macro to reset it manually. No other audio controls or hidden expert automation parameters are present.

The output is always **100% wet**; there is no direct path, output gain or loudness knob. Early reflections, tail, diffusion, delays, damping and stereo voicing are authored internally by the preset. Size changes decay and internal early/late voicing, while Tone changes spectral damping. Room selection replaces the room and clears the previous tail; there are no room blends, distance or movement simulations. Locations handles scene transitions separately.

When the host invokes the plugin bypass callback, the send is muted while processing continues. Hosts that route around the plugin control their own external path; mute the return in those hosts. Schema-1/2 prototype sessions restore the selected room with neutral macros; obsolete extra controls are discarded.
Die vom Auftraggeber nach dem Sprachhörtest freigegebene Phase-2-Abstimmung bildet kompakte Sprachräume ab: Wohnzimmer mit kräftigerer früher Raumantwort (nominale tieffrequente RT60 0,40 s), eine kurze, hellere Küche (0,32 s) und ein kleines gekacheltes Bad (0,38 s). Dies sind entworfene Raumeindrücke, keine gemessenen Akustikmodelle. Phase 2.1 lässt diese Akustik unverändert. The reported host tail is conservatively 6 s; latency is zero. Read [DSP and parameter design](docs/DESIGN.md) before extending the engine or changing parameter IDs.

## Roadmap and licensing

Die Retro-Oberfläche ist im separaten GUI-Branch implementiert. Light-/Dark-Skins, weitere Innenräume und kuratierte Outdoor-Presets folgen später; alle verwenden dasselbe Drei-Regler-Konzept. Appearance must never change audio behavior.

The project's distribution license remains undecided. JUCE is third-party software with its own AGPLv3/commercial licensing requirements; review the JUCE license and bundled VST3 SDK notices before distributing any binary. No third-party DSP code, impulse responses or artwork have been copied into the engine.

## Retro-Oberfläche (GUI-Branch)

Die Retro-Oberfläche folgt der mitgelieferten Original-Frontplatte; Raumliste und Preset-Pfeile sind zwei Wege für denselben Raumparameter. Größe/Klang lassen sich ziehen und per Doppelklick neutral stellen. Die anderen abgebildeten Tasten/Buchsen sind dekorativ. CRT, Lampen und Röhren reagieren ausschließlich visuell; die Anzeige nennt die tatsächliche Host-Samplerate/Kanalbelegung. Details, Asset-Nachweise und Screenshot-Verfahren: [Retro-GUI](docs/gui/RETRO_GUI.md).

## Episode-Frontplatten

Episode IV (verwittert) ist die Standardansicht, Episode I die fabrikneue Variante. Der metallische Kippschalter unten rechts ersetzt die dortige Preset-Navigation und wechselt nur die Oberfläche. Die Raumauswahl bleibt links oben. Die Skin-Wahl wird separat als GUI-Vorliebe gespeichert; Audioeinstellungen und Automation bleiben unverändert. [Bedienung, Bildnachweise und technische Prüfung](docs/gui/EPISODES.md).

## Entwicklungsphase 2

Hersteller: **Stromschlag Audio**. Plugin-Identität/Audio-State bleiben kompatibel; die alte Episode-Vorliebe wird bei Bedarf migriert. Die drei Raumabstimmungen sind als kompaktere Sprachräume neu vorgeschlagen. Eigenständige bernsteinfarbene CRT-Ansichten folgen der Raumwahl, dezente Röhren-/Linsenanimationen bleiben vom DSP getrennt. [Details und Prüfverfahren](docs/phase2/PHASE2.md) · [Akustik und A/B-Messwerte](docs/phase2/AKUSTIK.md). Die endgültige Klangabstimmung erfordert einen erneuten Hörtest.

## Phase 2.1

Aktuelle Herstelleridentität: **Stromschlag Audio**, `Strm` / `SpLb`, Bundle-ID `com.stromschlagaudio.spatiallab`. Neue Standardgröße 1504×846 mit separater Größenwiederherstellung, sichtbarere Lichtanimationen und verwitterte Episode-IV-Schalterfassung. Die freigegebene Phase-2-Akustik bleibt unverändert. [Änderungen, Identitätswechsel und Windows-Installation](docs/phase2.1/PHASE2_1.md).

[Phase 2.1: echte große Windows-Screenshots, Röhrenanimationen bei Stille/Audio und erfolgreiches Installationspaket](docs/phase2.1/PRUEFBILDER.md).

## Phase 2.2

Statische Original-Leuchtfelder, korrigierte Reglerflächen und pausierende unsichtbare Editoranimationen. Die Akustik bleibt bis zur eindeutigen Hörtestdiagnose erhalten. [Messbericht](docs/phase2.2/PHASE2_2.md) · [Windows-Build, Installationspaket und echte Prüfbilder](docs/phase2.2/PRUEFBILDER.md).

## Phase 2.3

[Offline-Hörvergleich aus der tatsächlich gebauten VST3: Download, Quellen, Pegel und Prüfbericht](docs/phase2.3/PHASE2_3.md). Die zwei zusätzlichen orangefarbenen Glimmstellen sind reine GUI-Animationen; DSP und Raumabstimmung bleiben erhalten. Audio und große Testmedien liegen ausschließlich in Actions-Artefakten.

## Phase 3A

19 Umgebungen: die bisherigen drei Räume plus sechs Innenräume, vier Fahrzeugkabinen, Kapelle/Kathedrale/Höhle und drei eigenständig abgestimmte Sci-Fi-Cockpits. Gruppierte Auswahl im bestehenden Display, eigene bernsteinfarbene CRT-Szenen, unveränderte Originalfrontplatten und weiterhin genau drei Audioparameter. [Preset-Liste, DSP-/Migrationsprüfung, Messwerte und Windows-Downloads](docs/phase3a/PHASE3A.md). Alte diskrete Raum-Automationspunkte bleiben erhalten; die Einschränkung interpolierter Zwischenwerte ist dort ausdrücklich dokumentiert. Phase 3B ist nur vorbereitet, keine Outdoor-Presets oder neue Browser-Demo.
