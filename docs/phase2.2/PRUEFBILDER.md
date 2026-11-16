# Phase 2.2 – echte Windows-Aufnahmen

Geprüfter Plugin-Code: `9ca058d742acf6f3bb90dbc67401f2f2ecec5ca5`. Alle PNGs sind unveränderte GDI-Aufnahmen des laufenden JUCE-Produktionseditors im Windows-Prüfhost, keine DAW-Screenshots.

| Reglerstellung | Episode I | Episode IV |
| --- | --- | --- |
| 0 | [PNG](screenshots/Episode-I-Regler-0.png) | [PNG](screenshots/Episode-IV-Regler-0.png) |
| 0.25 | [PNG](screenshots/Episode-I-Regler-0.25.png) | [PNG](screenshots/Episode-IV-Regler-0.25.png) |
| 0.5 | [PNG](screenshots/Episode-I-Regler-0.5.png) | [PNG](screenshots/Episode-IV-Regler-0.5.png) |
| 0.75 | [PNG](screenshots/Episode-I-Regler-0.75.png) | [PNG](screenshots/Episode-IV-Regler-0.75.png) |
| 1 | [PNG](screenshots/Episode-I-Regler-1.png) | [PNG](screenshots/Episode-IV-Regler-1.png) |

[Episode I in Standardgröße](screenshots/Episode-I-1504x846.png) · [Episode IV in Standardgröße](screenshots/Episode-IV-1504x846.png).

- [Erfolgreicher Windows-/Linux-Build und Tests](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43810951479)
- [Neues Windows-VST3-Installationspaket](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43810951479/artifacts/14200883717)
- [Vollständiges Artefakt mit Röhrensequenzen, Originalframes, Reglerbildern und Messungen](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43810951479/artifacts/14200664515)
- [Diagnose, CPU-Prüfung und gezielter Hörvergleich](PHASE2_2.md)
- [Windows-Tieftonmessung](room-diagnostics-windows.csv)

Alle fünf Windows-Tests, Linux-Sanitizer, Herstellerprüfung, statische Leuchtfelder, Röhrenanimation und feststehende äußere Reglerreflexionen bestanden. Im CI werden beide Regler in beiden Skins auf identische äußere Randpixel und veränderte innere Knopfflächen geprüft. Der vorhandene Audiocode und die beiden Original-PNGs bestehen die Hash-Prüfungen. Die Röhrenzeichnung und Glühfunktion sind gegenüber Phase 2.1 unverändert; die CRT-Wellenform bleibt pegelabhängig.

Installations-ZIP nach Download auf CRC und vollständigen SpatialLab.vst3-Ordner geprüft. SHA-256 des inneren ZIPs: `916358bb615ad7b246249a671ba0740e5cea96a4ff47d6e257ffb8ee97896cfc`. Der Nachweiscommit ergänzt nur Dokumentation/Bilder und verändert den geprüften Plugin-Code nicht.
