# SPATIAL LAB – Audio Drama Spatial Environments

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="docs/logo-dunkel.png">
  <img src="docs/logo-hell.png" alt="Stromschlag Audio" height="64">
</picture>

VST3-Send-Effekt für Hörspiel-Produktion. SPATIAL LAB bringt 26 typische Räume und Umgebungen mit, vom Wohnzimmer
über Keller, Wald und Schlucht bis zum Autoinneren, als verlässliche Ausgangspunkte. Zwei Regler, **SIZE** und
**TONE**, passen den Raum an die Szene an; den Hallanteil stellst du wie gewohnt mit dem Send in der DAW ein.

**⬇ Download (Windows · VST3 · Mono/Stereo):** https://github.com/StromschlagProductions/SpatialLab-vst/releases/latest

## Warum es dieses Plugin gibt

**Weniger suchen. Mehr Geschichten erzählen.**

Bei der Arbeit an Hörspielen habe ich immer wieder festgestellt, wie viel Zeit allein die Suche nach dem passenden Raumklang verschlingen kann. Man klickt sich durch Hunderte oder Tausende Impulsantworten, findet endlich ein vermeintlich passendes Wohnzimmer – und merkt beim Einsetzen in die Szene, dass es überhaupt nicht nach dem Wohnzimmer klingt, das man sich vorgestellt hat. Also beginnt die Suche von vorn.

Aus diesem Frust entstand die Idee zu **SPATIAL LAB**: Statt immer neue Hallbibliotheken zu durchsuchen, wollte ich eine überschaubare Auswahl typischer Räume und akustischer Umgebungen für Hörspiele direkt griffbereit haben – vom Wohnzimmer über den Keller bis zum Wald oder zur Schlucht. Nicht als Anspruch, jeden realen Ort perfekt nachzubilden, sondern als verlässliche Ausgangspunkte, die sich mit **SIZE** und **TONE** schnell an die jeweilige Szene anpassen lassen.

Denn am Ende möchte ich meine Zeit nicht damit verbringen, den vermeintlich perfekten Hall zu suchen. Ich möchte Geschichten erzählen – und mich auf die Stimmen, Geräusche und die Atmosphäre konzentrieren, die eine Szene lebendig machen.

*SPATIAL LAB soll niemandem die Erfahrung abnehmen, Räume selbst zu gestalten. Gerade am Anfang kann es viel Spaß machen und lehrreich sein, einen Raum selbst aus Impulsantwort, Vorverzögerung, Dämpfung und Hallanteil zusammenzusetzen und zu hören, wie diese Bausteine ineinandergreifen. Ich bin dankbar, dass ich diesen Weg selbst gegangen bin, auch wenn er manchmal umständlich und mühselig war. Gerade dadurch weiß ich, was ich mir von einem Werkzeug wünsche, das all das in einem vereint. SPATIAL LAB soll das Gestalten nicht ersetzen, sondern die technische Umsetzung im Alltag einfacher machen.*

– Lars Eickstädt, Stromschlag Audio

## Oberfläche

**Retro ist das ursprüngliche, charakteristische Design von SPATIAL LAB:** Episode I zeigt das fabrikneue Gerät, Episode IV die verwitterte Standardansicht. Classic Dunkel und Classic Hell sind schlichte Alternativen.

### Retro Episode I – fabrikneu

![SPATIAL LAB Retro Episode I – fabrikneu](docs/oberflaeche-retro-neu.png)

### Retro Episode IV – verwittert

![SPATIAL LAB Retro Episode IV – verwittert](docs/oberflaeche-retro.png)

### Classic Dunkel – schlichte Alternative

![SPATIAL LAB Classic Dunkel – schlichte Alternative](docs/oberflaeche-dunkel.png)

### Classic Hell – schlichte Alternative

![SPATIAL LAB Classic Hell – schlichte Alternative](docs/oberflaeche-hell.png)

Alle vier Bilder sind unveränderte echte Windows-Aufnahmen des geprüften v1.4-Builds. Die Classic-Ansichten zeigen SIZE/TONE im RADIOPHONE-Stil mit orangefarbenem Wertebogen und die überarbeitete Kopfzeile.

Über das Zahnrad oben rechts in Retro oder die Look-Auswahl in Classic wechselst du zwischen **Retro / Classic Dunkel / Classic Hell**. Episode I/IV wird weiterhin im Retro-Modus gewählt. Die Auswahl ist rein kosmetisch; alle Ansichten teilen dieselben Presets und Parameter. [GUI-Dokumentation und Bildherkunft](docs/classic/CLASSIC_GUI.md). Die historischen Screenshots bleiben im Repository erhalten.

## Umgebungen

