# SPATIAL LAB v1.4 – Pegelkorrektur für Hörspiel-Sends

**Audio Drama Spatial Environments**

- Neun normale Innenräume und sechs Außenräume liefern gegenüber v1.3 exakt +6 dB Wet-Pegel.
- Offene Landschaft erhält insgesamt +18 dB, weil +6 dB im neutralen −6-dB-Send-Aufbau weiterhin kaum wirksam wären. Sie bleibt trocken und ohne diffusen Nachhall oder neue Echos.
- Wald erhält +6 dB; sein kurzer diffuser Raumanteil bleibt erhalten.
- Fahrzeuge, Kapelle, Kathedrale, Höhle und alle Cockpits bleiben klanglich und im Pegel unverändert.
- Ausschließlich feste preset-spezifische Ausgangsverstärkung. Keine zusätzlichen Regler, keine Änderungen an akustischen Koeffizienten, Parametern, Preset-Reihenfolge oder Plugin-Identität.

## Installation und Headroom

SpatialLab-v1.4-Windows-x64-VST3.zip vollständig entpacken und den gesamten Ordner SpatialLab.vst3 nach `C:\Program Files\Common Files\VST3\` kopieren. Windows x64, kompatibler 64-Bit-VST3-Host und Microsoft Visual C++ Redistributable v14 x64 erforderlich. Ausschließlich Wet auf Send-/FX-Kanälen. Primäre Sampleraten 44,1 und 48 kHz.

SIZE/TONE 50 %, Send −6 dB, FX 0 dB bilden die Referenz. Die Verstärkung ist linear, ohne Limiter: sehr laute trockene Quellen können bei Dry+Wet-Summierung zusätzliche DAW-Headroom-Reserve benötigen. Die Messdokumentation zeigt sowohl eine Referenz mit offengelegtem festem Eingangstrim als auch die ungetrimmten Summenpeaks.

[26-Preset-Messungen, reproduzierbare A/Bs und Grenzen](https://github.com/StromschlagProductions/SpatialLab-vst/blob/main/docs/v1.4/WET_LEVELS.md). Eine subjektive Hörfreigabe durch den Anwender in Studio One wird nicht vorgetäuscht.


## Technische Abnahme und A/B-Dateien

Windows CTest **9/9**, Linux ASan/UBSan **7/7**, pluginval **Level 8 SUCCESS**. Alle 26 Presets bei 44,1/48 kHz vor/nachher gemessen: +6/+18/0 dB exakt, Reflexionsbeginn und normierte Nachhallverteilung unverändert. Fahrzeuge/Spezialräume/Cockpits einschließlich der vollständigen Vergleichs-WAVs bitidentisch.

[Unnormalisierte Dry-/Wet-/Mix-Hörvergleiche aller 26 Presets](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/44322843587/artifacts/14434544155) als zeitlich befristetes Actions-Artefakt (ca. 498 MiB, GitHub-Anmeldung). Die subjektive Wirkung in der eigenen Szene und Studio One bleibt durch den Anwender zu beurteilen; Messungen sind keine vorgetäuschte Hörfreigabe.
