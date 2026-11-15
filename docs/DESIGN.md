# Technisches Konzept: Statischer Hall für Send-Kanäle

## Genau drei Bedienelemente

SPATIAL LAB ist ein statischer, presetbasierter Raumhall. Raum auswählen, Send-Pegel in der DAW einstellen und Größe/Klang nur bei Bedarf anpassen.

| Parameter-ID | Anzeige | Wertebereich | Standard |
|---|---|---|---|
| `room` | Raum | Wohnzimmer, Küche, Badezimmer (Indizes 0–2) | Wohnzimmer |
| `size` | Größe | 0 kleiner, 0,5 neutral, 1 größer | 0,5 |
| `tone` | Klang | 0 dunkler, 0,5 neutral, 1 heller | 0,5 |

Ein Raumwechsel setzt Größe und Klang auf 0,5 zurück – auch bei Host-Automation und geschlossenem Editor. Die erneute Auswahl desselben Raums löscht keine Makroautomation. Beide Makros sind kontinuierlich ohne 0,01-Raster. Nach einem Raumwechsel eingehende Automation gilt in der vom Host gelieferten Reihenfolge. Parameter werden einmal pro Audioblock gelesen; die Koeffizienten und die interne Klangabstimmung gleiten über 50 ms. Das ist blockgenau, keine samplegenaue VST3-Offset-Automation.

Es gibt keine Raumüberblendung, Entfernungs- oder Bewegungssimulation und kein Oversampling. Außenräume sind eine spätere Erweiterung; der erste Meilenstein enthält drei Innenräume.

Frühe Reflexionen, Hallfahne, Verzögerungen, Diffusion, spektrale Dämpfung, Stereobild und Pegelabstimmung sind interne Presetwerte. Es gibt weder Dry/Wet, Output Gain noch separate Early/Late-, Decay-, Damping- oder Width-Regler – auch nicht versteckt automatisierbar. Das Ergebnis ist immer 100 % Wet. Direktsignal und Send-Pegel liegen in der DAW. Doppelklick setzt Größe/Klang auf neutral zurück.

## Signalweg und Raumcharakter

Mono-Eingang wird vor der Hallberechnung auf zwei Kanäle verteilt. Bei Mono-Ausgang wird das interne Stereo-Hallsignal gemittelt. Stereo-Eingänge regen die Raumkanäle unabhängig an. Unterstützt: Mono → Mono, Mono → Stereo, Stereo → Stereo. Surround und Stereo → Mono werden abgelehnt.

Jeder Raum verwendet sechs vorzeichenbehaftete frühe Reflexionen mit unterschiedlichen Links-/Rechts-Ankunftszeiten sowie ein gedämpftes Feedback-Delay-Netzwerk mit acht Leitungen. Eine Householder-Matrix verteilt die verzögerten Signale; die Leitungsdämpfung folgt `10^(-3 * delaySeconds / RT60)`. Verzögerungszeiten sind je Preset festgelegt und werden bei der Vorbereitung auf die Samplerate umgerechnet. Ein einpoliger Tiefpass pro Leitung bildet frequenzabhängiges Abklingen nach; alle Schleifenverstärkungen bleiben unter 1.

Größe verändert die nominelle RT60 mit `2^(2*(size-0.5))` (Faktor 0,5 bis 2). Zusätzlich ändern sich intern frühe und späte Anteile: `earlyGain = 1.2 - 0.4*size`, `lateGain = 0.65 + 0.7*size`; neutral sind beide 1. Klang verändert die Dämpfungs-Grenzfrequenz mit `presetCutoff * 2^(2*(tone-0.5))`, begrenzt unterhalb Nyquist. Bei neutral gelten die abgestimmten Presetwerte. Automatische Tests prüfen längere Hallenergie beziehungsweise mehr normierte Hochfrequenzenergie bei den jeweiligen Extremen. Das ersetzt keinen Hörtest.

Nur der gewählte Raum berechnet Audio; alle Verzögerungsspeicher werden im Voraus angelegt. Ein Raumwechsel löscht die bisherige Hallfahne, setzt die neuen Makrowerte und startet ohne Überblendung. Dadurch kann ein Wechsel während des Klangs hörbar unterbrechen. Für Szenenbewegungen ist LOCATIONS zuständig.

