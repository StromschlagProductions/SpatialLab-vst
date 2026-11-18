# SPATIAL LAB v1.4 – Wet-Pegel und Außenräume

## Korrektur

| Presets | Änderung gegenüber v1.3 |
|---|---:|
| Wohnzimmer, Küche, Badezimmer, Schlafzimmer, Flur, Treppenhaus, Keller, Dachboden, Garage/Werkstatt | +6 dB |
| Wald, Straße/Gasse, Innenhof, Canyon, Tunnel, Unterführung | +6 dB |
| Offene Landschaft | +18 dB insgesamt |
| Fahrzeuge, Kapelle, Kathedrale, Höhle, alle Cockpits | unverändert, 0 dB |

Feste Werte in Source/WetVoicing.h, ausschließlich nach Early/Late-Summierung und außerhalb aller Feedbackwege. Keine Änderungen an Laufzeiten, Absorption, Frequenzgangform, Nachhallzeiten, Diffusion, Stereo-Verteilung oder Early/Late-Balance. Die bisherigen sieben Outdoor-Profile bleiben einschließlich aller Taps unverändert. Gain folgt dem aktiven Preset; ein Wechsel erfolgt innerhalb der bisherigen 30-ms-Ausblendung bei Gain null. Keine neuen Allokationen, Parameter oder Bedienelemente.

Offene Landschaft lag mit neutralen Reglern und Send −6 dB bei Wet/Dry −37,66 dB. Die allgemeine +6-dB-Korrektur ergäbe −31,66 dB und bliebe sehr schwach. +18 dB insgesamt ergeben −19,66 dB (rund 10,4 % RMS-Amplitude der trockenen Quelle), weiterhin nur zwei schwache Boden-/Fernreflexionen und keinerlei diffuser Nachhall. Keine zusätzlichen Echos. Wald erreicht mit +6 dB rund −12,66 dB Wet/Dry (23,3 % RMS-Amplitude); der bereits vorhandene kurze, gedämpfte diffuse Anteil bleibt unverändert. Keine weitere Wald-Anhebung.

## Reproduzierbare Prüfung

[Alle 52 Vorher-/Nachher-Paare](COMPARISON.md) · [CSV mit allen 104 Messzeilen](metrics.csv) · [Quellen, Pegel und Hashes](source/reference.json).

44,1 und 48 kHz, identisches eigenes CMU-Flite-Sprachmaterial, dual-mono auf Stereo-FX, SIZE/TONE 50 %, Send −6 dB, FX 0 dB. Kein Loudness-Matching, keine Normalisierung, keine Begrenzung. Für die Headroom-Referenz wird die ursprüngliche TTS-Quelle einmalig und für alle Räume und beide Versionen identisch um feste −6 dB abgesenkt; dieser Eingangstrim wird nicht aus Ausgangspeaks berechnet. Ihr Peak liegt danach bei etwa −9,98 dBFS. Der Send bleibt zusätzlich bei −6 dB. Der ungetrimmte Quellpeak und die abgeleiteten ungetrimmten Summenpeaks werden ebenfalls ausgewiesen.

Die lineare Float-Engine clippt oder sättigt intern nicht. Die Addition einer lauten trockenen Quelle und des angehobenen Wet-Signals kann dennoch über 0 dBFS steigen: beim ursprünglichen TTS-Pegel beträgt der höchste berechnete Summenpeak +2,10 dBFS, bei der dokumentierten Headroom-Referenz −3,90 dBFS. Es gibt keinen versteckten Limiter. Die DAW-Summe benötigt entsprechend Reserve vor Fixed-Point-Ausgabe. Ein garantiert unter 0 dBFS liegender Dry+Wet-Mix für beliebige Eingangspegel wäre mit einer exakt linearen +6-dB-Anhebung nicht vereinbar.

Der eingefrorene v1.3-Quellstand und sein unabhängiges Hash-Manifest liegen unter Tests/Reference/V1_3. WetLevels prüft alle Makro-Ecken, beide Kanäle, exakt +6/+18/0 dB und sämtliche 676 Preset-Wechsel pro Hauptsamplerate. Fahrzeuge/Spezialräume/Cockpits sind samplegenau gleich, ihre vollständigen Vergleichs-WAVs ebenfalls bytegleich. Die Impulsantwort-Ankunft und der Anteil nach 50 ms bleiben bei allen 26 Profilen unverändert.

Reproduktion:

```sh
cmake -S . -B build -DSPATIALLAB_BUILD_PLUGIN=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
python Tools/wet_level_source.py build/wet-levels/source
build/SpatialLabWetLevelAudit build/wet-levels build/wet-levels/source
python Tools/compare_wet_levels.py build/wet-levels
```

Python benötigt numpy; die Quellen-Erzeugung benötigt ffmpeg mit CMU Flite. WAVs bleiben im Actions-Artefakt und werden nicht im Repository gespeichert. Der bestehende historische Issue-6-Audit wird zusätzlich gegen den eingefrorenen v1.3-Stand weitergeführt, damit dessen frühere Akustik-/Pegelziele nicht versehentlich als aktuelle v1.4-Spezifikation gelten.

## Hörvergleich und Grenzen

Das zusätzliche Artefakt SpatialLab-v1.4-Wet-Level-Comparison enthält eine unveränderte Referenz-Dry-Datei sowie für alle 26 Räume Wet und Dry+Wet aus v1.3 und v1.4. Beim Vergleichen dieselbe Wiedergabelautstärke verwenden; keine Player-Normalisierung aktivieren. Insbesondere Wohnzimmer, Küche, Bad, Schlafzimmer, Wald, offene Landschaft, Straße, Innenhof, Canyon und Tunnel direkt im Mix beurteilen.

Die technischen Messungen zeigen die tatsächliche Änderung im Dry/Wet-Mix, ersetzen aber keine subjektive Hörfreigabe. Eine durchgeführte Studio-One-Abhörprüfung wird hier nicht behauptet. Akustikform und Klang der unveränderten Spezialprofile sind separat durch Sample-/Dateivergleich abgesichert.

## Build und Veröffentlichung

Lokale Pegelmessungen und Regressionstests liegen vor. Vollständige Windows-/Linux-Abnahme, pluginval Level 8, Versions-Screenshots und Veröffentlichung folgen erst nach erfolgreicher Prüfung. Plugin-Identität, Preset-Reihenfolge, alle drei Parameter und Session-Schema bleiben unverändert.
