# Phase 2.1 – echte Windows-Aufnahmen

Geprüfter Code: `216505ee65d0c18a59202fa1528dab86f862ed05` auf `develop/retro-gui`.

Die Screenshots sind unveränderte GDI-Aufnahmen der sichtbaren Produktionsoberfläche im Windows-Prüfhost. Beide Skins werden bei Standard- und Maximalgröße aufgenommen; die vollständigen Einzelbilder und Zeitstempel liegen im Build-Artefakt. Keine Mockups und keine Aufnahmen aus Fender Studio Pro.

| Größe | Episode I | Episode IV |
| --- | --- | --- |
| Standard 1504×846 | [PNG](screenshots/Episode-I-1504x846.png) | [PNG](screenshots/Episode-IV-1504x846.png) |
| Maximum 1672×941 | [PNG](screenshots/Episode-I-1672x941.png) | [PNG](screenshots/Episode-IV-1672x941.png) |

## Individuelle Röhrenglut oben rechts

Die folgenden GIFs sind klar bezeichnete **Detailausschnitte echter Windows-Aufnahmen**. Es werden nur echte Pixel des Röhrenfensters gezeigt, keine künstlich erzeugten Zwischenbilder. Die Animation berücksichtigt die tatsächlich erfassten Aufnahmezeitpunkte. Die ursprünglichen verlustfreien PNG-Frames liegen im Artefakt; GIF reduziert technisch die Farbpalette.

| Episode / Größe | Absolute Stille | Audio |
| --- | --- | --- |
| I / 1504×846 | [Röhren-GIF](animations/Episode-I-1504x846-Silent.gif) | [Röhren-GIF](animations/Episode-I-1504x846-Audio.gif) |
| I / 1672×941 | [Röhren-GIF](animations/Episode-I-1672x941-Silent.gif) | [Röhren-GIF](animations/Episode-I-1672x941-Audio.gif) |
| IV / 1504×846 | [Röhren-GIF](animations/Episode-IV-1504x846-Silent.gif) | [Röhren-GIF](animations/Episode-IV-1504x846-Audio.gif) |
| IV / 1672×941 | [Röhren-GIF](animations/Episode-IV-1672x941-Silent.gif) | [Röhren-GIF](animations/Episode-IV-1672x941-Audio.gif) |

Die Stillesequenzen erfassen mindestens 14 Sekunden mit je 48 Detailbildern. Jede der vier Röhren wird separat auf steigende und fallende Helligkeit geprüft. Zusätzlich enthalten die Artefaktordner vollständige Bildschirm-Bildfolgen und GIFs, in denen sich die übrigen Kontrollleuchten und die bestehende CRT-Wellenform prüfen lassen. Die Stille ist digital null; Audio verwendet ausschließlich im Prüfhost synthetische Anregung bei 48 kHz/Stereo. Die Rendering- und Filmaufnahmefunktion ist nicht Teil des VST3-Plugins.

## Gesamte Oberfläche und Kontrollleuchten

- [Echte vollständige GUI-Animation, Episode IV bei digitaler Stille](animations/Whole-GUI-Episode-IV-Silent.gif)
- [Echte vollständige GUI-Animation, Episode IV mit Audio](animations/Whole-GUI-Episode-IV-Audio.gif)

Die vollständigen GIFs sind jeweils etwa 15 MB groß. Ihre Original-PNGs und tatsächlichen Aufnahmezeitpunkte sind ebenfalls im Build-Artefakt enthalten. Die Detailsequenzen wurden in beiden Skins und bei beiden großen Größen auf Position, individuelles Auf-/Abglimmen und orangefarbene Restglut kontrolliert. Die Schalterfassung ist in Episode IV einschließlich Innen-/Außenrand sichtbar matter und verschmutzt; Nieten, Plattenrand und Beschriftungen bleiben frei.

## Verifizierter Build und Installationspaket

- [Erfolgreicher Windows-/Linux-CI-Lauf 43756396801](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43756396801)
- [Windows-VST3-Installationspaket](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43756396801/artifacts/14179455197)
- [Vollständiges Build-Artefakt mit allen ursprünglichen Frames, Zeitstempeln, GIFs, Toolchain- und A/B-Daten](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43756396801/artifacts/14178817267)
- [Tatsächlich erzeugte Modul-Metadaten](moduleinfo-windows.json)
- [Sichtbarkeits- und Restglutmessungen](lighting-visibility.json)
- [Änderungen und Windows-Installationsanleitung](PHASE2_1.md)

Windows: DSP, RetroMotion, RoomVoicing, Processor und RetroGUI bestanden. Linux: ASan/UBSan einschließlich Leak-Prüfung bestanden. Die Herstellerdefinitionen Strm/SpLb, neue Bundle-ID, Factory-Hersteller und alle drei neuen VST3-CIDs wurden am tatsächlichen Build geprüft. Hash-Prüfungen bestätigen die unveränderte freigegebene Audio-Engine und beide Originalfrontplatten.

Beide heruntergeladenen Actions-Artefakte stimmen per SHA-256 mit GitHubs Artefakt-Digests überein. Das innere Installations-ZIP wurde zusätzlich per CRC und Struktur geprüft: ausschließlich vollständiger Ordner `SpatialLab.vst3` einschließlich `Contents/x86_64-win/SpatialLab.vst3` und `Contents/Resources/moduleinfo.json`. SHA-256 des inneren ZIPs: `c90d38d4cf7b68b025e27e418f5611a398e206dc424f14ec9b7608f06da67580`.

Die Restglut beträgt in den aufgenommenen vollständigen Glasflächen etwa 17–24 % der maximalen mittleren RGB-Helligkeit; alle vier Röhren zeigen steigende und fallende Abschnitte bei Stille. Diese Messung ergänzt die Sichtprüfung, sie ist kein photometrischer Wert. Die ursprünglichen Logos/Frontplatten werden nicht verändert. Der Nachweiscommit ergänzt nur Dokumentation und echte Prüfbilder; der getestete Plugin-Code bleibt identisch. Praktische Host-Sortierung und Größenwiederherstellung in Fender Studio Pro sind auf dem Zielrechner nach dem gezielten Neuscan zu bestätigen.