| Kategorie | Umgebungen |
|---|---|
| **INNENRÄUME** | Wohnzimmer, Küche, Badezimmer, Schlafzimmer, Flur, Treppenhaus, Keller, Dachboden, Garage/Werkstatt |
| **AUSSENRÄUME** | Offene Landschaft, Wald, Straße/Gasse, Innenhof, Schlucht/Canyon, Tunnel, Unterführung |
| **FAHRZEUGE** | Kleinwagen, großer PKW, Transporter, LKW-Kabine |
| **SPEZIALRÄUME** | Kleine Kapelle, Kathedrale, Höhle, X-Wing Cockpit, Y-Wing Cockpit, Snowspeeder Cockpit |

*Die Cockpits sind natürlich kreative Klanginterpretationen. Ich habe schließlich nicht persönlich neben Luke Skywalker im X-Wing gesessen und ihm ein Mikrofon über die Schulter gehalten. Aber ein bisschen so, wie man sich das Innere dieser Maschinen vorstellt, dürfen sie schon klingen.*

*Coming next: Darth Vader & Boba Fett Helmet.*

## So klingt es: algorithmische Akustik

Die **Indoor-Engine** leitet frühe Reflexionen aus dokumentierten neutralen Quell-/Empfänger-Geometrien ab und ergänzt ein Feedback-Delay-Netzwerk für frequenzabhängig gedämpften Nachhall. Die 19 Innenraum-, Fahrzeug- und Spezialprofile besitzen individuelle Begrenzungsflächen, Absorptions-/Streuannahmen, Laufzeiten, Diffusion und Dämpfung. Es sind eigenständige vereinfachte Modelle, keine Rekonstruktionen exakt vermessener Räume.

Die separate **Outdoor-Engine** erzeugt diskrete Stereo-Reflexionen aus Schallwegdifferenzen, Wegdämpfung, Absorption, Streuung und Panorama. Für Wald, Straße, Innenhof, Canyon, Tunnel und Unterführung ergänzt ein kontrollierter dichter Anteil die Rückwürfe. Die offene Landschaft besitzt keinen diffusen Nachhall. Der Canyon kombiniert nahe unregelmäßige Felsreflexionen mit schwächeren entfernten Konturen. Keine Faltung, keine externen Preset-Dateien und kein Oversampling.

SPATIAL LAB liefert **immer 100 % Wet** und ist für FX-/Send-Kanäle ausgelegt. Es gibt keinen Dry/Wet- oder Ausgangspegel-Regler. Den Effektanteil stellst du mit dem Send-Pegel in der DAW ein.

## Anwendung

- SPATIAL LAB auf einen FX-/Send-Kanal laden und die Quelle dorthin senden.
- Im großen CRT Kategorien aufklappen, mit Mausrad/Scrollleiste navigieren und ein Preset direkt anklicken. Die Pfeile links wählen den vorigen/nächsten Eintrag.
- **SIZE** verändert die wahrgenommene Größe; **TONE** den spektralen Charakter. **50 % / 50 %** ist die neutrale Ausgangsstellung. Ein Raumwechsel setzt beide Regler dorthin zurück.
- Beide Regler lassen sich ziehen, mit dem Mausrad bedienen und per Doppelklick neutral stellen.
- Send-Pegel in der DAW einstellen. Fertig.

Host-Auswahl und Session-Recall machen den aktiven Eintrag im Browser sichtbar. Klappzustände und Scrollposition werden nicht über das Schließen des Editors hinweg gespeichert. Es gibt kein Suchfeld und keine zusätzliche Tastatur-Navigation.

## Hinweise

### System und Formate

