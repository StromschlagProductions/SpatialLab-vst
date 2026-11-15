# Retro-GUI: Umsetzung und Prüfung

**Historischer Umsetzungsstand:** Die aktuelle Frontplatten-/Schalter-Implementierung ist in [EPISODES.md](EPISODES.md) beschrieben. Die frühere Logo-Retusche und die untere Preset-Navigation werden dort nicht mehr verwendet.

## Grundlage und Abhängigkeit

Der separate Branch `develop/retro-gui` basiert auf DSP-Commit `4efa6d8bff6185fc76a720c47b69dec163aa9943`. DSP-PR #1 war bei Arbeitsbeginn weiterhin offen/Draft. GUI-PR #2 richtet sich gegen `develop/juce8-room-foundation`, nicht gegen `main`. Vor Merge sind Freigabe und Hör-/DAW-Test erforderlich.

Der verbindliche Auftrag ist unverändert unter [CODEX_AUFTRAG.md](CODEX_AUFTRAG.md) abgelegt. Beide PNG-Dateien in `assets/` entsprechen bytegenau dem Übergabepaket:

- Frontplatte: SHA-256 `6ccfd1b74607507b6763dffcb15330c74953d20959fc7a046464ec26649855af`
- Original-Logo: SHA-256 `b199e6c323d08ba042bbceb3b827e293c5c7a51faf90308c849f15b0b433c9a6`

Keine Bildgenerierung, keine Nachzeichnung des Logos. Die PNGs werden über JUCE BinaryData eingebettet; die installierte VST3 benötigt keine externen Bildpfade.

## Grafik und aktive Bereiche

Das 1672×941-Raster bleibt der Hintergrund. Anordnung, Farben, Gebrauchsspuren, Schrauben, Warnstreifen, Lüftung und Röhren stammen direkt aus der akzeptierten Vorlage. Die Regler verwenden kreisförmig ausgeschnittene und rotierte Originalpixel einschließlich Originalzeiger; keine zweiten Regler oder Zeiger werden darübergezeichnet.

Überlagert werden nur funktionale Bereiche:

- Raumliste links und Preset-Pfeile unten rechts: eine gemeinsame vorhandene `room`-Quelle. Beide Wege rufen dieselbe Auswahlmethode auf; Makro-Reset und Session-Recall bleiben Aufgaben des unveränderten Parametercodes.
- Größe/Klang: zwei JUCE-Slider mit Host-Attachments, Mausziehen, Tastaturfokus und Doppelklick auf neutral. Hover/Fokus wird bernsteinfarben umrandet. Keine neuen Audio- oder Skin-Regler.
- CRT: das originale Drahtgitter und die Möbel bleiben erhalten. Die eingebrannte Signalspur wird lokal abgedeckt und durch eine signalabhängige Spur ersetzt; der aktuelle Raumname wird eingeblendet. Die Raumzeichnung ist eine stilisierte Anzeige, keine je Preset physikalisch unterschiedliche Simulation.
- Die eingebrannte Fantasie-Logozeichnung wird lokal durch rekonstruierte Metalltextur ersetzt, ohne Plakette, Rahmen oder rechteckige Deckfläche. Die unveränderte Alphaform der Original-PNG wird direkt als dunkelroter Siebdruck gerendert (Grundfarbe `#862c22`, 93 % Farbdeckung). Transparenz und proportionale Skalierung bleiben erhalten; die leicht durchscheinende Druckfarbe lässt die Metalloberfläche mitwirken.
- Der eingebrannte Text `STEREO · 48 kHz` wird lokal ersetzt. Tatsächliche Kanalbelegung und Samplerate kommen atomar aus dem Prozessor; ohne vorbereitete Audiowiedergabe steht dort `AUDIO BEREITSTELLEN`.
- BEREIT bedeutet: Audio vorbereitet. AKTIV folgt tatsächlich eingehenden Audioblöcken; SIGNAL zeigt den beobachteten Hallpegel. Röhren dürfen dazu leicht glimmen. Alle übrigen Tasten, Buchsen und Plaketten sind Dekoration ohne Audiofunktion.

Fensterstandard: 1003×565 logische Pixel, festes Seitenverhältnis, skalierbar bis 1672×941. Alle aktiven Flächen folgen derselben Koordinatentransformation. Windows-DPI-Skalierung erfolgt durch JUCE. Die 100/125/150-Prozent-JUCE-Render dienen zusätzlich zur Prüfung der Bild-/Schriftlesbarkeit; sie ersetzen keinen realen DAW-Test auf drei DPI-Einstellungen.

## Audio-Unverändertheit und Messwerte

`RoomEngine.h`, Parameter-IDs/-Definitionen und Zustandsserialisierung bleiben unangetastet. Die einzigen Prozessor-Ergänzungen veröffentlichen vorbereitete Samplerate/Kanäle sowie Blockaktivität und beobachtete Hallspitzen. Der Samplepfad und seine Berechnungsreihenfolge ändern sich nicht.

