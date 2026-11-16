# Phase 3B – Outdoor-Reflexionen und konsistenter Katalog

[Windows-VST3 herunterladen](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43941486329/artifacts/14264897381) · [52 echte Desktop-Screenshots](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43941486329/artifacts/14265042096) · [Native Maximalgrößen-Render](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43941486329/artifacts/14263857689) · [Erfolgreicher CI-Lauf](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43941486329)

Die verbindliche Reihenfolge und DSP-Zuordnung steht in [catalog.json](catalog.json). Indizes 0–8 sind Innenräume, 9–15 Außenräume, 16–19 Fahrzeuge, 20–25 Spezialräume einschließlich Cockpits. GUI und DSP verwenden Source/RoomCatalog.h. Der vorhandene Browser bleibt erhalten und übernimmt lediglich dessen vier Kategorien.

Der Raumparameter ist jetzt linear mit 26 Schritten normiert (Index/25). Die frühere Sonderkodierung wurde mit ausdrücklicher Zustimmung entfernt. State-Schema 5 speichert stabile Keys und Makros; ältere Test-Session-Schemata werden ohne Zustandsänderung abgewiesen. Alte Raumautomation ist nicht kompatibel. SIZE/TONE-IDs und Plugin-Identität bleiben erhalten. Ab dieser Liste sind Indizes und Keys verbindlich; zukünftige Erweiterungen benötigen eine ausdrücklich geprüfte Strategie.

Die 19 bisherigen RoomSpec-Datensätze und der eigentliche Indoor-Raumprozessor bleiben unverändert. Die eingefrorene vollständige Phase-3A-Engine dient für alle 19 neutralen Referenzantworten bei 44,1/48 kHz als bitgenaue Gegenprobe, unter der neuen Zuordnung.

## Outdoor-Engine

Eigenständige Stereo-Multi-Tap-Verarbeitung ohne Faltung oder externe Dateien. Ein gemeinsamer Stereo-Ringpuffer wird in prepare für drei Sekunden reserviert. Bis zu 16 Taps, davon 2/8/4/6/4/6/5 in den sieben Profilen. Jeder Tap besitzt Zeit, Pegel, Panorama, Tiefpass und Feedback. Sämtliche Werte stehen kompiliert in Source/OutdoorEngine.h; Source/OutdoorPlan.h enthält den gemeinsamen validierten Datenvertrag. Keine herangezogenen proprietären Presets oder fremder Slapper-Code.

SIZE skaliert Laufzeiten um Faktor 0,5–2. Zwei feste Lesezeiten werden über 50 ms überblendet; weitere Ziele werden anschließend übernommen, ohne die Überblendung durch jede Blockaktualisierung neu zu starten. Keine sprunghaft umgesetzten Lesezeiger. TONE skaliert Tap-Cutoffs um 0,5–2; Pole werden geglättet. Neutrale Stellung jeweils 50 %. Sehr kleine Filterzustände unter 1e-20 werden auf null gesetzt, um Denormal-Last zu vermeiden.

Feedback wird über einen gemeinsamen Bus geführt. Die Summe absoluter Feedbackwerte beträgt maximal 0,42; Stereo-Crossfeed, lineares Panorama, Tiefpass und lineare Leseinterpolation sind kontraktiv. Auch während einer SIZE-Überblendung bleibt die Gesamtschleife deshalb streng unter eins. Kein Limiter als Ersatz für Stabilität, keine Normalisierung und kein Dry-Anteil. Nur der aktive Backend-Pfad läuft. Die bestehende 30-ms-Aus-/Einblendung mit Reset am Nullpunkt schützt Indoor-/Outdoor-Wechsel; die kurze Wet-Absenkung ist beabsichtigt.

| Umgebung | Charakter |
|---|---|
| Offene Landschaft | Zwei sehr schwache entfernte Reflexionen, kein Feedback, bewusst geringe Wet-Energie |
| Wald | Acht unregelmäßige gedämpfte Rückwürfe, kein geschlossener Hall und kein Feedback |
| Straße/Gasse | Vier klar seitliche Fassadenreflexionen, keine diffuse Fahne |
| Innenhof | Sechs umschließende Wege und moderates Feedback |
| Canyon | Vier getrennte Echos bei neutral 310/570/890/1210 ms, zunehmend dunkler und leiser |
| Tunnel | Gerichtete Reflexionsfolge, Vorzeichenwechsel und größtes begrenztes Feedback-Budget |
| Unterführung | Starke frühe Deckenreflexion, fünf kurze Wege, wesentlich kompakter als Tunnel |

