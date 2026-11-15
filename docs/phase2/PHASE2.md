> Historischer Phase-2-Nachweis. Aktuelle Herstelleridentität und GUI: [Phase 2.1](../phase2.1/PHASE2_1.md). Die Raumakustik bleibt seit Phase 2 unverändert.

# Entwicklungsphase 2 — Stromschlag Audio

## Hersteller und Kompatibilität

Der Hersteller heißt ausschließlich **Stromschlag Audio**. CMake-Firmenname, aktuelle GUI-Einstellungsablage und Dokumentation sind korrigiert. GitHub-Account/Repository bleiben `StromschlagProductions/SpatialLab-vst`. Die historische Bundle-ID `com.stromschlagproductions.spatiallab`, Hersteller-Code `StPr`, Plugin-Code `SpLb` und alle drei vorhandenen Parameter-IDs bleiben unverändert. Diese Bezeichner sind Kompatibilitätskennungen, keine aktuelle Herstellerwerbung.

Eine vorhandene Episode-Wahl im früheren Ordner `Stromschlag Productions` wird beim Editor-Öffnen übernommen, sofern unter `Stromschlag Audio` noch kein `episodeIV`-Wert existiert. Die alte Datei bleibt unverändert; eine neue bewusste Auswahl hat Vorrang. Die Migration ist getrennt vom Audio-Session-State und wird mit isolierten Dateien getestet. Alte Projekte behalten dieselben VST3-Klassen-IDs. Ein tatsächlicher Projekt-Recall in Fender Studio Pro bleibt praktisch nachzutesten; manche Hosts benötigen einen erneuten Plugin-Scan für die aktualisierte Herstellersortierung.

`Tools/check_vst3_manufacturer.py` prüft die vom gebauten DLL-Modul erzeugte `moduleinfo.json`: Factory und Klassen müssen Stromschlag Audio nennen; Audio-, Controller- und Kompatibilitäts-CIDs müssen identisch zum bisherigen Modul sein. Es wird nicht nur die CMake-Quelldatei kontrolliert.

## Akustik

Siehe [AKUSTIK.md](AKUSTIK.md) mit alten/neuen Presetdaten, Begründung, Messwerten und konserviertem Vergleichs-DSP. Der neue Klang ist ausdrücklich ein technischer Hörtestvorschlag. Kein neuer Raum, kein zusätzlicher Audioregler, weiterhin 100 % Wet. Größe, Klang, Automation, Schema-3-Session-State und Raumwahl-Reset bleiben erhalten. Die gespeicherten Werte alter Projekte bleiben gültig; ihre Raumantwort ändert sich bewusst durch diese neue Preset-Abstimmung.

## Lebendige Retro-Oberfläche

Die freigegebenen 3344×1882-PNGs samt Logos werden unverändert eingebettet und weiterhin per SHA-256 kontrolliert. Episode I/IV, Standard IV, horizontaler Kippschalter und separate Skin-Speicherung bleiben bestehen.

Lichtbewegung findet ausschließlich im GUI-Thread bei 30 Hz statt. Vier feste Glasbereiche enthalten schmale warme Filament-/Lichtüberlagerungen; das im Bild dunkle Ersatzrohr bleibt besonders zurückhaltend. Mehrere langsame Sinusrhythmen und glatt interpolierte deterministische Schwankungen erzeugen voneinander unabhängiges Glimmen; seltene kurze Helligkeitsdips erlauben gelegentliches Flackern. Die jeweiligen Linsen-/Glasformen begrenzen die Animationen, keine freistehenden generischen Glow-Kreise. Nur einige Werte reagieren leicht auf den bereits lockfrei veröffentlichten tatsächlichen Hallpegel. Kein zusätzlicher DSP-/Audiothread-Aufwand und keine neuen Parameter. Der Editor-Timer endet beim Schließen; Repaints bleiben auf die aktiven Regionen beschränkt.

Der Zielcomputer wird vollständig innerhalb der vorhandenen CRT-Glasmaske gezeichnet. Eigenständige feste Perspektive, sparsame Raster und bernsteinfarbene Linien auf dunklem Grund: Wohnzimmer mit Sofa/Lampe, kompakte Küche mit Ober-/Unterschränken, Arbeitsplatte/Spüle und Kühlschrank, kleines gekacheltes Bad mit Wanne, Spiegel und Waschbecken. Die Figuren sind eigene einfache Vektorgeometrien, keine kopierten Filmgrafiken. Die Raumansicht folgt direkt dem vorhandenen Raumparameter; keine Kamera-/Raumbewegung oder mehrfarbige moderne Anzeige. Schalter, Bezel und übrige Gerätegrafik bleiben unangetastet.

## Prüfung und Lieferung

Lokale Release- und ASan/UBSan-DSP-/Abklingtests konzentrieren sich auf 44,1/48 kHz; im Cloud-Sandboxlauf ist nur LeakSanitizer wegen ptrace eingeschränkt. Die Linux-CI behält die vollständige Konfiguration einschließlich Leak-Prüfung. Neue reine GUI-Rhythmustests kontrollieren Grenzen, unabhängige Pulsrhythmen und Pegelreaktion. JUCE-GUI-Tests prüfen zusätzlich die Migration und drei unterschiedliche Möbel-Geometrien in beiden Skins. Alle bisherigen Prozessor-, Audio- und Skin-Tests bleiben aktiv.

Windows-CI erstellt sechs echte native Bildschirmaufnahmen (beide Episoden × alle drei Räume) sowie gesondert benannte JUCE-Komponenten-Render bei 100/125/150 %. Nur der Vorschauhost erzeugt synthetische Anregung. Die zusätzlichen Render sind keine DAW-Bildschirmfotos verschiedener Windows-DPI-Einstellungen. `--room=0/1/2` steuert ausschließlich den Prüfhost.

Das Installations-ZIP enthält weiterhin nur den vollständigen Wurzelordner `SpatialLab.vst3`. Der Build-Nachweis, die Bilder, gemessene Hersteller-/CID-Metadaten und offline erzeugte A/B-Impulsantworten werden im Actions-Artefakt bereitgestellt. Kein Merge nach main und kein endgültiges Release. Endgültige klangliche Freigabe erfolgt nach dem erneuten Sprachhörtest des Auftraggebers, nicht anhand der numerischen Tests allein.

## Abgeschlossener Build-Nachweis

Code `0482b7e9275c128dc64935ae3196ad9a2bc89f40`: [Windows-VST3 und Linux-Sanitizer erfolgreich](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43749906538). Windows: DSP, RetroMotion, RoomVoicing, Processor und RetroGUI bestanden. Tatsächliche Hersteller-Metadaten und unveränderte CIDs wurden aus dem erzeugten Modul geprüft. [Sechs echte Screenshots, Installationspaket und A/B-Daten](screenshots/README.md) sind dokumentiert; das heruntergeladene vollständige VST3-ZIP besteht die CRC-/Strukturprüfung. Der spätere Dokumentationscommit verändert den geprüften Code nicht.