`GuiTelemetry` verwendet ausschließlich auf der Zielplattform garantiert lockfreie Atomics. Die Spitzenwertübergabe hat maximal zwei CAS-Versuche, keine unbeschränkte Schleife. Die zusätzliche Pegelsuche läuft nur bei offenem Editor. Der Editor liest mit 30 Hz und glättet ausschließlich seine Darstellung. Keine GUI-Methoden, Message-Posts, Locks oder Allokationen im Audiothread. Beim Schließen enden Animation und Messwertsuche. Der beobachtete Pegel stammt aus der Hallberechnung; beim Plugin-Bypass kann diese intern weiterlaufen, während der vorhandene Bypass-Callback den tatsächlichen Send-Ausgang stummschaltet.

## Tests und echter Screenshot

Die bisherigen DSP-/Prozessor-Tests bleiben unverändert. `SpatialLabGuiTests` ergänzt Prüfungen auf bitidentische Samples mit/ohne Messwertbeobachtung bei 44,1/48 kHz, unveränderten Session-State beim Öffnen, genau drei Parameter/zwei Makroregler, synchronisierte Preset-Bedienwege und Ende der Messwertsuche nach Editor-Schließen.

`SpatialLabGuiPreview` öffnet denselben Produktionseditor in einem nativen JUCE-Fenster. Nur dieser Prüfhost speist synthetisches Audio ein, nicht das Plugin. Auf Windows erzeugt er mit GDI-BitBlt einen **echten Bildschirm-Screenshot** der sichtbaren Editorfläche und separat bezeichnete `createComponentSnapshot`-Render bei 100/125/150 %. Die Render sind echte JUCE-Komponentenzeichnungen, aber ausdrücklich keine Bildschirmfotos. Windows-CI legt alle Bilder unter `gui-preview/` im Build-Artefakt ab.

Lokal starten (Release-Build):

```powershell
& ".\build\SpatialLabGuiPreview_artefacts\Release\SpatialLab GUI Preview.exe" --output=build/gui-preview
```

Ohne `--output` bleibt das Vorschaufenster zur Bedienprüfung geöffnet. Der Prüfhost ist kein neuer Plugin-Modus und wird nicht in das VST3-Bundle installiert.

Offen bleiben die Freigabe der Logo-Integration und lokalen Textabdeckungen, reale Windows-DAW-Tests inklusive 100/125/150 % DPI sowie Hörtests. Eventuelle Einschränkungen des CI-Desktops werden bei der Screenshot-Übergabe benannt; ein Konzeptbild wird nicht als Screenshot ausgegeben.

## Ergebnis der automatisierten und visuellen Prüfung

Code-Stand `6d490fc6019ba3352ba761ab02b247cde2d5f456` ist im [CI-Lauf 43733243192](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43733243192) vollständig grün: Windows-VST3, DSP, Prozessor, RetroGUI, PNG-Dateiprüfung und Linux-ASan/UBSan. Die [Prüfbilder mit Herkunft und klarer Unterscheidung zwischen Screenshot und JUCE-Render](screenshots/README.md) sind im Repository abgelegt. Der anschließende Dokumentationscommit ergänzt ausschließlich diese Nachweise; der geprüfte Plugin-Code bleibt identisch.

## Korrektur: Logo direkt als Siebdruck

Ausschließlich der Logo-Bereich wurde gegenüber `32f4d8a` geändert. Die zusätzliche Plakette einschließlich Farbverlauf/Rahmen entfällt vollständig. `SpatialLab_LogoRegion_Restored.png` ist eine transparente, dünn besetzte Retuscheebene: Nur die alten Druckstellen sind deckend, keine geometrische Rechteckfläche. Eine klassische Navier-Stokes-Retusche rekonstruiert diese Stellen aus der benachbarten cremefarbenen Metalltextur. Schrauben, rote Markierungen und unveränderte Bildteile bleiben ausgespart. Kein KI-Modell und keine Logo-Nachzeichnung.

Die Retusche ist mit `Tools/restore_logo_region.py` reproduzierbar; nur dieses Asset-Werkzeug benötigt OpenCV 4.11.0.86 und NumPy 2.2.6. Im Plugin sind die fertigen PNG-Pixel eingebettet, keine OpenCV-Abhängigkeit und keine Laufzeit-Retusche. Die transparente Retusche wird beim Editor-Öffnen einmal in Originalauflösung mit einer Kopie des Hintergrunds verbunden; erst das zusammenhängende Bild wird skaliert. Dadurch entstehen keine hellen Interpolationssäume um die Retuschmaske. Beide Original-PNGs behalten die oben dokumentierten Prüfsummen. Der Audiocode, Messwertpfad, sämtliche Parameter und die übrige GUI sind gegenüber `32f4d8a` unverändert.
