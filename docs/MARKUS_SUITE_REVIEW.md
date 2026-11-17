# Markus-Feedback: Prüfung der Plugin-Suite

Geprüfte Ausgangsstände am 17. November 2026:

| Plugin | Quellstand | Markus-Vorschlag im Code | Veröffentlichter Stand zu Prüfungsbeginn |
|---|---|---|---|
| Locations | `db1368c` / 1.20 | Weniger Hall draußen, Echo-Zeit ×0,25–×4 in ORT A; Innenorte deaktivieren den Regler | Noch 1.18; Außenraum-Änderungen fehlten im Download |
| Doppelganger | `58051c7` / 1.2 | Fünf zusätzliche Glockenfilter, Knoten ziehen, Mausrad Q, Umschalt fein, Tooltip; EQ-Review bereits gemergt | 1.2, bereits enthalten |
| SpatialLab | `e4a0765` / 1.1.0 | Eigene Outdoor-Engine mit diskreten Echos statt dichtem Nachhall. SIZE verändert deren Laufzeiten | 1.1.0, bereits enthalten |
| Radiophone | `dea6bb8` / 1.5 | Radio-Aus mit Klick und Stille; drei zufällig gewählte, nicht direkt wiederholte Senderwechsel | Noch 1.4; neue Radio-Funktionen fehlten im Download |
| Dreamcatcher | `c2be992` / 2.1 | Traum-Verstimmung durch weich überblendeten Detune ersetzt | 2.1, bereits enthalten |

## Was verifiziert wurde

- Locations' Außenpresets sind im Code deutlich trockener. Die Echo-Zeit skaliert nur Ort A, Wald/Straße/Schlucht; z. B. Schlucht 420 ms ×4 = 1,68 s. Das ist eine klangliche Abstimmung und keine Zusicherung eines exakten Hall-/Echo-Verhältnisses von 1:5.
- Radiophone hält nach Abschalten und Ende des Abschaltklicks Stille. Die DSP-Tests prüfen das sowie alle drei Senderwechsel, ohne direkte Wiederholung. Die zwei zusätzlichen Samples sind Tempo-Varianten derselben Aufnahme, keine unabhängig neu aufgenommenen Senderwechsel. Alle eingebetteten WAV-Dateien haben tatsächlich 44.100 Hz und einen Kanal.
- SpatialLab verwendet für alle sieben Outdoor-Profile ausschließlich diskrete Echos mit begrenztem Feedback. Ein gesonderter Echo-Zeit-Regler existiert nicht; SIZE streckt die Laufzeiten um Faktor 0,5–2. SpatialLab ist ein 100-%-Wet-Send-Effekt; dessen stummer Bypass ist ausdrücklich beabsichtigt und bereits getestet.
- Dreamcatcher 2.1 enthält bereits die Reparatur des Traum-Pitch-Bausteins. Die vorgeschlagene Ducking-Ursache hat sich im damaligen Codewechsel nicht bestätigt. Neue Tests untersuchen den synthetischen Ausklang mit Ducking aus/an bis zwölf Sekunden und verlangen deutlich abfallende Restenergie; ein individueller DAW-Hörtest wird dadurch nicht ersetzt.
- Doppelgangers aktueller Stand enthält die zuvor geprüfte EQ-Korrektur einschließlich Mausrad-Q. Die DSP-Regressionen wurden nochmals ausgeführt; ohne weiteren bestätigten Fehler ist dafür kein zusätzlicher DSP-Umbau erforderlich.

## Zusätzliche technische Befunde und Änderungen

### Locations 1.20

- Ein Mono-Callback größer als der reservierte Scratch-Puffer verließ die Funktion unverarbeitet. Jetzt werden beliebig große Host-Blöcke in vorreservierten Abschnitten vollständig bearbeitet. Ein Prozessortest vergleicht 32.769 Samples bei angekündigten 64 Samples mit der vollständigen DSP-Referenz.
- Die pauschale 3-s-Nachklangmeldung war für die neue maximale Schlucht-Echozeit zu kurz. Sie berücksichtigt nun Preset, Echo-Faktor und Feedback, damit exportierende Hosts nicht zu früh abschneiden.
- Ungültige Eingangswerte/Parameter werden abgefangen. Reset initialisiert die Steuer-/Wetterzustände neu.
- Portable CTest-DSP-Tests und JUCE-Prozessortests ergänzt; Windows-Release führt CTest vor pluginval aus.

### Dreamcatcher 2.2

