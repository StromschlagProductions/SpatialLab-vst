# SPATIAL LAB Phase 3A

[Windows-VST3 herunterladen](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43865790113/artifacts/14226906488) · [38 echte Screenshots](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43865790113/artifacts/14226960903) · [38 Impulsantworten und Messdaten](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43865790113/artifacts/14226518817) · [Erfolgreicher CI-Lauf](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43865790113)

## Umfang und akustische Ziele

19 auswählbare Indoor-/Fahrzeug-/Cockpit-Umgebungen. Die ersten drei behalten semantische Indizes 0/1/2 und ihre freigegebenen neutralen Impulsantworten. Alle folgenden Presets sind eigenständige RoomSpec-Datensätze mit individuellen frühen Zeiten, Vorzeichen/Pegeln, FDN-Delaylängen, Dämpfung, Diffusionswinkel und Crossfeed. Kein Lautheitsnormalisierer, kein Dry-Signal, kein Oversampling. Neutrale Regler jeweils SIZE/TONE 50 %.

| Index | Umgebung | nominelle RT60 | Dämpfungs-Cutoff | Early/Late | Ziel |
| --- | --- | ---: | ---: | --- | --- |
| 0 | Wohnzimmer | 0.4 s | 3400 Hz | 1.3/1.15 | Freigegebener möblierter Wohnraum, unveränderte frühe Räumlichkeit. |
| 1 | Küche | 0.32 s | 5200 Hz | 1.08/0.95 | Freigegebene kompakte helle Küche, unveränderte Antwort. |
| 2 | Badezimmer | 0.38 s | 7600 Hz | 1.12/0.95 | Freigegebenes kurzes gekacheltes Bad, unveränderte Antwort. |
| 3 | Schlafzimmer | 0.26 s | 2800 Hz | 1.35/0.9 | Weich möbliert, kurze warme Antwort und zurückhaltender Late-Anteil. |
| 4 | Flur | 0.65 s | 4600 Hz | 1.18/1.12 | Längliche Begrenzung, zeitlich gestaffelte klarere Wandreflexionen. |
| 5 | Treppenhaus | 1.35 s | 5100 Hz | 1.12/1.18 | Vertikale harte Flächen, gestaffelte Rückwürfe und längerer diffuser Nachhall. |
| 6 | Keller | 0.92 s | 2900 Hz | 1.25/1.1 | Dunkle massive Flächen, tiefere modale Struktur mit mittlerer Fahne. |
| 7 | Dachboden | 0.45 s | 2400 Hz | 1.3/0.94 | Schräge hölzerne Flächen, unregelmäßige Wege und stärkere Höhendämpfung. |
| 8 | Garage / Werkstatt | 0.82 s | 6200 Hz | 1.28/1.12 | Harte größere Arbeitsfläche, klare Reflexionen und hellerer mittellanger Ausklang. |
| 9 | Kleinwagen | 0.11 s | 2400 Hz | 1.18/0.68 | Sehr kurze kompakte Kabine, absorbierende Ausstattung, wenig Late. |
| 10 | Großer PKW | 0.17 s | 2900 Hz | 1.22/0.76 | Breitere Kabine und etwas längere, weichere Reflexionsverteilung. |
| 11 | Transporter | 0.33 s | 5100 Hz | 1.25/1.03 | Hohlerer metallischer Laderaum mit längeren Rückwürfen. |
| 12 | LKW-Kabine | 0.21 s | 3600 Hz | 1.2/0.88 | Kompakte hohe Kabine, mittlere Dämpfung und asymmetrische Reflexionswege. |
| 13 | Kleine Kapelle | 2.1 s | 4800 Hz | 0.95/1.22 | Kleiner hoher Sakralraum, stärkere Diffusion und gut erkennbare kurze Kirchenfahne. |
| 14 | Kathedrale | 5.2 s | 4200 Hz | 0.83/1.25 | Hoher großer Raum mit langen Laufwegen, dichter diffuser Fahne und zunehmender Höhendämpfung. |
| 15 | Höhle | 2.7 s | 2700 Hz | 1.24/0.94 | Unregelmäßig verteilte diskrete Rückwürfe bis 348 ms, dunkler und weniger diffus als die Kathedrale. |
| 16 | X-Wing Cockpit | 0.14 s | 6500 Hz | 1.26/0.77 | Geschlossenes metallisches Cockpit, kurze Wege und eher kohärente modale Reflexionen. |
| 17 | Y-Wing Cockpit | 0.25 s | 4300 Hz | 1.27/1.02 | Hohler, rauer und mechanischer; längere ungleiche kurze Delay-Moden und geringere Diffusion. |
| 18 | Snowspeeder Cockpit | 0.085 s | 7800 Hz | 1.17/0.58 | Sehr kurze harte Wege, trockene blecherne Reflexionen und geringer Late-Anteil. |

