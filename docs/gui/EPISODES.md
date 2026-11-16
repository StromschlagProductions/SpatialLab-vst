Aktuelle Identität, Fenstergröße und Beleuchtung: [Phase 2.1](../phase2.1/PHASE2_1.md).

# Episode I / Episode IV

Die freigegebenen Originale `assets/SpatialLab_Episode_I.png` und `assets/SpatialLab_Episode_IV.png` sind beide 3344×1882 Pixel groß. Sie werden unverändert als JUCE-BinaryData in die VST3 eingebettet, vollständig im ursprünglichen Seitenverhältnis dargestellt und zur Laufzeit nicht aus externen Dateien geladen. `Tools/check_episode_artwork.py` prüft Maße und SHA-256 in CI. Kein Retuschieren, kein neues Logo, keine Farbänderung, kein Beschneiden der Dateien. Frühere Logo-Retuschen und Logo-Überlagerungen sind im aktuellen Editor deaktiviert und nicht mehr als Ressourcen eingebunden.

## Bedienung

**Episode IV** ist die verwitterte Standardansicht beim ersten Öffnen. Der horizontale mechanische Kippschalter auf der freien Metallfläche unten rechts wechselt unmittelbar zu **Episode I** (links, fabrikneu) bzw. **Episode IV** (rechts). Es gibt keine Überblendung. Die Beschriftung lautet ausschließlich `EPISODE I` und `EPISODE IV`. Fassung, Hebel, Lichtreflexe und Schatten werden im Editor gezeichnet; keine Originalgrafik wird dafür bearbeitet. Die bisherige untere Preset-Bedienung entfällt vollständig. Raumauswahl, Größe und Klang bleiben an ihren Positionen.

Der Schalter besitzt Maus- und Tastaturbedienung (Leertaste/Enter bei Fokus). Im logischen 1672×941-Koordinatensystem belegt er einschließlich Beschriftungen `(1230,746)` bis `(1503,813)` innerhalb der freien Metallfläche. Nieten und erhabener Rahmen bleiben frei. Beide Bilder verwenden dieselben Koordinaten. Die bestehende Knopf-Darstellung entnimmt die zum jeweiligen Skin passenden Originalpixel mit doppelten Quellkoordinaten; die kompletten Hintergrund-PNGs bleiben unverändert. Seit Phase 2.1 startet das Fenster ohne gespeicherte Größe mit 1504×846 logischen Pixeln; Minimum 1003×565, Maximum 1672×941. Die letzte Größe wird beim Schließen separat gespeichert. Die Frontplatte behält ihr Seitenverhältnis.

CRT, Signalspur, Lampen und Röhrenglimmen bleiben aktiv. Die dynamische Betriebsanzeige zeigt die tatsächliche Host-Samplerate und Kanalbelegung; ihr Untergrund folgt der gewählten Frontplatte.

## Rein kosmetische Speicherung

Die Auswahl ist **kein Audioparameter** und verändert weder Raum/Größe/Klang noch Automation oder Session-Recall. Der Episode-Schalter greift weder in die Audio-Engine noch in Prozessorparameter oder Audio-Serialisierung ein. Die gesonderte Phase-2-Klangkorrektur ist in [AKUSTIK.md](../phase2/AKUSTIK.md) beschrieben. Die Auswahl wird als separate, benutzerbezogene JUCE-PropertiesFile mit der Anwendung `SpatialLab`, Ordner `Stromschlag Audio` (eine alte Episode-Einstellung aus `Stromschlag Productions` wird einmalig übernommen, sofern im neuen Pfad noch kein Wert vorliegt), Suffix `.gui.settings` und Schlüssel `episodeIV` gespeichert. Beim nächsten Editor-Öffnen wird diese GUI-Vorliebe wiederhergestellt. Bestehende geöffnete Instanzen bleiben unabhängig bedienbar. Die Schreib-/Lesezugriffe erfolgen ausschließlich im GUI-Thread.

Die Standardansicht IV gilt bei fehlender Einstellung; eine bewusst gespeicherte I-Auswahl bleibt erhalten. Auf einem anderen Computer ohne Einstellungsdatei startet IV. Die Audio-Session enthält keine Skin-Daten und bleibt bytegleich.

## Tests und echte Bilder

Der GUI-Test prüft den Standard IV, die beiden Umschaltstellungen, bytegleichen Audio-State, bitidentische Audiosamples während der Umschaltung, separate Wiederherstellung der GUI-Auswahl sowie die Grenzen der Bedienfläche bei 1003×565, 1504×846 und 1672×941. Die bisherigen DSP- und Prozessor-Tests laufen ebenfalls.

Windows-CI öffnet den echten Produktionseditor getrennt für beide Frontplatten und erstellt jeweils einen nativen Windows-Bildschirm-Screenshot sowie gesondert benannte JUCE-Komponenten-Render. Der Vorschauhost verwendet dafür isolierte GUI-Einstellungsdateien und setzt keine Benutzervorliebe um. Beispiel:

```powershell
& ".\build\SpatialLabGuiPreview_artefacts\Release\SpatialLab GUI Preview.exe" --output=build/gui-preview/Episode-I --episode=I
& ".\build\SpatialLabGuiPreview_artefacts\Release\SpatialLab GUI Preview.exe" --output=build/gui-preview/Episode-IV --episode=IV
```

Die Screenshots sind Bildschirmaufnahmen des laufenden JUCE-Editors im Prüfhost, keine Konzeptbilder und keine Aufnahmen aus einer DAW. Reale DAW-/DPI-Abnahme bleibt separat erforderlich. Das vorhandene Installations-ZIP `SpatialLab-VST3-Windows.zip` enthält den vollständigen Ordner `SpatialLab.vst3`. Es wird als Actions-Artefakt bereitgestellt, nicht als endgültiges Release veröffentlicht. Kein Merge nach main ohne Zustimmung.

## Ergebnis

Code `df416f805644b7b8ae1dac337f411872104da6aa` ist im [CI-Lauf 43745098224](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43745098224) vollständig grün: unveränderte Originalgrafiken, Windows-VST3, DSP-/Prozessor-/GUI-Tests, beide echten Bildschirmaufnahmen, PNG-Dateiprüfung und Linux-Sanitizer. Die [aktuellen Screenshots und Paketnachweise](screenshots/README.md) sind abgelegt. Beide Schalterstellungen und die vollständigen Beschriftungen wurden visuell kontrolliert; Nieten und Umrandung bleiben sichtbar. Der nachfolgende Nachweiscommit ändert nur Dokumentation/Bilder, keinen Plugin-Code.
