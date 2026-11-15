# SPATIAL LAB – Raumhall für Hörspiele

**Stromschlag Audio · Windows · VST3 · Stereo**

SPATIAL LAB ist ein bewusst einfach gehaltener Raumhall für Hörspielproduktionen. Statt dutzender technischer Regler wählst du einen Raum und passt bei Bedarf nur **Größe** und **Klang** an. Das Plugin ist für **Send-/FX-Kanäle** gedacht und liefert immer **100 % Hall**, ohne Direktsignal.

> **Entwicklungsstand:** Noch keine veröffentlichte, hörgetestete Version. Die derzeitige Fassung ist ein Prototyp mit drei Räumen. Bitte nicht als fertige Produktionsfreigabe verstehen.

## ⬇ SPATIAL LAB herunterladen

**[WINDOWS-VST3 ALS ZIP HERUNTERLADEN – HIER KLICKEN]**(https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43733243192/artifacts/14169382572)**

**Das ZIP entpacken und den kompletten Ordner `SpatialLab.vst3` nach `C:\\Program Files\\Common Files\\VST3\\` kopieren.** Der Link führt zum ZIP-Artefakt des erfolgreichen Windows-Builds vom 15. November 2026. Gegebenenfalls ist eine Anmeldung bei GitHub nötig. **Der Testdownload ist zeitlich befristet** und enthält neben `SpatialLab.vst3` auch Prüfdateien und GUI-Vorschaubilder.