Alle implementierten Delayzeiten, Tap-Gains, Crossfeeds und Diffusionswinkel stehen vollständig in [presets.json](presets.json) und `Source/RoomEngine.h`. Die nominellen RT60 beschreiben den tiefen Loop-Gain; der breitbandige integrierte T20-Wert hängt zusätzlich von Reflexionen und Dämpfung ab und ist kein physikalisches Messmodell.

Fahrzeuge/Cockpits verwenden dichte kurze Laufzeiten und kleine Late-Anteile. Ihre unterschiedlichen modalen Färbungen entstehen durch die tatsächlichen individuellen FDN-Längen, Rückkopplung, Diffusionsrotationen und Tap-Vorzeichen; kein zusätzliches Motorgeräusch, kein künstliches Rauschen oder fremdes Faltungsmodell. Diese kreativen Cockpit-Ziele behaupten keine offiziell festgelegte Fahrzeugakustik. Hörfreigabe erfolgt am Zielsystem.

## DSP-Grundlage

Der bisherige Householder-FDN mit orthogonalen Paarrotationen bleibt das passive Kernmodell. Es wird ausschließlich der aktive Raum berechnet. Alle 19 Netzwerke werden in prepareToPlay vorallokiert; keine Speicherallokation bei Auswahl oder Verarbeitung. Early-Puffer werden für lange Reflexionen in Kapelle/Kathedrale/Höhle passend dimensioniert. Die alten kurzen Puffer behalten ihre ursprüngliche Länge.

SIZE behält Faktor 0,5–2 für den Decay sowie die bestehende Early/Late-Gewichtung. Es entstehen keine bewegten Delayzeiten oder Doppler-Effekte. TONE behält den Faktor 0,5–2 für die Rückkopplungs-Tiefpassfrequenz. Die ursprünglichen drei Räume sind bei neutralen Reglern bitgleich zum eingefrorenen Phase-2-DSP (`Tests/Reference/RoomEnginePhase2.h`).

Ein Auswahlwechsel blendet die bisherige Wet-Antwort in 30 ms aus, löscht das vorbereitete Zielnetzwerk am Nullpunkt und blendet es in 30 ms ein. Keine Mischung zweier Räume, keine Distanz-/Bewegungssimulation. Die kurze Wet-Absenkung beim Wechsel ist beabsichtigt, um harte Abbrüche zu vermeiden. Unterschiedliche Raumpegel werden nicht automatisch angeglichen; individuelle Pegel- und Reflexionsunterschiede gehören zu den Presets. Ein Wechsel während einer starken Transiente und die subjektive Klickfreiheit bleiben praktisch zu prüfen. Der Reset ist eine begrenzte vorallokierte Pufferlöschung; extreme Blockgrößen/höhere Sampleraten können dabei höhere kurzzeitige CPU-Last zeigen.

Der Host-Tail wird konservativ mit 24 s gemeldet (maximale nominelle Kathedralen-Abklingzeit bei SIZE=100 %: 10,4 s). Latenz bleibt null, Routing Mono→Mono, Mono→Stereo oder Stereo→Stereo, weiterhin 100 % Wet. Der Send-Bypass bleibt stumm.

## Navigation und CRT

