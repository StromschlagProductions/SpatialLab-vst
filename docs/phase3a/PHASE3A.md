# SPATIAL LAB Phase 3A

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

Der Mess-Export erstellt 38 Stereo-Float32-Impulsantworten bei 44,1/48 kHz und eine CSV mit Energie, T20-Schätzung, Spitzenwerten und Signaturen. Kein Normalisieren. Die Daten sind reproduzierbare technische Raumantworten, keine physikalische Raumvermessung. [Lokale Messwerte](environments-local.csv). Windows-Resultate und echte Screenshots aller 19 Umgebungen beider Skins werden im Actions-Artefakt bereitgestellt.

Die früheren sofortigen Hard-Reset-Tests sind an die ausdrücklich neue Klickschutz-Ein-/Ausblendung angepasst: nach Ablauf der Wechselzeit darf kein alter Tail zurückkehren. Subjektive Klickfreiheit, Sprachverständlichkeit und angenehm unterscheidbare Fahrzeug-/Cockpitfärbung können automatisierte Zahlen nicht garantieren und bedürfen des Hörtests in Fender Studio Pro.

## Phase 3B: Konzept, keine Outdoor-Presets

Vorgemerkt: offene Landschaft, Wald, Straße/Gasse, Innenhof, Schlucht/Canyon, Tunnel, Unterführung. Keine dieser Umgebungen ist in Phase 3A auswählbar.

`Source/OutdoorPlan.h` definiert nur einen geprüften Datenvertrag für maximal 16 diskrete Taps: Verzögerung, Pegel, Tiefpassfrequenz, Stereo-Panorama und begrenztes optionales Feedback, plus diffuse Beimischung. Kein Prozessor und keine aktive Instanz. Die spätere eigenständige Engine soll alle Speicher in prepare vorallokieren, Tap-Parameter glätten, Delaywechsel über getrennte Lesezeiger entschärfen und kontrollierte Feedbackpfade stabil begrenzen. Tap-Panorama ist Energieverteilung pro Reflexion, keine Bewegungssimulation. Sehr trockene offene Räume können keinen Late-Anteil und nur wenige leise Reflexionen erhalten; Canyon benötigt lange diskrete Wiederholungen.

Slapper ST dient nur als vom Auftraggeber benannte konzeptionelle Referenz für getrennte Taps. Es liegen keine geprüften Messungen oder Originaldateien dieses Plugins vor; keine nachgemessenen Eigenschaften behauptet und kein Code/Asset daraus übernommen.

Phase 2.3 bleibt abgeschlossen: keine neue HTML-Seite und keine GIF-Demopakete. Die CI erzeugt für diese Phase ausschließlich Build, native Screenshots, technische Protokolle und IR-Messmedien. Kein Merge nach main, kein Release, keine öffentliche Veröffentlichung.
