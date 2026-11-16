# Phase 2: kompakte Sprachräume — technischer Hörtestvorschlag

Ausgangspunkt: `49d02df5d9814aa810c8e4d37da459c4c98198b8`. Die vollständige bisherige Engine liegt unverändert bis auf den Namespace unter `Tests/Reference/RoomEnginePhase1.h`. Keine Auswahl eines zusätzlichen Raums oder DSP-Modus im Plugin. Der Vergleich ist ausschließlich offline. Der frühere Windows-Build bleibt über [Lauf 43745098224](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43745098224) nachvollziehbar. Identische Plugin-IDs bleiben erhalten; alte und neue DLL deshalb nicht gleichzeitig unter demselben Host-Namen installieren.

## Abstimmung bei neutraler Größe/Klang

| Raum | RT60 alt → neu | Dämpfungs-Cutoff alt → neu | FDN-Wege alt → neu (ms) | Early-Wege alt → neu (ms) |
| --- | --- | --- | --- | --- |
| Wohnzimmer | 0,48 → 0,40 s | 3600 → 3400 Hz | 29,7–73,9 → 23,3–59,9 | 4,1–31,7 → 5,3–36,7 |
| Küche | 0,85 → 0,32 s | 6200 → 5200 Hz | 23,9–71,9 → 13,7–41,9 | 2,7–27,1 → 2,3–19,1 |
| Badezimmer | 1,35 → 0,38 s | 9000 → 7600 Hz | 19,3–67,1 → 9,7–31,1 | 1,9–21,1 → 1,4–13,1 |

Das Wohnzimmer erhält eine deutlich kräftigere, zeitlich gestaffelte frühe Raumantwort statt eines langen Nachhalls. Etwas spätere frühe Ankünfte, eine breitere Verteilung und ein geänderter Stereo-Anregungsanteil erzeugen eine deutlichere Raumwahrnehmung; die dunkle, kurze Fahne bleibt möbliert. Die Küche bekommt kürzere Umlauf- und Erstreflexionswege, höhere Streuung und begrenzte Höhenfahne. Das Bad behält die helleren, enger gestaffelten signierten Reflexionen harter Flächen, bekommt aber deutlich kürzere Schleifen und erheblich kürzere Fahnen. Kein bloßes Leiserdrehen des Ausgangs.

Zusätzliche interne Presetwerte (keine neuen Regler): Early-/Late-Kalibrierung Wohnzimmer 1,30/1,15, Küche 1,08/0,95, Bad 1,12/0,95; früher Stereo-Queranteil 0,30/0,22/0,14. Die vormals für alle Räume gleichen Tap-Gewichte sind jetzt individuell; die vollständigen Werte stehen in `RoomSpec`. Vier feste paarweise Rotationen (0,48/0,62/0,28 rad) ergänzen die Householder-Streuung. Beide Operationen sind orthogonal, ändern also die Reflexions-/Echodichte ohne zusätzliche Schleifenenergie. Keine Modulation, keine Bewegung, keine variable Delay-Interpolation.

Größe behält seine bisherige kontinuierliche RT60-Skalierung 0,5–2 und Early/Late-Voicing; Klang skaliert den internen Dämpfungscutoff um 0,5–2. Bei maximaler Größe liegen die nominalen RT60 bei 0,80/0,64/0,76 s. Koeffizienten werden weiter geglättet; Speicher bleiben vorallokiert. 100 % Wet, Mono-/Stereo-Routing, IDs und State-Schema bleiben erhalten. Klangänderungen beim Laden alter Sessions sind hier die beabsichtigte Preset-Korrektur, keine Änderung ihrer gespeicherten Parameterwerte.

## Messung und A/B

`SpatialLabRoomComparison` vergleicht die echte eingefrorene alte Engine mit der neuen bei 44,1 und 48 kHz. Gleicher Mono-Impuls, −6 dB Send, FX-Ausgang unverändert bei Unity, Größe/Klang 0,5. Keine Loudness-Normalisierung oder zusätzliche Dry-Beimischung. Optional schreibt das Programm zwölf Stereo-Float-WAVs (3 Räume × 2 Versionen × 2 Raten, je 3 Sekunden) und eine CSV. CI legt diese im Vergleichsartefakt ab. Dies sind Impulsantworten, keine erfundenen Sprach-Hörtests.

[Gemessene Werte](room-metrics.csv): Der über die gesamte Stereo-IR nach Schroeder integrierte T20-RT60-Schätzer fällt von rund 0,33/0,54/0,92 s auf 0,19/0,20/0,22 s. Das ist ein breitbandiger Schätzer mit starker Early-Gewichtung, keine physikalisch gemessene Raum-RT60 und nicht identisch mit dem nominalen tieffrequenten Feedback-Ziel. Beim Wohnzimmer liegt die Energie von 20–80 ms etwa beim 2,5-Fachen der alten Antwort. Küchen-/Bad-Tails nach 500 ms und T20 sind deutlich reduziert. Die automatischen Grenzwerte prüfen kurze, nicht stumme, stabile Antworten und echte Verbesserungen gegenüber der alten Engine, nicht subjektive Natürlichkeit.

Zusätzlich bestehen die bisherigen Tests für beide primären Sampleraten, komplette Größe-/Klang-Spannen, spektrales Verhalten, schnelle Automation, Reset und allokationsfreies Processing. Höhere Raten erhalten weiter Smoke-Checks; kein Oversampling.

**Noch kein klangliches Endurteil:** Dies ist ein fachlich begründeter technischer Vorschlag. Erneuter Hörtest durch den Auftraggeber mit derselben trockenen Sprache, FX 0 dB, Send −6 dB und neutralen Makros ist erforderlich. Dabei insbesondere Wohnzimmer-Präsenz, kompakte Küchenhelligkeit und kurze gekachelte Badreflexionen bewerten. Größe/Klang anschließend auch an den Endpunkten beurteilen.