Das vorhandene linke Display zeigt Kategorie und Position n/19 sowie drei Zeilen der jeweiligen Seite. Kategorien: Innenräume (0–8), Fahrzeuge (9–12), große/besondere Räume (13–15), Sci-Fi-Cockpits (16–18). Die beiden vorhandenen Pfeile wählen die vorige/nächste Umgebung zyklisch; beim Überschreiten der Seite oder Kategorie wird die passende Seite eingeblendet. Zeilentasten wählen direkt den angezeigten Eintrag. Keine zusätzlichen Fenster, Bedienelemente oder Änderungen der Photoshop-Dateien.

Die ersten drei CRT-Geometrien bleiben erhalten. Neue eigene bernsteinfarbene Vektorgeometrien zeichnen Bett, Flur/Türen, Treppenläufe, Kellerregale/Fässer, Dachschrägen, Werkbank und vier unterschiedlich proportionierte Fahrzeuge. Kapelle und Kathedrale verwenden unterschiedlich hohe/tiefe Bogen- und Bankfolgen; die Höhle unregelmäßige Felskonturen. Cockpits sind eigenständig vereinfachte technische Zeichnungen: X-Wing mit perspektivischen Trenchlinien und mittigem Schachtziel, Y-Wing mit separater Cockpitgeometrie und fernem gerastertem Kugelkörper, Snowspeeder mit kompakter Scheibenstruktur und eigenem schematischem Vierbein-Walker. Keine importierten Filmstills, Cockpitgrafiken oder 3D-Modelle.

Originalfrontplatten und Logos bleiben SHA-256-identisch. Episode-Wechsel/Speicherung, Fenstergrößen, Metallregler, CRT-Wellenform, vier Röhren und zwei zusätzliche Glimmstellen sowie die statischen übrigen Anzeigen bleiben erhalten. Unsichtbare Editoren pausieren weiter; der 32-Instanzen-Test ohne Editor bleibt aktiv.

## Kompatibilität und Migration

Hersteller Stromschlag Audio / Strm, Plugin-Code SpLb, Bundle-ID com.stromschlagaudio.spatiallab und alle drei VST3-Klassenkennungen werden nicht geändert. Weiterhin genau drei Audioparameter mit IDs room/size/tone und Version 1. SIZE/TONE bleiben kontinuierlich und unverändert normiert.

Eine einfache 19er AudioParameterChoice-Liste würde alte 0/0,5/1-Automation falsch zuordnen. Deshalb besitzt room eine explizite diskrete Kodierung: Living Room=0, Kitchen=0,5, Bathroom=1. Die neuen Umgebungen belegen die restlichen Schritte des 19-stufigen Rasters. Die interne semantische Reihenfolge 0–18 ist vom normierten Host-Schritt getrennt. Auch die bisherigen englischen Host-Texte und Textimporte für die ersten drei Räume bleiben erhalten. Die GUI verwendet die übersichtliche deutsche Kategorienreihenfolge.

**Einschränkung:** Die Host-Auswahl erscheint deshalb in normierter Reihenfolge nicht wie die GUI-Liste (Kitchen liegt in der Mitte, Bathroom am Ende). Alte diskrete Automationspunkte 0/0,5/1 bleiben korrekt. Eine alte frei interpolierte Automationskurve oder ein unsnapped Zwischenwert kann nun neue Umgebungen ansteuern; dieselbe kontinuierliche Zwischenwertbedeutung lässt sich bei Erweiterung auf 19 Schritte ohne neue Parameter-ID nicht erhalten. Keine stillschweigende Behauptung vollständiger Automationskompatibilität. Hosts mit zwischengespeicherter Parameter-Schrittzahl müssen das Plugin gegebenenfalls neu scannen. Reale Studio-Pro-Projektabnahme erforderlich.

Neues Session-Schema 4 schreibt zusätzlich den stabilen environmentKey. Beim Laden wird dieser statt einer veränderlichen Katalogposition verwendet. Schema 1/2/3 lädt weiterhin ausschließlich die alten drei semantischen Indizes; bei Schema 3 werden SIZE/TONE exakt übernommen. Unbekannte Schlüssel, ungültige Werte und zukünftige Schemata werden ohne Zustandsänderung verworfen. Der neue Zustand lässt sich nicht in einer alten Testversion ohne Schema-4-Unterstützung laden. Vor einem Downgrade Testprojekte sichern. Die GUI-Präferenzen bleiben getrennt vom Audio-State.

