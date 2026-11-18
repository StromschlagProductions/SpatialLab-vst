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

Aktuelle Bilder: `docs/oberflaeche-retro-neu.png`, `docs/oberflaeche-retro.png`, `docs/oberflaeche-dunkel.png`, `docs/oberflaeche-hell.png`. README und aktuelle GUI-Dokumentation werden damit aktualisiert; historische Release Notes bleiben unverändert.

## Homepage

Die SPATIAL-LAB-Karte erhält den verbindlichen Untertitel und eine neue Retro-Aufnahme. Die vorhandene dynamische Release-Abfrage liefert Tag und tatsächlich hochgeladenes Windows-VST3-Asset. Der Upload von `screenshots.json` aktiviert die bereits vorhandene Screenshot-Auswahl aus dem realen Release-Tag. Keine fest eingetragene Ersatz-Versionsanzeige; bei API-Ausfall bleibt der Download-Fallback `/releases/latest`. Andere Plugin-Karten bleiben unverändert.

## Abnahme

Lokale reine DSP-Tests: 6/6 bestanden; JUCE-GUI-/Prüfhost-Quellen syntaktisch geprüft. Native Windows-Aufnahmen, vollständige Windows-/Linux-Abnahme, pluginval Level 8 und die öffentliche Homepage werden vor Abschluss überprüft. Die endgültigen CI-/Release-/Bildnachweise werden nach erfolgreicher Prüfung hier ergänzt.
