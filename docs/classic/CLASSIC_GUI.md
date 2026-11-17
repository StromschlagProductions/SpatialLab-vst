# Zusätzliche Classic-Oberfläche

Branch: feature/classic-gui. Kein zweites Plugin, kein Release und keine Zusammenführung nach main.

## Bestandsaufnahme und Wiederverwendung

SpatialLabEditor besitzt bereits die beiden Slider samt SliderParameterAttachments, PresetBrowser, die unabhängige Episode-Präferenz und einen Editor-Timer. Diese Instanzen bleiben bestehen. Die Erweiterung ergänzt ausschließlich einen Look-Zustand und alternative Anordnung/Zeichnung im selben Editor. Kein eigener Classic-Prozessor, keine separaten Parameter, keine Änderungen an AudioProcessor, Raumkatalog, DSP oder Audio-State.

Gestalterische Referenz: [Locations-vst PluginEditor.cpp](https://github.com/StromschlagProductions/Locations-vst/blob/d6a1a565842c23b313878d111e57c0aff2b1b630/Source/PluginEditor.cpp), insbesondere die Dunkel-/Hell-Paletten und Kopfzeile mit Original-Logo als Tintenmaske. Farben zentral in ClassicTheme.h; keine separate Hell-/Dunkel-Implementierung.

## Bedienung

Retro bleibt Standard für bestehende Einstellungen. Ein kleines Zahnrad rechts oben im freien Hintergrund öffnet Retro / Classic Dunkel / Classic Hell. Der vorhandene Episode-I/IV-Schalter bleibt ausschließlich im Retro-Modus und unverändert an seiner bisherigen Position.

Classic zeigt dieselbe Look-Auswahl in der Kopfzeile, das originale Stromschlag-Audio-Logo, SPATIAL LAB und einen Untertitel. Darunter arbeitet derselbe PresetBrowser mit allen 26 Räumen in vier Kategorien, Wheel, Schieber, Direktwahl und Host-Synchronisierung. Die beiden vorhandenen Drehregler stehen unter dem Browser. Ihre Werte und Attachments bleiben unverändert; kein neuer Audioparameter.

Der Look wird als reiner GUI-Schlüssel `look` (0 Retro, 1 Dunkel, 2 Hell) in der bisherigen PropertiesFile gespeichert. Die separate Episode-Einstellung bleibt erhalten. Fenstergröße und proportionale Skalierung bleiben für alle Looks gleich (1003×565 bis 2174×1223, Standard 1504×846). Eine kompaktere unabhängige Classic-Fenstergröße wird bewusst nicht eingeführt; damit bleibt die vorhandene Größen-/Hostlogik bestehen.

Röhren, Glut und Raumzeichnungen werden nur im Retro-Modus gezeichnet. Classic benötigt keine Animation und zeichnet nur bei Änderungen neu. Der vorhandene Timer wird für Parameter-/Host-Synchronisierung weiterverwendet; keine neuen Timer oder Hintergrundberechnungen.

## Abgrenzung und Prüfung

Tools/check_classic_scope.py sichert zwölf unveränderte Release-Dateien per SHA-256: Prozessor/State/Identität, beide Engines, Katalog, RetroMotion, GuiTelemetry, GuiPreferences sowie Originalfrontplatten und Logo. Nur Git-Zeilenenden der Quelltexte werden kanonisiert; PNGs bleiben binär unverändert. CMake ergänzt lediglich die vorhandene Logo-PNG als eingebettete GUI-Ressource, ohne Plugin-ID oder Version zu ändern.

Erweiterte GUI-Tests prüfen wiederholte Look-/Episode-Wechsel, bytegleichen Audio-State und samplegleiches Audio gegenüber einer Instanz ohne Lookwechsel, 26 Direktwahlen je Look bei drei Größen, unabhängige Persistenz und unverändert drei Audio-Parameter. Bestehende Audio-/GUI-/Lichttests bleiben aktiv. Windows-CI ergänzt pluginval v1.0.4 mit festem Paket-Hash und strictness level 8 sowie echte Desktop-Aufnahmen der drei Looks.

Lokal: alle vier Release-DSP-Tests bestanden; tatsächliche Editor-/GUI-Testquellen gegen JUCE 8.0.6 syntaktisch geprüft. Windows-/pluginval-Ergebnisse und Bildlinks werden nach der Abnahme ergänzt. Es wird kein Erfolg vorweggenommen.