## Tests und Messung

Die vier lokalen Release-Tests (Environments, DSP, RetroMotion, RoomVoicing) prüfen 44,1 und 48 kHz. Environments fordert 19 eindeutige Impulsantworten je Rate, endliche begrenzte Samples, Stereoantwort, Wirkung beider Makros und ausklingende Fahnen; die ersten drei Räume werden sampleweise bitgleich mit der eingefrorenen Referenz verglichen. DSP überwacht auch Allokationen bei automatisierten Wechseln über alle 19 Räume. Hohe Sampleraten behalten den bisherigen Smoke-Test, sind keine Hauptabnahme.

Windows ergänzt Prozessor-Routing bei allen 19 Räumen, sechs Blockgrößen inklusive 0/1 Samples, alle Zustands-Roundtrips, Legacy-Anker, Legacy-Schemata, unveränderte GUI-/Audio-Zustände, 19 unterschiedliche CRT-Geometrien je Skin, Größen/Präferenzen und geschlossene Instanzen. Linux-CI behält ASan/UBSan und Leak-Prüfung.

Der Mess-Export erstellt 38 Stereo-Float32-Impulsantworten bei 44,1/48 kHz und eine CSV mit Energie, T20-Schätzung, Spitzenwerten und Signaturen. Kein Normalisieren. Die Daten sind reproduzierbare technische Raumantworten, keine physikalische Raumvermessung. [Lokale Messwerte](environments-local.csv). [Windows-Resultate](environments-windows.csv) und echte Screenshots aller 19 Umgebungen beider Skins stehen in den oben verlinkten Actions-Artefakten.

Die früheren sofortigen Hard-Reset-Tests sind an die ausdrücklich neue Klickschutz-Ein-/Ausblendung angepasst: nach Ablauf der Wechselzeit darf kein alter Tail zurückkehren. Subjektive Klickfreiheit, Sprachverständlichkeit und angenehm unterscheidbare Fahrzeug-/Cockpitfärbung können automatisierte Zahlen nicht garantieren und bedürfen des Hörtests in Fender Studio Pro.

## Phase 3B: Konzept, keine Outdoor-Presets

Vorgemerkt: offene Landschaft, Wald, Straße/Gasse, Innenhof, Schlucht/Canyon, Tunnel, Unterführung. Keine dieser Umgebungen ist in Phase 3A auswählbar.

`Source/OutdoorPlan.h` definiert nur einen geprüften Datenvertrag für maximal 16 diskrete Taps: Verzögerung, Pegel, Tiefpassfrequenz, Stereo-Panorama und begrenztes optionales Feedback, plus diffuse Beimischung. Kein Prozessor und keine aktive Instanz. Die spätere eigenständige Engine soll alle Speicher in prepare vorallokieren, Tap-Parameter glätten, Delaywechsel über getrennte Lesezeiger entschärfen und kontrollierte Feedbackpfade stabil begrenzen. Tap-Panorama ist Energieverteilung pro Reflexion, keine Bewegungssimulation. Die Einzelgrenze eines Feedback-Taps ist noch kein Stabilitätsbeweis für ein verschaltetes Tap-Netz: Die spätere Engine benötigt zusätzlich ein globales Feedback-Gain-Budget beziehungsweise eine geprüfte Rückkopplungsmatrix. Sehr trockene offene Räume können keinen Late-Anteil und nur wenige leise Reflexionen erhalten; Canyon benötigt lange diskrete Wiederholungen.

Slapper ST dient nur als vom Auftraggeber benannte konzeptionelle Referenz für getrennte Taps. Es liegen keine geprüften Messungen oder Originaldateien dieses Plugins vor; keine nachgemessenen Eigenschaften behauptet und kein Code/Asset daraus übernommen.

