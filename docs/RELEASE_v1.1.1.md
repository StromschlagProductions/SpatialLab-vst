# SPATIAL LAB v1.1.1 – Außenraum-Effizienz und Robustheit

Markus' Wunsch nach trockeneren Außenräumen ist in der separaten Outdoor-Engine bereits umgesetzt: diskrete Stereo-Echos und begrenzte Rückkopplung statt eines dichten diffusen Hallnetzes. SIZE verändert dort auch die Echo-Abstände (Faktor 0,5 bis 2); einen separaten Echo-Zeit-Regler gibt es bewusst nicht. Das Plugin bleibt ein 100-%-Wet-Effekt für FX-/Send-Kanäle.

Neu in 1.1.1:

- Stabile Outdoor-Laufzeiten benötigen keine doppelten Lesevorgänge für bereits abgeschlossene Überblendungen. Ringpuffer-Lesezugriffe vermeiden unnötige Modulo-Operationen.
- Ein Vergleich aller sieben Outdoor-Profile mit 150.000 Stereo-Samples je Profil (inklusive SIZE-/TONE-Änderung) bestätigte identische Float-Ausgaben im untersuchten Build. Die 19 vorhandenen Innenraumprofile werden weiterhin gegen die eingefrorene Referenz geprüft.
- Ungültige Eingangs-/Steuerwerte werden abgefangen; Null-Sample-Callbacks werden vor Puffer-/Meterzugriffen beendet.
- Neue Grenzfalltests für alle 26 Presets bei 22.050, 44.100, 48.000, 96.000 und 192.000 Hz.

Die ursprünglichen Grafiken und die eingefrorenen DSP-Referenzen bleiben unverändert. Der freigegebene Quellcode-Prüfsummenstand wurde für diese technische Änderung ausdrücklich auf 1.1.1 fortgeschrieben. Der Release-Workflow prüft das Paket weiter gegen den erfolgreich abgenommenen Build.

Installation: ZIP entpacken und den vollständigen Ordner `SpatialLab.vst3` bei geschlossener DAW nach `C:\Program Files\Common Files\VST3` kopieren. Die alten Releases bleiben erhalten.
