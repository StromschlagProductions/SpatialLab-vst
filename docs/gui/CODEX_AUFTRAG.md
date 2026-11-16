# SPATIAL LAB – GUI-Implementierung (Codex-Auftrag)

Repository: `StromschlagProductions/SpatialLab-vst`

## Ausgangslage und Branch

Der DSP-Prototyp liegt im Draft-PR #1, Branch `develop/juce8-room-foundation`. Verwende diesen Stand als Ausgangspunkt und erstelle einen **separaten GUI-Branch** sowie einen eigenen Draft-PR gegen den DSP-Branch, nicht gegen `main`. Falls PR #1 inzwischen verändert oder gemergt wurde, zunächst Branch-/PR-Status prüfen und die Abhängigkeit dokumentieren. **Keine ungeprüften Änderungen an `main` und keine parallelen Eingriffe in Codex' DSP-Arbeit.**

## Verbindliche Bilddateien

- `assets/SpatialLab_Retro_Frontplatte_Referenz.png` – zuletzt akzeptierte Designreferenz, 1672 × 941 Pixel. **Kein neues Bild generieren, keine freie Neuinterpretation.** Erhalte die prägende Anordnung, Proportionen, Farben, Gebrauchsspuren, Röhren, Warnstreifen, Schrauben, Lüftungsschlitze und das bernsteinfarbene CRT.
- `assets/Stromschlag_Audio_Original_Logo.png` – echte, transparente Original-PNG, 360 × 356 Pixel. Diese Datei als Logoquelle benutzen. Die bereits in der Frontplattenreferenz sichtbare Logo-Zeichnung ist nicht verbindlich und soll, falls erforderlich, sauber überdeckt/ersetzt werden. **Keine KI-Nachzeichnung, kein Ersatzlogo.**

Die Frontplatte ist ein gerastertes Konzeptbild, kein Satz einzeln editierbarer Bedienelemente. Deshalb die aktiven Bedienflächen und animierten Displayregionen als präzise ausgerichtete JUCE-Overlays bzw. zugeschnittene grafische Ebenen realisieren. Statische Elemente können Teil des Hintergrundbilds bleiben. Keine doppelt sichtbaren Knöpfe. Falls sich einzelne Regionen nicht sauber isolieren lassen, transparent dokumentieren und einen gezielten Vorschlag machen, statt die gesamte Frontplatte neu zu entwerfen.

## Bedienkonzept – genau drei Klangparameter

1. **Raum**: Auswahl des Presets. Für die erste Version Wohnzimmer, Küche, Badezimmer; später Indoor und Outdoor, ungefähr 15–25 kuratierte Presets.
2. **Größe**: kleiner ↔ neutral ↔ größer.
3. **Klang**: dunkler ↔ neutral ↔ heller.

Bei jedem tatsächlichen Raumwechsel Größe und Klang auf neutral zurücksetzen. Beim erneuten Öffnen eines gespeicherten DAW-Projekts die gespeicherten Werte dagegen wiederherstellen. **Keine zusätzlichen Audioparameter:** kein Dry/Wet, Output Gain, Early/Late, Pre-Delay, Damping, Width, Distance, Diffusion, Stereo-Schalter oder sonstige Expertenregler. Immer 100 % Wet; Send-Pegel liegt in der DAW. Der Plugin-DSP bleibt statisch; keine Raumfahrten oder Bewegungsautomatisierung.

Die beiden im Referenzbild sichtbaren Preset-Auswahlbereiche dürfen nicht zu zwei widersprüchlichen Zuständen führen: eine einzige zugrundeliegende Raum-Parameterquelle; falls beide grafisch bleiben, sind sie synchronisierte Darstellungen/Bedienwege desselben Parameters. Beschriftungen wie `STEREO · 48 kHz` dürfen **keinen festen Betriebszustand vortäuschen**: tatsächliche Samplerate/Kanalkonfiguration anzeigen oder als dekorativen Text neutralisieren. `BEREIT`, `AKTIV`, `SIGNAL` sind Statusanzeigen, keine Schalter. Andere dekorative Tasten/Stecker sind keine verborgenen Audiofunktionen.

## Animationen und Qualität

Das CRT zeigt eine stilisierte bernsteinfarbene Drahtgitter-Raumansicht und eine dezente, signalabhängige Anzeige. Röhren und Statuslampen dürfen glimmen oder auf Aktivität reagieren. Keine exakte physikalische Visualisierung nötig. Audio-zu-GUI-Metering ausschließlich über geeignete lockfreie, atomare Datenübertragung; keine GUI-Operationen, Locks, Allokationen oder Message-Thread-Aufrufe im Audiothread. Bei geschlossenem Editor keine GUI-Animation. Angemessene Updatefrequenz und niedrige GUI-CPU-Last.

GUI skalierbar und bei Windows 100 %, 125 %, 150 % lesbar. Aktive Bedienflächen mit sinnvollen Hitboxes, sichtbarem Zustand und Maus-/Host-Synchronisation. Keine Änderung der bestehenden Parameter-IDs, Zustandsserialisierung oder DSP-Klangalgorithmen. Drei Skin-Varianten (Retro, Light, Dark) sind **nicht Bestandteil dieses Auftrags**; zunächst ausschließlich die Retro-Oberfläche überzeugend umsetzen, um Umfang und Risiko gering zu halten. Weathering-Regler ebenfalls nicht implementieren: keine zusätzlichen GUI-Controls ohne separate Freigabe.

## Abnahme und Übergabe

Windows-VST3 über GitHub Actions bauen und bestehende Tests unverändert erfolgreich halten. Wenn möglich einen echten Screenshot des laufenden Plugin-Editors erstellen und im PR verlinken; ein gerendertes Konzeptbild ist kein Beleg für die tatsächliche JUCE-Oberfläche. Dokumentation, PR-Text und Bedienhinweise auf Deutsch. Benenne bekannte Einschränkungen ehrlich. `main` unverändert lassen; vor Merge erst Freigabe und Hör-/DAW-Test abwarten.