Phase 2.3 bleibt abgeschlossen: keine neue HTML-Seite und keine GIF-Demopakete. Die CI erzeugt für diese Phase ausschließlich Build, native Screenshots, technische Protokolle und IR-Messmedien. Kein Merge nach main, kein Release, keine öffentliche Veröffentlichung.

### Reproduzieren ohne Plugin-Build

```sh
cmake -S . -B build-ir -DSPATIALLAB_BUILD_PLUGIN=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build-ir --parallel 2
ctest --test-dir build-ir --output-on-failure
./build-ir/SpatialLabEnvironmentTests build-ir/impulses > build-ir/environments.csv
```

Unter Windows bei Visual-Studio-Generator entsprechend `--config Release`, `ctest -C Release` und `build-ir/Release/SpatialLabEnvironmentTests.exe` verwenden. Der Export verwendet einen unveränderten Einheitspuls identisch links/rechts, neutrale Makros, keine Normalisierung, drei frühe Referenztests und alle 38 Messantworten. Zusätzliche Auswertungen und abgeschnittene Darstellungen sind keine eigenen Raummodelle.

Der informative DSP-Benchmark deckt alle 19 Räume bei beiden Zielraten, Mono-/Stereo-Anregung und konstanten/automatisierten Makros sowie Presetwechseln ab. Die Messwerte sind Maschinen-/Build-abhängig und haben keine universelle CPU-Prozent-Garantie. Die Screenshot- und IR-Artefakte werden getrennt bereitgestellt; kein Browserpaket wird erzeugt.

Für Phase 3B muss die Trennung zwischen stabilem Environment-Key, DSP-Backend und normiertem Host-Transport erhalten bleiben. Insbesondere darf der jetzige 19-Schritt-Parameter nicht blind durch einen gleichmäßig normierten 26er Choice ersetzt werden: Das würde erneut bestehende Automationscodes verschieben. Die Schema-4-Schlüssel sichern die Session-Zuordnung; der zusätzliche Host-Transport benötigt vor der nächsten Erweiterung eine ausdrücklich getestete versionierte Lösung. Dies ist eine bekannte Voraussetzung der späteren Erweiterung, kein bereits implementiertes Outdoor-Verhalten.


### Lokaler CPU-Vorlauf

Release, GCC 14.2.0, Linux x86-64; je Messfall vier Sekunden Audio, 64-Sample-Parameter-Refresh, alle 19 Räume, Mono/Stereo. Vorbereitung/Speicherreservierung, Host und GUI sind nicht in dieser reinen DSP-Zeit enthalten. [Alle 228 Rohwerte](cpu-local.csv).

| Rate | konstant: Median | Makro-Automation: Median | Presetwechsel: Median |
| --- | ---: | ---: | ---: |
| 44,1 kHz | 0,176 % | 0,189 % | 0,190 % |
| 48 kHz | 0,190 % | 0,204 % | 0,205 % |

Prozent eines CPU-Kerns auf dieser Ausführungsmaschine; größte gemessene Einzelmessung 0,506 %. Kurze Laufzeiten unterliegen Scheduler-/Takt-Schwankungen. Dies ersetzt keine Messung im Zielhost und keine Worst-Case-Deadline-Prüfung kleinster Audioblöcke.


### Verfahren für echte GUI-Nachweise

Der separate Windows-Prüfhost öffnet den unveränderten Produktionseditor mit JUCEs Software-Fenster-Renderer. Screenshots stammen aus Win32-Desktop-Pixeln (BitBlt), nicht aus einer nachträglich zusammengesetzten Frontplatte. Vor der Aufnahme werden ausstehende Repaints ausgeführt. Drei statische Metallstellen werden gegen den gleichzeitig erzeugten Komponenten-Render geprüft: Ist der Desktop verdeckt, wird die Aufnahme verworfen und begrenzt erneut versucht. Ein Komponentenbild ersetzt niemals den Desktop-Screenshot; die bestehenden Sichtbarkeits-/Restglut-Grenzen bleiben unverändert. Diese Maßnahmen betreffen ausschließlich `Tools/GuiPreview.cpp`, nicht den Renderer des Plugins im Zielhost.


## Abgeschlossene Windows-/Linux-Abnahme

