# Prüfstand und noch offene Tests

Stand: 15. November 2026. Audio-Grundlage: Branch `develop/juce8-room-foundation`. Den genau getesteten Commit zeigen die Prüfungen in PR #1.

## Haupt-Abtastraten: 44,1 und 48 kHz

Die eigenständige DSP-Testsuite läuft bei beiden Haupt-Abtastraten. Sie prüft:

- Unterschiedliche, nicht stumme Raumantworten; kein Direktsignal; Stereodiffusion aus Mono; endliche, begrenzte Impulse und abklingende Hallfahnen.
- Interne frühe und späte Anteile sowie unveränderten Presetcharakter bei neutralen Makros.
- Mehr späte Hallenergie bei größerem Größenwert und mehr normierte Hochfrequenzenergie (erste Differenz) bei hellerem Klang – für alle drei Räume.
- Reset und harten Raumwechsel ohne Überblendung oder Wiederauftauchen einer alten Hallfahne.
- Schnelle Makroautomation und wiederholte Raumwechsel ohne unbeschränkte Werte und ohne gemessene gewöhnliche Heap-Allokationen.
- Identische Sample-Glättung bei gleichen Zielwerten und unterschiedlichen Blockgrenzen.

Die JUCE-Prozessortests prüfen genau drei sichtbare Parameter, alle unterstützten Mono-/Stereo-Busbelegungen und die Ablehnung nicht unterstützter Belegungen, den Wet-Signalweg gegen die eigenständige Engine, Blockgrößen 0/1/7/64/511/2048, beide Haupt-Abtastraten und alle Räume. Außerdem werden stummer Send-Bypass, sofortiger neutraler Makro-Reset bei Host-Raumwechsel ohne geöffneten Editor, Erhalt der Automation bei erneuter Wahl desselben Raums, A → B → A zwischen Audiocallbacks, Session-Recall mit angepassten Makros, Migration von Schema 1/2, Ablehnung ungültiger und zukünftiger Zustände sowie Editor-Erstellung und -Layout geprüft.

96 und 192 kHz erhalten kurze Tests auf endliche Ausgaben und Verzögerungen, aber keine vollständige Klangabstimmung oder CPU-Abdeckung. Kein Oversampling.

Lokale native Release-/CTest-Läufe verwenden GCC 14.2/C++17. Lokale ASan-/UBSan-Läufe benötigen `ASAN_OPTIONS=detect_leaks=0`, weil LeakSanitizer im Cloud-Sandbox-Kontext Threads wegen einer ptrace-Beschränkung nicht prüfen kann. Linux-CI behält die vollständige ASan-/UBSan- und Leak-Konfiguration bei. Das ist eine Einschränkung der lokalen Umgebung, keine deaktivierte CI-Prüfung.

Die Allokationsmessung erfasst gewöhnliche C++-Heap-Allokationen im eigenständigen DSP, nicht Host oder Betriebssystem. Numerische Tests und Allokationsmessungen beweisen weder harte Echtzeitfähigkeit noch hörbare Qualität.

## CPU-Beobachtungen (informativ)

`SpatialLabDspBenchmark` misst 44,1/48 kHz, Mono/Stereo und jeweils vier Sekunden pro Fall: unveränderte Regler, Makroautomation alle 64 Frames und Presetwechsel alle 2048 Frames. Es wird immer nur ein ausgewählter Raum verarbeitet; alle Speicher sind vorbereitet. Ausgegeben werden Nanosekunden pro Frame und der mittlere Anteil eines CPU-Kerns. Windows-CI lädt `dsp-benchmark.csv` zusammen mit VST3 und Buildnachweisen hoch. Es gibt keinen maschinenabhängigen Zeit-Grenzwert für einen fehlgeschlagenen Test.

Nach der Umstellung auf nur eine aktive Raum-Engine lagen lokale GCC-Release-Messungen bei ungefähr **34–40 ns/Frame** beziehungsweise **0,15–0,20 % eines CPU-Kerns**. Der ältere Prototyp mit drei Engines lag auf derselben Cloud-Maschine bei ungefähr 94–124 ns/Frame. Diese Beobachtungen enthalten Messrauschen und Änderungen an Architektur und Klangabstimmung. Sie erfassen keine DAW-Deadline, keinen Editor-/Host-Overhead und nicht den schlimmsten einzelnen Raumwechsel-Callback.

## Vor einer Veröffentlichung noch erforderlich

- Das vollständige Windows-VST3-Bundle in mindestens zwei DAWs laden und neu scannen. Mono-/Stereo-Return, Drei-Regler-Oberfläche und Neutral-/Reset-Verhalten prüfen.
- Dialog bei 48 kHz und Musik bei 44,1 kHz hören; außerdem Impulse, Dauertöne und Transienten in jedem Raum. Klangfärbung, Dichte, Natürlichkeit, Stereobild und Monokompatibilität beurteilen.
- Größe/Klang automatisieren und wiedergeben. Raumwechsel über Editor und Hostautomation bei geschlossenem Editor prüfen, einschließlich neutralem Reset und nachfolgender Makroautomation. Szenenwechsel gehören zu LOCATIONS und erwarten keine Raumüberblendung.
- Angepasste Sessions speichern und erneut öffnen, Instanzen duplizieren, Bypass ein-/ausschalten, Wiedergabe stoppen/starten, Samplerate und Puffergröße ändern. Der Plugin-Bypass-Callback schaltet stumm, manche DAWs leiten das Signal jedoch extern um das Plugin herum.
- CPU-Deadlines mit vielen Instanzen und kleinen Puffern messen, auch den begrenzten Speicher-Reset bei Raumwechseln. Durchschnittliche Microbenchmark-Werte garantieren keine Echtzeit-Deadline.
- Unabhängige VST3-Validierung beziehungsweise pluginval, Langzeittests und Prüfung der Distributionslizenz durchführen.

Dieser Meilenstein beansprucht **keine** abgeschlossenen Hörtests, DAW-Zertifizierung, unabhängige VST3-Validierung oder signierte Produktionsfreigabe.