Die Abstimmungen sind kreative Hörspiel-Voicings, keine gemessenen realen Orte. Sprachverständlichkeit und subjektive Raumidentität benötigen die Anwenderabnahme im Zielhost.

## GUI

Standard 1504×846 und Minimum 1003×565 bleiben erhalten; Maximum jetzt 2174×1223. Alle Inhalte verwenden die bestehende proportionale Skalierung und Größenwiederherstellung. Beide Original-PNGs bleiben unverändert. Keine neuen Bedienelemente; keine separate SCI-FI-Kategorie. Sieben zusätzliche eigene bernsteinfarbene technische Zeichnungen ergänzen links Landschaft, Wald, Fassaden, Hof, Canyon, Tunnel und Unterführung. Die bisherigen Zeichnungen werden über die neue Zuordnung wiederverwendet.

## Prüfung und Lieferung

Die Environments-Prüfung exportiert reproduzierbare Float32-Stereo-Impulsantworten und CSV ohne Normalisierung; diese sind Messmedien, keine Faltungsressourcen des Plugins. Alle 26 Antworten werden auf Endlichkeit, Unterschiede, Makrowirkung, Stereo und Ausklingen geprüft. Zusätzlich laufen die bisherigen DSP-Allokations-/Automations-, Routing-, Session-, GUI- und Beleuchtungstests. Fenstergrößen einschließlich neuem Maximum und dessen Wiederherstellung werden geprüft. Windows erfasst echte Desktop-Pixel beider Skins bei Standard und Maximum; kein HTML-Ersatz.

CI-Ergebnisse und Download-Links stehen im folgenden Abschlussnachweis. Kein Merge, kein Release.


## Reproduzierbare Messung

```sh
cmake -S . -B build-ir -DSPATIALLAB_BUILD_PLUGIN=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build-ir --parallel 2
ctest --test-dir build-ir --output-on-failure
./build-ir/SpatialLabEnvironmentTests build-ir/impulses > build-ir/environments.csv
python3 Tools/measure_outdoor.py build-ir/impulses > build-ir/outdoor-temporal.csv
```

Anregung ist ein identischer Einheitspuls L=R, SIZE/TONE jeweils 50 %, keine Normalisierung, Float32-Stereo. Der Mess-Export prüft zusätzlich Makroextreme und vergleicht die ursprünglichen 19 Räume bei beiden Zielraten sampleweise mit dem eingefrorenen Vorgänger. Mess-WAVs sind ausschließlich Prüfmedien und werden nicht im Plugin eingebettet.

Bei diskreten Outdoor-Echos ist die T20-basierte RT60-Schätzung in der allgemeinen CSV **keine klassische Nachhallzeit**: Sie beschreibt den zeitlichen Energieverlauf mehrerer Rückwürfe. Die zusätzliche temporale Messung nennt deshalb den ersten/letzten Pegel über −80 dB relativ zur jeweiligen Spitze sowie den Zeitpunkt von 95 % kumulierter Energie. Die Schwelle dient ausschließlich der Messung, nicht einer Anpassung des Audiosignals.

[Lokale Rohwerte aller 52 Antworten](environments-local.csv) · [14 temporale Outdoor-Messungen](outdoor-temporal-local.csv). Beispiel 44,1 kHz:

| Profil | Erster signifikanter Rückwurf | Letzter über −80 dB relativ | 95 % Energie bis | Spitze |
|---|---:|---:|---:|---:|
| canyon | 0.3100 | 2.6702 | 0.6200 | 0.1075 |
| courtyard | 0.0310 | 0.5382 | 0.1370 | 0.0903 |
| forest | 0.0230 | 0.3901 | 0.1490 | 0.0237 |
| open-land | 0.1900 | 0.4307 | 0.4300 | 0.0044 |
| street | 0.0190 | 0.1133 | 0.0710 | 0.1216 |
| tunnel | 0.0270 | 0.4451 | 0.1171 | 0.0791 |
| underpass | 0.0070 | 0.1202 | 0.0290 | 0.0777 |

Auch die bewusst schwache Landschaft ist eine reine Wet-Antwort. Eine längere zeitliche Streuung im Wald ist nicht als geschlossene diffuse Hallfahne zu interpretieren. Die Werte sind algorithmische Messungen und kein physikalisches Raumgutachten.