## Echtzeitverhalten und Parameterverwaltung

Dynamische Speicher werden bei `prepare` angelegt. Verarbeitung und Raumwechsel verwenden vorbereitete Puffer, begrenzte Schleifen, lockfreie atomare Steuerwerte und Koeffizientenglättung. Im DSP- und Raumwechselpfad finden weder XML-Verarbeitung, Dateizugriffe, Logging, Locks, Host-Benachrichtigungen, Speicherallokationen noch Betriebssystemnachrichten statt. Unveränderte Makrowerte verwenden vorhandene Koeffizienten; Ringpuffer-Indizes werden ohne Laufzeitdivision zurückgesetzt. Der Raumwechsel löscht begrenzte Speicher und kann deshalb teurer als ein gewöhnlicher Block sein.

JUCE verwaltet drei Ranged-Parameter, nicht einen APVTS mit alten Expertenparametern. Ein Choice-Callback setzt bei einem tatsächlichen Raumwechsel beide Makro-Atomics sofort zurück und erhöht eine lockfreie Revision. Der Audiocallback wertet die Revision aus, sodass auch A → B → A zwischen zwei Blöcken die alte Hallfahne löscht. Wiederherstellen eines Zustands verwirft ebenfalls die laufende Hallfahne. Ein Timer im Message-Thread meldet die aktuellen Makrowerte später an Host-Listener, überschreibt aber keine neueren Automationswerte. Der Editor liest die Werte zusätzlich zu JUCE-Slider-Attachments. Für den DSP-Reset ist kein Editor und kein Message-Dispatch erforderlich.

`processBlock` unterstützt auch null, einzelne und größere Hostblöcke. JUCE deaktiviert Denormals während der Verarbeitung. Reset löscht Speicher ohne Resize. State-, Editor- und Timer-Arbeit finden außerhalb des Audiocallbacks statt; der Host-Lebenszyklus muss Prepare, Reset und State-Restore mit der Verarbeitung synchronisieren.

## Projektzustand und Migration

Der binäre XML-Zustand mit Wurzel `SpatialLabState` und `schemaVersion=3` speichert Raumindex und Größe/Klang. Beim Laden wird zuerst der Raum, danach werden die gespeicherten Makrowerte wiederhergestellt. Ungültige Wurzeln, unbekannte Versionen sowie Werte außerhalb des Bereichs oder ohne endlichen Zahlenwert werden abgewiesen.

Prototypzustände der Schemata 1 und 2 übernehmen nur den Raum mit neutralen Makros. Veraltete Mix-/Gain-/Early-Late-/Width-/Decay-/Damping-Werte werden verworfen. Alte Automations-IDs werden nicht neu belegt; entsprechende DAW-Automationsspuren müssen entfernt werden. Die Parameter-Versionshinweise sind 1.

Gespeichert werden Bedienelemente, keine laufenden Hallfahnen. Herstellerkennung `StPr`, Plugincode `SpLb` und Bundle-ID `com.stromschlagproductions.spatiallab` bleiben stabil. Raumindizes, IDs und Bereiche dürfen nach Veröffentlichung nicht ohne Migration geändert werden.

## Sampleraten, Bypass und Grenzen

44,1 und 48 kHz sind vollständig in Regressionstests und CPU-Messungen abgedeckt. Für 96/192 kHz existieren nur kurze Funktionstests. Oversampling ist für diesen linearen Ansatz nicht erforderlich.

Der Bypass-Callback verarbeitet intern weiter und schaltet den Send-Ausgang stumm. Manche DAWs umgehen das Plugin stattdessen extern; dann muss der Return-Kanal gegebenenfalls stummgeschaltet werden.

Statische Verzögerungen können Dauertöne färben. Die Räume sind gestaltete Klangeindrücke, keine vermessenen Räume. Vor einer Veröffentlichung sind Hörtests, Hostvalidierung und CPU-Deadline-Tests erforderlich. Limiter, physikalisches Szenenmodell, gemessene IRs, zusätzliche Skins und Außenräume gehören nicht zum ersten Meilenstein. Die gemeldete Hallfahne von sechs Sekunden ist konservativ und keine harte interne Begrenzung; Latenz: null.