Geprüfter Branch-Code: `33dde1b07ad4c4c4716054000aea868213d7ae6c`. Actions prüfte den virtuellen PR-Merge-Commit `0fe9d269a0101d1aeb552ec804ffbb29260f620e`; beide besitzen exakt denselben Git-Tree `206538e7f6a046240589514e730db15ba8a68f30`. Dies ist ein GitHub-Prüfcommit, kein Merge des Entwicklungsbranches in main. Der anschließende Nachweiscommit ergänzt ausschließlich Dokumentation/CSV/README.

[CI 43865790113](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43865790113), JUCE 8.0.6 am festen SHA, CMake 3.31.6, VS 2022 x64 / Ubuntu 24.04:

| Prüfung | Ergebnis |
| --- | --- |
| Windows Release | 6/6 bestanden, 17,31 s: Environments, DSP, RetroMotion, RoomVoicing, Processor, RetroGUI |
| Linux ASan/UBSan mit Leak-Prüfung | 4/4 bestanden, 36,67 s; keine Sanitizer-Meldungen |
| Originalgrafiken und eingefrorene Audio-Referenz | SHA-Prüfungen bestanden; ursprüngliche drei neutrale Antworten pro Compiler bei beiden Zielraten sampleweise bitgleich |
| Alle 19 Umgebungen | je Zielrate 19 unterschiedliche IR-Signaturen, endliche begrenzte Antworten, Stereoanteil, ausklingende Fahnen, SIZE/TONE wirksam |
| Routing/State | alle 19 Räume, Mono→Mono/Mono→Stereo/Stereo→Stereo, Blockgrößen 0/1/7/64/511/2048; 19 Roundtrips, Legacy-Schemata/Anker und ungültige Schlüssel geprüft |
| Echtzeit-/GUI-Lifecycle | keine gezählten new/new[]-Allokationen im automatisierten DSP-Stress; 32 geschlossene Instanzen ohne GUI-Telemetrie; sichtbare/unsichtbare Editoren geprüft |
| Beleuchtung | alle acht Kombinationen aus Skin/Größe/Stille/Audio bestanden; individuelle Röhren-/Glutänderung und Restglut, unveränderte statische Leuchtfelder |
| CRT/Screenshots | 19 unterschiedliche Geometrien je Skin; 38 native Desktop-PNGs, jeweils 1504×846; CRC/Abmessungen nach Download erneut geprüft |
| Windows-Paket | beide ZIP-Ebenen per CRC geprüft; genau ein vollständiger SpatialLab.vst3-Ordner; PE-Maschine x64 und alle drei Strm/SpLb-CIDs verifiziert |

Die Begrenzung/Allokationsprüfung ist ein automatisierter Belastungstest, kein Beweis subjektiver Klickfreiheit oder jedes denkbaren Host-/Deadline-Verhaltens. Bei Raumwechseln ist die dokumentierte kurze Wet-Absenkung beabsichtigt. GUI-Prüfungen bestätigen Verhalten/Zustand und tatsächliche Bildänderungen; eine allgemeine CPU-Garantie für jeden Grafiktreiber folgt daraus nicht.

### Nachgewiesene akustische Unterschiede

[Windows-Messwerte aller 38 Fälle](environments-windows.csv), identischer Einheitspuls L=R, SIZE/TONE 50 %, Float32-Stereo ohne Normalisierung. Beispiele bei 44,1 kHz:

| Umgebung | T20-basierte RT60-Schätzung | Wet-Spitze pro Kanal |
| --- | ---: | ---: |
| Kleinwagen | 0,0415 s | 0,3422 |
| Transporter | 0,1561 s | 0,4500 |
| Kleine Kapelle | 1,2709 s | 0,2280 |
| Kathedrale | 3,2373 s | 0,1660 |
| Höhle | 0,8990 s | 0,3968 |
| X-Wing Cockpit | 0,0671 s | 0,4284 |
| Y-Wing Cockpit | 0,1016 s | 0,4826 |
| Snowspeeder Cockpit | 0,0453 s | 0,3744 |