## Verbindliche Zuordnung (nullbasierter interner Index)

| Index | Umgebung | DSP-Profil |
|---|---|---|
| 0 | Wohnzimmer | indoor 0 |
| 1 | Küche | indoor 1 |
| 2 | Badezimmer | indoor 2 |
| 3 | Schlafzimmer | indoor 3 |
| 4 | Flur | indoor 4 |
| 5 | Treppenhaus | indoor 5 |
| 6 | Keller | indoor 6 |
| 7 | Dachboden | indoor 7 |
| 8 | Garage / Werkstatt | indoor 8 |
| 9 | Offene Landschaft | outdoor 0 |
| 10 | Wald | outdoor 1 |
| 11 | Straße / Gasse | outdoor 2 |
| 12 | Innenhof | outdoor 3 |
| 13 | Schlucht / Canyon | outdoor 4 |
| 14 | Tunnel | outdoor 5 |
| 15 | Unterführung | outdoor 6 |
| 16 | Kleinwagen | indoor 9 |
| 17 | Großer PKW | indoor 10 |
| 18 | Transporter | indoor 11 |
| 19 | LKW-Kabine | indoor 12 |
| 20 | Kleine Kapelle | indoor 13 |
| 21 | Kathedrale | indoor 14 |
| 22 | Höhle | indoor 15 |
| 23 | X-Wing Cockpit | indoor 16 |
| 24 | Y-Wing Cockpit | indoor 17 |
| 25 | Snowspeeder Cockpit | indoor 18 |

## Erfolgreiche Abnahme und Grenzen

Geprüfter Plugin-Code: `b23ba437fa8536f04790d9ce445f9383997c9016`. Actions prüfte den virtuellen PR-Commit `138da568d00357641188eba3bb3397d91309a94c`; beide besitzen denselben Tree `0e69f21e6b7a45726fcd88f5a954a835f82d3103`. Kein Merge nach main. Nachfolgende Commits ergänzen nur Berichte, Messauswertung und kontingentschonende CI-Auslösung, nicht den Plugin-Code.

| Prüfung | Ergebnis |
|---|---|
| Windows Release | 6/6 Tests bestanden, 28,21 s |
| Linux ASan/UBSan einschließlich Leak-Prüfung | 4/4 bestanden, 260,48 s |
| Alle 26 Umgebungen | Verschiedene endliche Stereoantworten, SIZE/TONE-Wirkung, begrenzte Samples und ausklingende neutrale Antworten bei 44,1/48 kHz |
| Alte 19 Profile | Sampleweiser Vergleich mit eingefrorener Phase-3A-Engine bestanden; nach Download zusätzlich alle 38 alten Windows-WAVs über volle Exportlänge bytegleich zu Phase 3A |
| Feedback | Alle sieben Profile validiert; gemeinsame absolute Feedback-Summe <=0,43, tatsächlich maximal 0,42; keine instabilen Extremwerte im Makro-/Wechselstress |
| Audiothread | Kein gezähltes new/new[] bei automatisierten Wechseln über alle 26 Presets; Mono→Mono, Mono→Stereo, Stereo→Stereo und bestehende Blockgrößentests bestanden |
| Sessions | 26 Roundtrips in Schema 5 und Ablehnung ungültiger Zustände bestanden; keine Kompatibilitätsbehauptung für alte Test-Sessions |
| Browser/GUI | Alle 26 Direktwahlen und 26 unterschiedliche Miniaturgrafiken pro Skin; vier Kategorien aus dem zentralen Katalog; Wheel/Scroll/Host-Recall und Größen bis 2174×1223 inklusive Größenwiederherstellung bestanden |
| Beleuchtung | Alle acht echten Zustände bei 1504×846 und 1672×941, Episode I/IV, Stille/Audio bestanden |
| Originale/Identität | Episode-PNGs SHA-identisch; tatsächliche Hersteller- und alle drei VST3-CIDs verifiziert |
| Auslieferung | ZIP-CRC beider Ebenen, vollständiger Bundle-Ordner, Windows-x64-PE und PNG-Abmessungen nach Download geprüft |

Ein Randfehler durch Float-Rundung am Delay-Ringende wurde vom Sanitizer erkannt und vor dieser Abnahme mit doppelter Positionsgenauigkeit und begrenzten Indizes behoben. Der erfolgreiche Lauf enthält diese Korrektur.

