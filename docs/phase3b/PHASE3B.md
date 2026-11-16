# Phase 3B – Outdoor-Reflexionen und konsistenter Katalog

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

CI-Ergebnisse und Download-Links werden nach Abschluss ergänzt. Kein Merge, kein Release.