- Windows x64, Windows 10/11 und ein kompatibler **64-Bit-VST3-Host**.
- **Microsoft Visual C++ Redistributable v14 für x64 (2015–2022)**; falls noch nicht vorhanden: [offizieller Microsoft-Installer](https://aka.ms/vs/17/release/vc_redist.x64.exe).
- Mono→Mono, Mono→Stereo und Stereo→Stereo.
- Primäre Qualitäts- und Test-Sampleraten: **44,1 und 48 kHz**. Höhere Raten sind nicht gleichwertig umfassend abgenommen.
- Fenster: mindestens **1003×565**, Standard **1504×846**, maximal **2174×1223**, proportional skalierbar. Für die vollständige Maximalansicht ist ein entsprechend großer Desktop erforderlich.
- Windows-x64-VST3 ist das bereitgestellte Format. Keine AU-, AAX-, VST2- oder Standalone-Ausgabe.

### Kompatibilität und bekannte Grenzen

Version 1.4 behält 26 linear kodierte Raumwerte, Preset-Keys, drei Parameter und Session-Schema 5 bei. Hersteller-/Plugin-Kennungen und Parameter-IDs bleiben unverändert. Die Raumabstimmungen wurden gezielt überarbeitet; ein Legacy-Klangmodus oder zusätzliche Session-Migrationen werden nicht eingeführt.

Raumwechsel blenden das Wet-Signal kurz aus und wieder ein, um harte Abrisse zu vermeiden. Raumpegel werden nicht künstlich angeglichen. Durchschnitts-CPU-Messungen sind keine Garantie für jede Hardware oder extrem kleine Audioblöcke.

Die CI prüft Layout und Eingaben bei 2174×1223. Ihr Desktop ist jedoch auf 1920×1080 begrenzt: Maximalgrößen-Bildnachweise sind dort native JUCE-Komponentenrender, keine Desktop-Screenshots. Die obigen Standardansichten sind echte Desktop-Aufnahmen.

## Installation (Windows)

1. Die Datei **SpatialLab-v1.4-Windows-x64-VST3.zip** vom [neuesten Release](https://github.com/StromschlagProductions/SpatialLab-vst/releases/latest) herunterladen.
2. DAW schließen und das ZIP vollständig entpacken.
3. Den **gesamten Ordner `SpatialLab.vst3`** nach `C:\Program Files\Common Files\VST3\` kopieren. Nicht nur die innere Moduldatei kopieren.
4. DAW starten und bei Bedarf die VST3-Suche erneut ausführen.

Das Release-ZIP enthält direkt den vollständigen VST3-Ordner; ein Installer oder eine weitere ZIP-Ebene ist nicht erforderlich.

## Versionen

Die Versionsnummer steht im Plugin-Kopf und im Namen der Release. Alle Einzelheiten stehen im
[Changelog](docs/CHANGELOG.md).

- **1.4** – Nutzbare Wet-Pegel für Hörspiel-Sends: neun normale Innenräume und sechs Außenräume **+6 dB Wet** gegenüber 1.3, offene Landschaft **+18 dB**, weiterhin trocken und ohne neue Echos. Wald behält seinen kurzen diffusen Anteil; Fahrzeuge, Kapelle/Kathedrale/Höhle und Cockpits bleiben unverändert. Keine zusätzlichen Regler. [Messungen aller 26 Presets, Hördateien und Headroom](docs/v1.4/WET_LEVELS.md) · [Release Notes v1.4](docs/RELEASE_v1.4.md). Referenz: Send −6 dB, FX 0 dB, SIZE/TONE 50 %. Sehr laute Dry+Wet-Summen brauchen Headroom in der DAW, es gibt keinen versteckten Limiter.
- **1.3** – Gemeinsamer Untertitel **Audio Drama Spatial Environments** für Plugin, Dokumentation und Homepage; Hersteller- und Versionszeile in allen Looks, Version automatisch aus dem Projekt. Akustik unverändert. [Release-Hinweise](docs/RELEASE_v1.3.md) · [Prüfbericht und Screenshot-Herkunft](docs/v1.3/UPDATE.md).
- **1.2.0** – Alle 26 Umgebungen für Sprachräume neu abgestimmt: geometrisch begründete frühe Reflexionen, deutlichere Kabinenrückwürfe, dichterer Tunnel-/Unterführungscharakter, unregelmäßige Canyon-Reflexionen statt Pingpong. Die offene Landschaft bleibt bewusst nahezu trocken. [Release-Hinweise](docs/RELEASE_v1.2.0.md) · [Vorher-/Nachher-Messungen](docs/issue6/COMPARISON.md) · [Windows-/Linux-Abnahme und Hördateien](docs/issue6/ACCEPTANCE.md) · [Analyse, Modellbeschreibung und Hörtestanleitung](docs/issue6/IMPLEMENTATION.md).
- **1.1.2** – Classic Dunkel/Hell: SIZE und TONE in denselben Proportionen wie in den anderen Stromschlag-Plugins. Klang unverändert. [Release-Hinweise](docs/RELEASE_v1.1.2.md).
- **1.1.1** – Weniger redundante Outdoor-Berechnungen, Schutz vor ungültigen Eingangs- und Steuerwerten, zusätzliche Grenzfalltests. [Release-Hinweise](docs/RELEASE_v1.1.1.md).
- **1.1.0** – Drei Looks im selben Plugin: Retro, Classic Dunkel und Classic Hell. Klang unverändert. [Release-Hinweise](docs/RELEASE_v1.1.0.md).
- **1.0.0** – Erste stabile Veröffentlichung: 26 Umgebungen in vier Kategorien, Indoor- und Outdoor-Engine, Retro Episode I und IV.

## Aufbau

C++17, JUCE **8.0.6** an festem Commit, CMake **3.31.6**, Visual Studio 2022 x64. Der Build lädt JUCE einschließlich VST3-SDK selbst; kein gesonderter SDK-Checkout nötig.

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DBUILD_TESTING=ON
cmake --build build --config Release --parallel 2
ctest --test-dir build -C Release --output-on-failure
```

Vollständige Windows-/GUI- und Linux-Sanitizer-Prüfungen laufen bei relevanten Pull-Request-Änderungen und gezielter manueller Abnahme. Kein doppelter Push-Trigger und keine automatische Veröffentlichung. Die bisherigen 19 Raumantworten werden gegen den eingefrorenen Vorgänger geprüft. Weitere Architektur- und Messdetails stehen in [der technischen Dokumentation](docs/phase3b/PHASE3B.md); ältere Phasenberichte sind historische Entwicklungsnachweise.

Weitere Unterlagen: [Offizielles Windows-x64-VST3-ZIP](https://github.com/StromschlagProductions/SpatialLab-vst/releases/download/v1.4/SpatialLab-v1.4-Windows-x64-VST3.zip) · [Changelog](docs/CHANGELOG.md) · [Bedienung, Oberflächen und Version](docs/classic/CLASSIC_GUI.md)

## Qualität und Prüfung

Wenige Regler heißen nicht, dass hier etwas schnell zusammengeschustert wurde. Die Einfachheit liegt in der Bedienung, nicht in der Technik dahinter. Jede Version durchläuft Prüfungen, wie sie auch bei kommerziellen Plugins üblich sind, und alles ist im Quellcode nachvollziehbar:

- **pluginval, Stufe 8:** Vor jeder Veröffentlichung läuft das Prüfprogramm pluginval von Tracktion auf strenger Stufe. Es lädt das Plugin wieder und wieder, wechselt Abtastraten und Puffergrößen, speichert und lädt Einstellungen und fährt alle Regler mit Zufallswerten durch.
- **Automatische Tests der Klangberechnung**, zusätzlich mit Speicher- und Fehlerprüfern (AddressSanitizer, UndefinedBehaviorSanitizer), die auch seltene Programmierfehler aufspüren.
- **Echtzeitsicher:** Während der Wiedergabe wird kein Speicher angefordert und nirgends gewartet. Das verhindert Aussetzer und Knackser, auch bei kleinen Puffern und vielen Instanzen.
- **Geprüfte Räume:** Alle 26 Umgebungen werden mit Referenz-Impulsantworten verglichen, die Oberfläche hat eigene Tests.
- **Gemessene Rechenlast:** Benchmarks über alle Räume, beide Abtastraten, Mono und Stereo sowie Presetwechsel.

Ob es gut klingt, entscheidet am Ende das Ohr. Deshalb geht jede Version auch durch den Studioalltag von Markus Hildebrandt (siehe unten).

## Beta-Test und Danke

**SOUNDJOB / Markus Hildebrandt** – Beta-Testing

Ein großes Dankeschön an **Markus Hildebrandt** von SOUNDJOB. Markus betreibt ein eigenes Tonstudio und hat weit mehr Hörspiele produziert als ich. Er hat SPATIAL LAB im Studioalltag getestet und mir ehrliches, fachkundiges Feedback gegeben. Seine Hinweise zu Außenräumen, also wenig Hall und dafür deutliche, einzelne Echos, stecken in der eigenen Outdoor-Engine. Dafür danke ich ihm sehr: Seine Erfahrung hat das Plugin spürbar besser gemacht.

---

## Hinweis / Disclaimer

**Open Source · Work in Progress**

Dieses Plugin wurde in erster Linie für meinen persönlichen Workflow bei der Hörspielproduktion entwickelt. Es ist auf meine eigenen Anforderungen zugeschnitten und muss daher nicht für jeden Anwendungsfall geeignet sein.

Das Projekt wird laufend weiterentwickelt. Trotz sorgfältiger Tests können Fehler oder Kompatibilitätsprobleme auftreten. Die Software wird ohne Gewährleistung bereitgestellt; die Nutzung erfolgt auf eigene Verantwortung. Eine Haftung für mögliche Schäden oder Datenverluste wird, soweit gesetzlich zulässig, ausgeschlossen. Es gelten die jeweiligen Lizenzbedingungen.

**Feedback, Fehlermeldungen und Verbesserungsvorschläge sind herzlich willkommen!**

Wenn dieses kleine Werkzeug auch anderen die Hörspielarbeit erleichtert und mehr Raum für Geschichten und menschliche Kreativität schafft, freut mich das ganz besonders.
