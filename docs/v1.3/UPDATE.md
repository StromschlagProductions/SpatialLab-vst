# SPATIAL LAB v1.3 – Audio Drama Spatial Environments

## Umfang

GUI-Branding, öffentliche Versionsdarstellung, echte Screenshots, Dokumentation, GitHub-Release und Homepage. Der DSP aus v1.2.0 einschließlich aller 26 Raumabstimmungen bleibt unverändert. AudioProcessor/Schema 5, drei Parameter und deren IDs, Preset-Keys/-Reihenfolge, Buslayouts, Hersteller-/Plugin-/Bundle-IDs sowie Originalfrontplatten und Logo bleiben per Hash geschützt.

## Automatische Versionsquelle

CMake führt intern die Version 1.3.0. `configure_file` erzeugt `SpatialLabBranding.h`; öffentliche Major.Minor-Versionen lassen eine interne Patch-Null weg. Der GUI-Code enthält keine fest eingetragene Versionsnummer. Caption und Herstellerzeile stammen für alle Looks aus derselben generierten Quelle. Classic zeigt die Untertitelzeile in Versalien; die öffentliche Schreibweise ist **Audio Drama Spatial Environments**.

Die neue öffentliche Version lautet **1.3**. Tag `v1.3`, Release-Titel `SPATIAL LAB v1.3` und ZIP `SpatialLab-v1.3-Windows-x64-VST3.zip` enthalten keine unnötige dritte Null oder Build-Suffixe. Der Veröffentlichungsworkflow normalisiert den Tag für den Vergleich mit den internen VST3-Metadaten auf 1.3.0. Bestehende Release-Tags und Assets werden weder umbenannt noch überschrieben.

## Layout

Classic behält Logo, Farben, Browser, Drehregler und Look-Auswahl. Titel und orangefarbener Untertitel stehen auf derselben Zeile, die automatisch erzeugte Hersteller-/Versionszeile darunter. Kopfzeile und Look-Auswahl bleiben voneinander getrennt.

Retro ergänzt den Produktuntertitel unterhalb der tatsächlichen Bus-/Sampleratenanzeige und die Hersteller-/Versionszeile im freien Metadatenstreifen. Der in die Originalgrafik eingebrannte funktionale Geräteaufdruck „ACOUSTIC ENGINE / ROOM SIMULATION“ bleibt als Baugruppenbeschriftung erhalten; er ist nicht der Produktuntertitel. Keine Frontplatten-Retusche, keine Logo-Nachzeichnung, keine zusätzlichen Timer und keine Änderungen an interaktiven Bereichen oder Animationen.

## Echte Screenshots und Veröffentlichung

Der bestehende Windows-Prüfhost öffnet den tatsächlich kompilierten Produktionseditor. Vier Standardaufnahmen zeigen Retro Episode I, Retro Episode IV, Classic Dunkel und Classic Hell, nativ 1504 × 846 Pixel. Eine zusätzliche Metadaten-Datei pro Aufnahme nennt die tatsächlich eingebettete interne/öffentliche Version, Caption, Herstellerzeile und Look/Episode. Die CI prüft diese Angaben gegen CMake.

`Tools/package_branding_screenshots.py` kopiert ausschließlich die nativen `Windows-Editor-Screenshot.png` aus dem erfolgreichen CI-Lauf, bytegleich und ohne Bildbearbeitung. Es erzeugt `docs/screenshots.json` mit tatsächlicher Capture-Version, Quelllauf, Bildpfaden und SHA-256. Der Release-Workflow prüft das Manifest und hängt es als Asset an. Historische Bilder bleiben erhalten.

Aktuelle Bilder: `docs/oberflaeche-retro-neu.png`, `docs/oberflaeche-retro.png`, `docs/oberflaeche-dunkel.png`, `docs/oberflaeche-hell.png`. README und aktuelle GUI-Dokumentation sind damit aktualisiert; historische Release Notes bleiben unverändert.

## Homepage

Die SPATIAL-LAB-Karte erhält den verbindlichen Untertitel und eine neue Retro-Aufnahme. Die vorhandene dynamische Release-Abfrage liefert Tag und tatsächlich hochgeladenes Windows-VST3-Asset. Der Upload von `screenshots.json` aktiviert die bereits vorhandene Screenshot-Auswahl aus dem realen Release-Tag. Keine fest eingetragene Ersatz-Versionsanzeige; bei API-Ausfall bleibt der Download-Fallback `/releases/latest`. Andere Plugin-Karten bleiben unverändert.

## Abnahme

[Vollständige Abnahme 44278481794](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/44278481794): **alle drei Jobs erfolgreich**.