**Bildnachweis-Einschränkung:** Der Windows-CI-Desktop unterstützt nur 1920×1080. Der Versuch 2560×1440 meldet DISP_CHANGE_BADMODE (-2). Die 52 Standardansichten sind echte Desktop-Screenshots. Für das neue Maximum liegen echte native JUCE-Komponentenrender beider Skins mit exakt 2174×1223 vor, keine vergrößerten Standard-Screenshots. Diese Dateien sind ausdrücklich **keine Desktop-Screenshots**; die Fähigkeitsprotokolle und NO-DESKTOP-CAPTURE.txt liegen im Artefakt. Ein vollständiger Desktop-Nachweis bei Maximalgröße bleibt auf einem ausreichend großen Zielmonitor nachzuholen. Komponentenlayout, Eingaben und Größenwiederherstellung bei diesem Maximum sind automatisiert geprüft.

[Windows-Rohmessungen aller 52 Fälle](environments-windows.csv) · [Temporale Outdoor-Auswertung des Windows-Exports](outdoor-temporal-windows.csv) · [312 CPU-Rohwerte](cpu-windows.csv). Der CPU-Benchmark misst jeweils vier Sekunden Audio ohne prepare, GUI oder Host. Durchschnittswerte ersetzen keine Prüfung kurzfristiger Reset-Spitzen: Ein Raumwechsel löscht vorbereitete Puffer am Nullpunkt; sehr kleine Blockgrößen/hohe Raten sind im Zielhost praktisch zu prüfen. Die primären Qualitäts-/CPU-Ziele bleiben 44,1/48 kHz.

Subjektiv zu prüfen sind Sprachverständlichkeit, überzeugender Outdoor-Charakter, SIZE-/TONE-Verlauf und Umschalten bei starken Transienten. Zahlen und BIBO-Begrenzung garantieren keine subjektive Klangfreigabe.

## Downloads und Installation

- [Windows-VST3-Testbuild](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43941486329/artifacts/14264897381): Actions-ZIP entpacken, darin SpatialLab-VST3-Windows.zip vollständig entpacken. Bei geschlossener DAW den gesamten SpatialLab.vst3-Ordner nach `C:\Program Files\Common Files\VST3\` kopieren. Neue Test-Session verwenden; alte Raumautomation/States sind ausdrücklich inkompatibel.
- [52 native Desktop-PNGs](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43941486329/artifacts/14265042096): Episode-I/Room-0..25 und Episode-IV/Room-0..25, je 1504×846. Indizes siehe Tabelle.
- [Maximalgröße 2174×1223](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43941486329/artifacts/14263857689): native Komponentenrender und Desktop-Fähigkeitsprotokolle beider Skins, klar als Render gekennzeichnet.
- [52 Mess-WAVs und CSV](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43941486329/artifacts/14264562469).
- [Vollständige Build-Provenienz, CPU- und Prüfprotokolle](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43941486329/artifacts/14264017727).

SHA-256 inneres Installations-ZIP: `61d0d6866e9bcc4fd9884d0a94f18939d50079e41d92b7921887f014deec2db0`.

SHA-256 Windows-x64-Modul: `ff5562b606df9faf3d00c003388f2c2dfe4ba0515e9eead0ad63bac294429850`.

## CI-Kontingent

[Neue sparsame Ausführung und manuelle Abnahmebefehle](../ci/ACTIONS_BUDGET.md): keine Push-/PR-Doppelung; automatisch nur drei kurze Linux-Sanitizer-Tests bei relevanten PR-Änderungen. Vollständige Windows-/GUI-/Linux-Abnahme einschließlich langem 26-Umgebungen-Test nur gezielt per workflow_dispatch beziehungsweise dem dokumentierten ci:full-PR-Label. Alle Prüfungen bleiben erhalten; keine neuen Releases oder Merge-Aktionen. Für die anschließende Trigger-/Dokumentationsänderung wurde kein weiterer Vollbuild gestartet.

Die Workflowdatei fehlt derzeit auf main; deshalb ist bis zu einer späteren freigegebenen Übernahme der Labelweg auf Draft-PR #2 die verfügbare gezielte Abnahme. Das Repository-Label wurde angelegt, aber nicht dem PR hinzugefügt. Dadurch wurde kein neuer CI-Lauf gestartet.