Die Höhle setzt stärker auf späte einzelne Rückwürfe und schwächere Diffusion, die Kathedrale auf eine längere diffuse Fahne; beide haben eigene Laufzeit-/Dämpfungsverteilungen. Unterschiede in Pegel und Energie bleiben erhalten. Diese Zahlen und eindeutige IRs zeigen technische Unterschiede, garantieren aber keine subjektiv überzeugende Raumidentität. Fiktive Cockpits sind kreative Voicings.

### Windows-CPU-Messung

[228 Windows-Rohwerte](cpu-windows.csv), gleiche vier Sekunden Audio und Messbedingungen wie im lokalen Vorlauf; Prozent eines Kerns, ohne prepare, Host und GUI:

| Rate | konstant: Median | Makro-Automation: Median | Presetwechsel: Median |
| --- | ---: | ---: | ---: |
| 44,1 kHz | 0,221 % | 0,254 % | 0,227 % |
| 48 kHz | 0,239 % | 0,273 % | 0,247 % |

Größter Mittelwert eines viersekündigen Messfalls 0,497 %. Kurzzeitige Reset-Spitzen und Echtzeit-Deadline-Grenzen werden von solchen Durchschnittswerten nicht aufgelöst. Die Messungen sind informativ, keine hardwareübergreifenden Grenzwerte.

### Downloads und Installation

- [Installationspaket](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43865790113/artifacts/14226906488): Actions-ZIP entpacken, darin `SpatialLab-VST3-Windows.zip` vollständig entpacken. Den kompletten Ordner `SpatialLab.vst3` bei geschlossener DAW nach `C:\Program Files\Common Files\VST3\` kopieren. Nicht nur die innere Moduldatei kopieren. Danach neu scannen, insbesondere bei zwischengespeicherter alter Parameter-Schrittzahl.
- [Screenshots Episode I/IV, alle 19 Umgebungen](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43865790113/artifacts/14226960903): `Episode-I/Room-0..18/Windows-Editor-Screenshot.png` beziehungsweise `Episode-IV/...`; Indizes entsprechen der obigen Liste. Originale unbeschnittene Desktop-PNGs. Visuell geprüft: beide vollständigen Skins sowie alle 19 CRT-Geometrien.
- [Impulsantworten/Messdaten/Presets](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43865790113/artifacts/14226518817): 38 Stereo-Float32-WAVs bei 44,1/48 kHz, CSV und vollständige Preset-Tabelle.
- [Vollständige Build-Provenienz und Protokolle](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43865790113/artifacts/14226364931): VST3, Toolchain, CPU-/IR-/Bass-/Referenzmessungen, CTest-Protokoll und Lichtprüfbericht. Der Name trägt den virtuellen PR-Prüfcommit.

Downloads benötigen angemeldeten Repository-Zugriff; aktuelle Actions-Aufbewahrung bis 14.02.2027. Keine Medien dauerhaft eingecheckt, keine neue HTML-Demo, keine GIF-Präsentation, kein öffentliches Hosting, kein Release und kein Merge.

SHA-256 des inneren Installations-ZIP: `fca72f8edd00f134bab00e0a206d25c5fd03f265ce82512ffe5b26ffad090fa9`.

SHA-256 des tatsächlich enthaltenen Windows-x64-Moduls: `7047bc8fdd52be72b73fca43212a70086495bd9067de90069e06c2b0ee218cc9`.

### Noch beim Anwender zu prüfen

Hörfreigabe mit eigener Sprache/Geräuschen in Fender Studio Pro: Raumidentität, Verständlichkeit, natürliche lange Fahnen, Tiefton-/Resonanzfärbung der Fahrzeuge/Cockpits und Wechsel bei starken Transienten. Alte reale Studio-Pro-Projekte einschließlich Automation nach Rescan öffnen und vergleichen; automatisierte Zustandstests ersetzen diese Host-Abnahme nicht. Alte frei interpolierte Raum-Automationskurven haben ausdrücklich die oben beschriebene Einschränkung. Neue Schema-4-Sitzungen sind nicht rückwärts in alte Plugin-Versionen ladbar.