| Prüfung | Ergebnis |
|---|---|
| Windows x64 Release-VST3 | erfolgreich gebaut; Version 1.3.0, Stromschlag Audio, alle bisherigen CIDs bestätigt |
| Windows CTest | 8/8 bestanden: Calibration, Boundaries, Environments, DSP, RetroMotion, RoomVoicing, Processor, RetroGUI |
| Linux ASan/UBSan | 6/6 bestanden, keine gemeldeten Sanitizer-Fehler |
| pluginval 1.0.4, Level 8 | SUCCESS; Audio, State/Restore, Automation, Editor, Parameter-Thread-Safety und Bus-Prüfungen |
| Vier aktuelle Windows-Aufnahmen | 1504 × 846, Metadaten v1.3 aus generiertem Header, visuell geprüft |
| Bestehende GUI-/Lichtnachweise | bestanden; 52 Umgebungsaufnahmen beider Retro-Episoden, große Editorgrößen, Animationen bei Stille/Audio |
| Akustik-Audit / Impulsantworten | erfolgreich, bestehende Mess-/Hördateien erneut vom tatsächlichen DSP erzeugt |
| Schutzprüfungen | v1.2-DSP, 26 Presets, Processor/State/Parameter, Grafikdateien unverändert |
| Lokale Tests | 6/6 bestanden, GUI-/Prüfhost-Syntax mit JUCE 8.0.6 geprüft |

[pluginval-Protokoll](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/44278481794/artifacts/14415554073) · [Test-/Build-Provenienz](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/44278481794/artifacts/14417136999) · [26-Umgebungs-Messungen](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/44278481794/artifacts/14416880943).

Getesteter CI-Merge-Commit: `80a981cb3dc06b5a5d2e3a48ded6050cee6ed0f9`. SHA-256 sämtlicher Build-Eingaben: `833986146eb98cd5ee8108a8acaec291031445ed6550b0729589eebc1f58fe36`. Die anschließenden Screenshot-/Dokumentationsnachträge verändern diese Eingaben nicht.

Installations-ZIP vollständig heruntergeladen und geprüft: CRC fehlerfrei, genau ein Wurzelordner `SpatialLab.vst3`, `Contents/Resources/moduleinfo.json` und `Contents/x86_64-win/SpatialLab.vst3` vorhanden; Modul als Windows-x64-PE identifiziert. SHA-256: `e171493f0e37f78d65bff84b57fc199953b58a3d5115e20c2f98fad8e16f9831`.

Die Prüfung ersetzt keinen subjektiven Hörtest oder eine Bedienprüfung in jeder DAW. pluginval wurde nicht mit einem separaten Steinberg-VST3-Validator konfiguriert; dieser Teil wird im Protokoll explizit als übersprungen ausgewiesen. Die Standardansichten sind native Desktop-Aufnahmen; Maximalgrößen auf dem kleineren CI-Desktop bleiben Komponentenrender, wie bisher dokumentiert.

## Geprüfte native Aufnahmen

Alle vier Bilder aus [CI-Lauf 44278481794](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/44278481794), Artefakt [SpatialLab-Classic-Looks](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/44278481794/artifacts/14415883025), sind bytegleich übernommen und visuell geprüft: vollständige Titel-/Untertitelzeile ohne Überschneidung, Herstellerkennung v1.3, unveränderte Classic-Regler; in Retro beide Episoden, Produktkennung im Metadatenbereich, Logo, Browser und Samplerate sichtbar. Vier PNGs jeweils 1504 × 846 Pixel. [Manifest mit SHA-256](../screenshots.json).

Die Homepage-Auflösung wurde in einem echten Chromium mit kontrollierten Release-Antworten geprüft: v1.3 und historisches v1.2.0 setzen Versionsanzeige, passendes Tag-Bild und tatsächliches Windows-VST3-ZIP korrekt. Bei API-Ausfall bleibt die Versionsanzeige leer/ausgeblendet und der Link führt zu /releases/latest. Die abschließende Live-Prüfung ist ebenfalls bestanden, siehe Veröffentlichung unten.

## Tatsächlich veröffentlicht

[SPATIAL LAB v1.3](https://github.com/StromschlagProductions/SpatialLab-vst/releases/tag/v1.3) ist öffentlich veröffentlicht und als neueste stabile Version gekennzeichnet (`draft=false`, `prerelease=false`). [Veröffentlichungsworkflow 44301446471](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/44301446471): erfolgreich. [PR #8](https://github.com/StromschlagProductions/SpatialLab-vst/pull/8) ist nach `main` übernommen; Tag v1.3 verweist auf diesen geprüften Stand einschließlich der aktuellen Screenshots.

[Windows-x64-VST3 direkt herunterladen](https://github.com/StromschlagProductions/SpatialLab-vst/releases/download/v1.3/SpatialLab-v1.3-Windows-x64-VST3.zip). Die öffentliche ZIP-Datei wurde nach Veröffentlichung erneut heruntergeladen: 18.885.330 Byte, CRC fehlerfrei, SHA-256 identisch zum akzeptierten Build (`e171493f0e37f78d65bff84b57fc199953b58a3d5115e20c2f98fad8e16f9831`). SHA256SUMS.txt und screenshots.json sind ebenfalls reale Release-Assets. Historische Releases bleiben unverändert.

[Homepage](https://stromschlagproductions.github.io/): aktualisiert und [GitHub-Pages-Bereitstellung erfolgreich](https://github.com/StromschlagProductions/StromschlagProductions.github.io/actions/runs/44303637567). Ein echter Chromium hat die öffentlich geladene Seite geprüft: Untertitel Audio Drama Spatial Environments, dynamisch aus dem tatsächlichen Release gelesene Version v1.3, geladene native Retro-Aufnahme 1504 × 846 aus dem Tag v1.3 und Download-Button exakt auf dem oben genannten öffentlichen ZIP. Die fünf anderen Plugin-Karten sind bytegleich unverändert.