- Der bisherige Reset führte zwei neue Heap-Allokationen aus (`GlitchShifter::reset()` rief Prepare auf). Die bestehenden Vektorpuffer werden jetzt wiederverwendet. Der Vergleich wurde per `operator new`-Zähler reproduziert.
- Keine `triggerAsyncUpdate()`-OS-Nachricht mehr aus dem Audio-Callback. Eine Message-Thread-Timer-Abfrage veröffentlicht geänderte Geist-Latenz; die DAW-Latenzänderung bleibt wie bisher asynchron.
- Echo-/Reverse-/Seiten-Ringpositionen werden pro Sample auf den Ring begrenzt. Die früher endlos steigenden signed-int-Zähler könnten in mehrstündigen Sitzungen überlaufen.
- Reset initialisiert die Modus-/Steuerzustände neu; Steuerparameter und Eingang werden geprüft. Werte jenseits ±4 am Eingang werden zum Schutz der ganzzahligen Glitch-Verarbeitung begrenzt.
- CTest für DSP und JUCE-Prozessor ergänzt, einschließlich Allokationsschutz bei Moduswechsel/Reset und Parameter-State-Rundreise.

### SpatialLab 1.1.1

- Die Outdoor-Engine las auch nach abgeschlossener Überblendung beide Leseköpfe; bei stabiler Laufzeit werden die redundanten Zugriffe übersprungen. Die bereits begrenzten Leseindizes benötigen keine Divisions-/Modulooperation.
- Direkter Vergleich der Float-Hashes aller sieben Outdoor-Profile, je 150.000 Stereo-Samples inklusive SIZE-/TONE-Wechsel: identisch. Die 19 Indoor-Profile werden weiterhin gegen die eingefrorene Referenz getestet.
- Die DSP-Grenze fängt nicht-endliche Samples und Steuerwerte ab. Null-Sample-Callbacks enden vor Puffer-/Meterzugriffen. Neue Grenzfalltests prüfen alle 26 Presets bei fünf Sample-Rates.
- Der historische Quellcode-Freeze für die frühere reine Classic-GUI-Änderung wurde ausdrücklich auf den neuen 1.1.1-Quellstand fortgeschrieben. Originalgrafiken, Parameter-/Plugin-Identität und eingefrorene Phase-Referenzen bleiben unverändert; keine Prüfung wurde entfernt.

### Radiophone 1.5

Die vorhandene technische Prüfung aus 1.4 und die Radio-Funktionen aus 1.5 bestehen in den DSP-Tests. Ergänzt ist die explizite State-Wiederherstellung des Radio-Ein/Aus-Parameters; keine unbegründete Änderung des bereits geprüften Klangkerns.

## Performance-Einordnung

Die Outdoor-Optimierung reduziert strukturell Pufferlesevorgänge und teure Modulo-Operationen. Ein enger Mikrobenchmark (GCC 14.2, `-O3 -DNDEBUG`, 48 kHz, Waldprofil, eine Million Frames, fünf abwechselnde Vorher-/Nachher-Läufe in der Cloud) ergab Median 325,408 → 304,874 ns/Frame, identische Prüfsumme. Die Streuung der Einzelwerte ist erheblich; daraus wird keine garantierte DAW-CPU-Ersparnis abgeleitet. Durchschnittliche CPU-Werte sind kein Nachweis einer garantierten Echtzeitgrenze.

Keine pauschale Abschaltung der gesamten Effektkette bei Stille: Filter, Echos und Hall sollen korrekt ausklingen. Klangentscheidungen wie das Verhältnis von Stimme, Hall und Echo bleiben durch Hörkontrolle zu beurteilen. Messwerte und Signaltests ersetzen keine Abnahme mit Markus' Originalmaterial.

## Testumfang und Veröffentlichung

Vorhandene Radiophone-DSP-Tests und vier SpatialLab-Suites bestanden unverändert. Neue Locations-/Dreamcatcher-DSP-Suites sowie Radiophone und die fünf SpatialLab-Suites bestanden mit AddressSanitizer/UndefinedBehaviorSanitizer (bei Locations/Dreamcatcher/Radiophone zusätzlich Float-Cast-Overflow-Prüfung). Alle prüfen tatsächliche Ausgabewerte; Allokationszähler sind auch im Release-Build aktiv.

Die vollständigen lokalen Plugin-/Prozessortests und pluginval werden zusätzlich ausgeführt. Windows-Releases werden erst durch ihre erfolgreich abgeschlossenen Build-/Regression-/pluginval-Schritte freigegeben. Der endgültige Status und die Download-Links stehen in den jeweiligen GitHub-Releases und Pull Requests.

Die Veröffentlichung der geprüften neuesten Versionen wurde vom Eigentümer ausdrücklich beauftragt. Alte Releases werden nicht gelöscht. Für Doppelganger ist 1.2 bereits aktuell; ein inhaltsgleiches neues Release ist nicht notwendig.