**⬇ [Spätere freigegebene Versionen (GitHub Releases)](https://github.com/StromschlagProductions/SpatialLab-vst/releases/latest)** – hier gibt es **noch keine** freigegebene Version. Nach Einführung eines Release-Verfahrens soll dies der dauerhafte Download-Einstieg sein, wie bei RADIOPHONE und LOCATIONS.

## Warum es dieses Plugin gibt

Für ein Hörspiel braucht eine Szene einen glaubwürdigen akustischen Ort: das gedämpfte Wohnzimmer, die hellere Küche oder das gekachelte Badezimmer. SPATIAL LAB soll diesen Raumklang schnell bereitstellen, ohne dass du für jede Szene eine komplizierte Hallkette aufbauen musst.

SPATIAL LAB gestaltet den **festen Hall des Szenenraums**. Für bewegte Figuren, Stimmen hinter Türen und wechselnde Entfernungen ist das separate Plugin [LOCATIONS](https://github.com/StromschlagProductions/Locations-vst) vorgesehen.

## Oberfläche und Vorschau

Die Retro-Oberfläche orientiert sich an einer verwitterten Science-Fiction-Gerätefront mit Röhren, Kontrollleuchten und bernsteinfarbenem Bildschirm. Nur Raumauswahl, Größe und Klang steuern den Hall. Die übrigen Geräte-Elemente sind Dekoration oder Statusanzeigen.

**[Echte Windows-GUI-Vorschau und weitere Renderings ansehen](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43733243192)** – im Build-Artefakt liegen `Windows-Editor-Screenshot.png` sowie Renderings bei 100, 125 und 150 %. Ein dauerhaft eingebettetes Vorschaubild ergänzen wir, sobald die Logo-Korrektur freigegeben und ein Screenshot ins Repository übernommen ist.

## Räume und Regler

| Bedienelement | Funktion |
|---|---|
| **Raum** | Wohnzimmer, Küche oder Badezimmer auswählen |
| **Größe** | Raumwirkung kleiner oder größer gestalten; Mitte = neutral |
| **Klang** | Hall dunkler oder heller gestalten; Mitte = neutral |

Bei der Auswahl eines anderen Raums springen Größe und Klang auf die neutrale Mittelstellung zurück. Ein Doppelklick setzt jeden der beiden Drehregler ebenfalls zurück. Die DAW kann alle drei Parameter automatisieren und speichert die Einstellungen mit dem Projekt.

| Raum | Klangidee |
|---|---|
| **Wohnzimmer** | Eher kurzer, dunkler Nachhall |
| **Küche** | Hellerer Raum mit deutlicheren Reflexionen |
| **Badezimmer** | Längerer, reflektierender Hall |

Die Räume sind klanglich gestaltete Eindrücke, keine vermessenen Impulsantworten. Weitere Innen- und Außenräume sind geplant, aber noch nicht implementiert.

## Anwendung in der DAW

1. Einen **FX-/Send-Kanal** anlegen und SPATIAL LAB dort einsetzen.
2. Den passenden Raum auswählen.
3. Über den **Send-Pegel der DAW** bestimmen, wie viel Hall die Stimme oder das Geräusch erhält.
4. Nur wenn nötig **Größe** oder **Klang** verändern.

Das Plugin hat absichtlich keinen Dry/Wet-Regler, keinen Ausgangspegel und keine separaten Expertenparameter für Early/Late, Diffusion oder Dämpfung. Der direkte Ton bleibt auf der Quellspur. Ein Raumwechsel erfolgt ohne Überblendung und kann eine laufende Hallfahne abschneiden; er ist nicht als Bewegungsautomation gedacht.

## Installation (Windows, Testversion)

1. Das ZIP über den Testdownload oben laden und entpacken.
2. Im Archiv den **gesamten Ordner** `SpatialLab.vst3` suchen und nach `C:\Program Files\Common Files\VST3\` kopieren.
3. DAW neu starten beziehungsweise VST3-Plugins erneut scannen.

Das Testarchiv kann zusätzliche Dateien wie Benchmarks, Werkzeugdaten und Screenshots enthalten. Diese werden **nicht** im VST3-Ordner installiert. Der Build installiert nichts automatisch.

## Technische Angaben

- **Format:** Windows x64, VST3, JUCE 8 und C++17.
- **Kanäle:** Mono → Mono, Mono → Stereo, Stereo → Stereo.
- **Abtastraten:** Schwerpunkt 48 kHz für Hörspiele und 44,1 kHz für Musik. 96/192 kHz sind bisher nur grundlegend geprüft.
- **Signalweg:** Immer 100 % Wet; gemeldete Plugin-Latenz 0; konservativ gemeldete Hallfahne 6 Sekunden.
- **Hallverfahren:** Raumspezifische frühe Reflexionen und ein gedämpftes Feedback-Delay-Netzwerk.
- **Bedienung:** Drei Parameter; keine zusätzlichen versteckten Klangregler.

## Entwicklung, Tests und Freigabe

Die Audio-Grundlage und die Retro-GUI werden in getrennten Entwicklungszweigen geprüft. Automatische DSP-, Prozessor- und GUI-Tests sowie Windows-Builds existieren bereits. **Hörtests in echten DAWs, unabhängige VST3-Validierung und die Klärung der Lizenzierung stehen vor einer Veröffentlichung noch aus.** Ein erfolgreicher CI-Build ersetzt diese Prüfungen nicht.

- [Technisches Konzept und Parameter](docs/DESIGN.md)
- [Bisherige Tests und offene Prüfungen](docs/VALIDATION.md)
- [Retro-Oberfläche und Screenshot-Verfahren](docs/gui/RETRO_GUI.md)
- [Windows-Builds und Artefakte](https://github.com/StromschlagProductions/SpatialLab-vst/actions)

**Wichtig zur Lizenz:** Die Veröffentlichungs-Lizenz ist noch nicht festgelegt. Vor der Weitergabe fertiger Binärdateien müssen insbesondere die JUCE- und VST3-SDK-Bedingungen geprüft werden.

## Geplant

Nach den ersten drei Räumen sollen weitere kuratierte Innenräume und Außenumgebungen folgen. Light-/Dark-Ansichten sind mögliche spätere Oberflächenvarianten. Das einfache Drei-Regler-Konzept und die klare Trennung von SPATIAL LAB (statischer Szenenhall) und LOCATIONS (Bewegung/Entfernung) bleiben erhalten.
