# SPATIAL LAB v1.3 – Audio Drama Spatial Environments

Einheitliche Produktkennung und Versionsanzeige in allen Oberflächen sowie aktualisierte echte Windows-Screenshots, Dokumentation und Homepage-Präsentation.

- Verbindlicher Untertitel: **Audio Drama Spatial Environments**.
- Classic Dunkel und Classic Hell: Herstellerzeile **STROMSCHLAG AUDIO · v1.3**, automatisch aus der CMake-Projektversion erzeugt. SPATIAL LAB und der orangefarbene Untertitel stehen nebeneinander.
- Retro Episode I und IV zeigen denselben Produktuntertitel und dieselbe automatisch erzeugte Versionskennung im Metadatenbereich. Originalfrontplatten, Stromschlag-Audio-Logo, Bedienelemente, Animationen und tatsächliche Sampleratenanzeige bleiben erhalten.
- Öffentliches Versionsschema: Tag **v1.3**, Release **SPATIAL LAB v1.3**, ZIP **SpatialLab-v1.3-Windows-x64-VST3.zip**. Interne JUCE-/VST3-Version: 1.3.0.
- DSP und alle 26 Raumabstimmungen aus v1.2.0 bleiben bytegenau unverändert. Keine Änderungen an AudioProcessor, Parametern, Preset-Keys, Session-Schema, Hersteller-/VST3-IDs oder Buslayouts.

## Download und Installation

Das Windows-ZIP enthält direkt den vollständigen Ordner `SpatialLab.vst3`. DAW schließen, ZIP entpacken und den gesamten Ordner nach `C:\Program Files\Common Files\VST3\` kopieren. Anschließend die VST3-Suche bei Bedarf erneut ausführen. Prüfsumme: `SHA256SUMS.txt` im Release.

Windows x64 und ein kompatibler 64-Bit-VST3-Host erforderlich; Microsoft Visual C++ Redistributable v14 x64. Primäre Sampleraten bleiben 44,1 und 48 kHz. Das Plugin liefert ausschließlich 100 % Wet für Send-/FX-Kanäle.

## Oberflächen und technische Prüfung

[Aktuelle echte Windows-Screenshots](https://github.com/StromschlagProductions/SpatialLab-vst/blob/main/docs/classic/CLASSIC_GUI.md) · [Versionsquelle, Prüfbericht und Bildherkunft](https://github.com/StromschlagProductions/SpatialLab-vst/blob/main/docs/v1.3/UPDATE.md).

Vollständiger Windows-Release-Build, Windows-CTest (8/8), Linux ASan/UBSan (6/6), pluginval Strictness Level 8 und Prüfung der nativen Windows-Aufnahmen beider Retro-Skins und beider Classic-Themes sind erfolgreich abgeschlossen. Der Veröffentlichungsworkflow prüft außerdem Paket-Herkunft, Versionsmetadaten, Identität, Screenshot-Manifest und ZIP-Prüfsumme.
