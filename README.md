# SPATIAL LAB – Spatial Environments for Audio Drama

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="https://raw.githubusercontent.com/StromschlagProductions/Locations-vst/main/docs/logo-dunkel.png">
  <img src="https://raw.githubusercontent.com/StromschlagProductions/Locations-vst/main/docs/logo-hell.png" alt="Stromschlag Audio" height="64">
</picture>

## Weniger suchen. Mehr Geschichten erzählen.

SPATIAL LAB versetzt Dialoge und Geräusche in charakteristische akustische Umgebungen. Statt durch große Impulsantwort-Bibliotheken zu suchen, wählst du einen Raum und passt ihn mit **SIZE** und **TONE** an.

**[SPATIAL LAB v1.0 für Windows herunterladen](https://github.com/StromschlagProductions/SpatialLab-vst/releases/tag/v1.0.0)**

[Windows-x64-VST3-ZIP](https://github.com/StromschlagProductions/SpatialLab-vst/releases/download/v1.0.0/SpatialLab-v1.0.0-Windows-x64-VST3.zip) · [Bedienung und technische Details](docs/phase3b/PHASE3B.md)

## 26 Umgebungen, vier Kategorien

| Kategorie | Umgebungen |
|---|---|
| **INNENRÄUME** | Wohnzimmer, Küche, Badezimmer, Schlafzimmer, Flur, Treppenhaus, Keller, Dachboden, Garage/Werkstatt |
| **AUSSENRÄUME** | Offene Landschaft, Wald, Straße/Gasse, Innenhof, Schlucht/Canyon, Tunnel, Unterführung |
| **FAHRZEUGE** | Kleinwagen, großer PKW, Transporter, LKW-Kabine |
| **SPEZIALRÄUME** | Kleine Kapelle, Kathedrale, Höhle, X-Wing Cockpit, Y-Wing Cockpit, Snowspeeder Cockpit |

Die Cockpits sind eigenständige kreative Klanginterpretationen; es wird keine offiziell festgelegte Akustik fiktiver Fahrzeuge behauptet.

## Algorithmische Akustik

Die **Indoor-Engine** kombiniert frühe Reflexionen mit einem Feedback-Delay-Netzwerk für frequenzabhängig gedämpften Nachhall. Die 19 Innenraum-, Fahrzeug- und Spezialprofile besitzen individuelle Laufzeiten, Reflexionsverteilungen, Diffusion und Dämpfung.

Die separate **Outdoor-Engine** erzeugt diskrete Stereo-Reflexionen mit individuellen Verzögerungen, Pegeln, Panorama und Tiefpässen. Wo sinnvoll, ergänzt begrenztes Feedback wiederkehrende Echos. Die offene Landschaft liefert bewusst nur sehr schwache Rückwürfe; der Canyon deutlich getrennte entfernte Echos. Keine Faltung, keine externen Preset-Dateien und kein Oversampling.

SPATIAL LAB liefert **immer 100 % Wet** und ist für FX-/Send-Kanäle ausgelegt. Es gibt keinen Dry/Wet- oder Ausgangspegel-Regler. Den Effektanteil stellst du mit dem Send-Pegel in der DAW ein.

## Zwei Retro-Oberflächen

**Episode I – fabrikneu**

![Episode I: echte Windows-Oberfläche](docs/release-v1/screenshots/Episode-I.png)

**Episode IV – verwittert, Standardansicht**

![Episode IV: echte Windows-Oberfläche](docs/release-v1/screenshots/Episode-IV.png)

Die Bilder zeigen den freigegebenen Phase-3B-Editor als echte Windows-Desktop-Aufnahmen. Der kosmetische Episode-Schalter verändert keine Audioeinstellungen. Beide Originalfrontplatten sind unverändert eingebettet.

## Installation

1. Die Datei **SpatialLab-v1.0.0-Windows-x64-VST3.zip** vom offiziellen Release herunterladen.
2. DAW schließen und das ZIP vollständig entpacken.
3. Den **gesamten Ordner `SpatialLab.vst3`** nach `C:\Program Files\Common Files\VST3\` kopieren. Nicht nur die innere Moduldatei kopieren.
4. DAW starten und bei Bedarf die VST3-Suche erneut ausführen.

Das Release-ZIP enthält direkt den vollständigen VST3-Ordner; ein Installer oder eine weitere ZIP-Ebene ist nicht erforderlich.

## Bedienung

- SPATIAL LAB auf einen FX-/Send-Kanal laden und die Quelle dorthin senden.
- Im großen CRT Kategorien aufklappen, mit Mausrad/Scrollleiste navigieren und ein Preset direkt anklicken. Die Pfeile links wählen den vorigen/nächsten Eintrag.
- **SIZE** verändert die wahrgenommene Größe; **TONE** den spektralen Charakter. **50 % / 50 %** ist die neutrale Ausgangsstellung. Ein Raumwechsel setzt beide Regler dorthin zurück.
- Beide Regler lassen sich ziehen, mit dem Mausrad bedienen und per Doppelklick neutral stellen.
- Send-Pegel in der DAW einstellen. Fertig.

Host-Auswahl und Session-Recall machen den aktiven Eintrag im Browser sichtbar. Klappzustände und Scrollposition werden nicht über das Schließen des Editors hinweg gespeichert. Es gibt kein Suchfeld und keine zusätzliche Tastatur-Navigation.

## System und Formate

- Windows x64, Windows 10/11 und ein kompatibler **64-Bit-VST3-Host**.
- Mono→Mono, Mono→Stereo und Stereo→Stereo.
- Primäre Qualitäts- und Test-Sampleraten: **44,1 und 48 kHz**. Höhere Raten sind nicht gleichwertig umfassend abgenommen.
- Fenster: mindestens **1003×565**, Standard **1504×846**, maximal **2174×1223**, proportional skalierbar. Für die vollständige Maximalansicht ist ein entsprechend großer Desktop erforderlich.
- Windows-x64-VST3 ist das bereitgestellte Format. Keine AU-, AAX-, VST2- oder Standalone-Ausgabe.

## Kompatibilität und bekannte Grenzen

Version 1.0 verwendet 26 linear kodierte Raumwerte und Session-Schema 5. Ältere Test-Sessions und deren Raumautomation sind ausdrücklich nicht kompatibel; neue Sessions anlegen oder Einstellungen neu setzen. Hersteller-/Plugin-Kennungen und SIZE/TONE-IDs bleiben erhalten.

Raumwechsel blenden das Wet-Signal kurz aus und wieder ein, um harte Abrisse zu vermeiden. Raumpegel werden nicht künstlich angeglichen. Durchschnitts-CPU-Messungen sind keine Garantie für jede Hardware oder extrem kleine Audioblöcke.

Die CI prüft Layout und Eingaben bei 2174×1223. Ihr Desktop ist jedoch auf 1920×1080 begrenzt: Maximalgrößen-Bildnachweise sind dort native JUCE-Komponentenrender, keine Desktop-Screenshots. Die obigen Standardansichten sind echte Desktop-Aufnahmen.

## Entwicklung und Qualitätssicherung

C++17, JUCE **8.0.6** an festem Commit, CMake **3.31.6**, Visual Studio 2022 x64. Der Build lädt JUCE einschließlich VST3-SDK selbst; kein gesonderter SDK-Checkout nötig.

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DBUILD_TESTING=ON
cmake --build build --config Release --parallel 2
ctest --test-dir build -C Release --output-on-failure
```

Vollständige Windows-/GUI- und Linux-Sanitizer-Prüfungen laufen bei relevanten Pull-Request-Änderungen und gezielter manueller Abnahme. Kein doppelter Push-Trigger und keine automatische Veröffentlichung. Die bisherigen 19 Raumantworten werden gegen den eingefrorenen Vorgänger geprüft. Weitere Architektur- und Messdetails stehen in [der technischen Dokumentation](docs/phase3b/PHASE3B.md); ältere Phasenberichte sind historische Entwicklungsnachweise.
