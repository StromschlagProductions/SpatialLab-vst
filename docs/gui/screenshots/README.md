# SPATIAL LAB – Audio Drama Spatial Environments

[Aktuelle vier Oberflächen und Bildherkunft](../../classic/CLASSIC_GUI.md) · [Versionsbericht 1.3](../../v1.3/UPDATE.md)

![Aktueller echter Windows-Screenshot – Retro Episode IV](../../oberflaeche-retro.png)

Die folgenden Nachweise dokumentieren ältere Entwicklungsstände; historische Bilddateien bleiben unverändert erhalten.

---

# Historischer Stand: Phase 2.1

[Vier große echte Windows-Screenshots, individuelle Röhrenglut bei Stille/Audio, Build und Installationspaket](../../phase2.1/PRUEFBILDER.md). Die folgenden Bilder dokumentieren frühere Stände.

---

# Historischer Stand: Entwicklungsphase 2

[Sechs aktuelle echte Windows-Screenshots mit drei Raumzeichnungen, Build und Installationspaket](../../phase2/screenshots/README.md). Die folgenden Nachweise sind historische Stände vor Phase 2.

---

# Aktuelle Episode-Frontplatten: echte Windows-Screenshots

Code-Stand: `df416f805644b7b8ae1dac337f411872104da6aa`.
[Erfolgreicher Build samt Tests](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43745098224).
[Installationspaket](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43745098224/artifacts/14174007474).
[Build-Artefakt mit weiteren 100/125/150-%-JUCE-Rendern](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43745098224/artifacts/14174007481).

| Ansicht | Echter Screenshot | Schalterstellung |
| --- | --- | --- |
| Episode I, fabrikneu | [Episode-I.png](Episode-I.png) | links |
| Episode IV, verwittert, Standard | [Episode-IV.png](Episode-IV.png) | rechts |

Beide Dateien sind unveränderte native Windows/GDI-Bildschirmaufnahmen des laufenden Produktionseditors im JUCE-Prüfhost, jeweils 1003×565 Pixel. Keine Konzeptbilder, Montage oder nachträgliche Pixelbearbeitung. Die Original-Frontplatten sind unverändert mit 3344×1882 Pixeln eingebettet. Nur der Prüfhost speist synthetisches Audio ein. Die im Artefakt zusätzlich enthaltenen JUCE-Komponenten-Render sind keine Bildschirmfotos verschiedener Windows-DPI-Einstellungen.

Visuell geprüft: beide Skin-Zustände, vollständige Beschriftung `EPISODE I`/`EPISODE IV`, Hebel links/rechts, Abstand zu allen vier Nieten und zum erhabenen Rand. GUI-Test zusätzlich: Standard IV, Umschalten, getrennte Wiederherstellung, unveränderter Audio-State, bitidentische Samples sowie Hitbox-Grenzen bei drei Fenstergrößen. Windows-VST3, alle drei Tests und Linux-ASan/UBSan sind grün.

Das heruntergeladene Installations-ZIP wurde per CRC und Inhalt geprüft: genau ein Wurzelordner `SpatialLab.vst3`, darin `Contents/Resources/moduleinfo.json` und `Contents/x86_64-win/SpatialLab.vst3`. SHA-256 des inneren Installations-ZIPs: `165cd3ea5a24b9810396f3ec0524352597dabd896cec3a72df39746e93a6f225`.

[Bedienung und separate GUI-Einstellung](../EPISODES.md). Der Nachweiscommit ergänzt ausschließlich Dokumentation und die beiden Screenshots; der geprüfte Plugin-Code bleibt identisch. Reale DAW-/DPI-Abnahme ist nicht durch diese Prüfhost-Aufnahmen ersetzt.

---

## Historischer Stand vor den finalen Episode-Grafiken

# Prüfbilder der implementierten JUCE-Oberfläche

Code-Stand: `6d490fc6019ba3352ba761ab02b247cde2d5f456`.
[Erfolgreicher Windows-/Linux-CI-Lauf](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43733243192).
[Originales Build-Artefakt mit VST3 und Bildern](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43733243192/artifacts/14169382572).

Die Dateien wurden unverändert aus diesem CI-Artefakt übernommen. Es fand keine Pixelbearbeitung, Montage oder Bildgenerierung statt. Der native Prüfhost öffnete denselben Editor wie die VST3 und speiste ausschließlich für die Vorschau synthetisches Audio in den bestehenden Prozessor. Gezeigt: Wohnzimmer, neutrale Größe/Klang, Stereo, tatsächlich vorbereitete 48 kHz. Das Original-Logo erscheint als dunkelroter Siebdruck auf lokal rekonstruierter Metalltextur; die bisherige Metallplakette ist entfernt.

| Datei | Nachweis | Abmessungen |
| --- | --- | --- |
| [Windows-Editor-Screenshot.png](Windows-Editor-Screenshot.png) | **Echter Bildschirm-Screenshot** der sichtbaren Editorfläche, Windows/GDI-BitBlt | 1003×565 |
| [JUCE-Editor-Render-100.png](JUCE-Editor-Render-100.png) | `createComponentSnapshot`, 100 % | 1003×565 |
| [JUCE-Editor-Render-125.png](JUCE-Editor-Render-125.png) | `createComponentSnapshot`, 125 % | 1254×706 |
| [JUCE-Editor-Render-150.png](JUCE-Editor-Render-150.png) | `createComponentSnapshot`, 150 % | 1504×848 |

Die drei Render sind direkt vom Produktionseditor gezeichnet, aber **keine Bildschirmfotos bei drei unterschiedlichen Windows-DPI-Einstellungen**. Reale DAW-/DPI-Prüfungen bleiben Teil der manuellen Abnahme. Die Bilder wurden visuell auf nahtlose Logo-Integration, Ausrichtung und Lesbarkeit geprüft; sie belegen keine Hör-/DAW-Freigabe.

Im selben Lauf bestanden unverändert die bisherigen DSP- und Prozessor-Tests, die neuen GUI-Isolationstests und die Linux-Sanitizer. Die PNG-Prüfung bestätigt Abmessungen, CRCs und genau eine Aufnahme pro Datei.

SHA-256 des echten Screenshots: `8a00e35d799d3ae7bb25fa3cf382644ffacc12ef17e13021c59c72f96203fe4c`.

[Historischer Windows-Screenshot](Windows-Editor-Screenshot.png)

Der neue echte Screenshot wurde mit dem Screenshot aus `32f4d8a` verglichen: Alle Pixel außerhalb des Logo-Bereichs und der bereits vorhandenen animierten CRT-Signalspur sind identisch. Die Signalspur zeigt aufgrund unterschiedlicher Aufnahmezeitpunkte eine andere Animationsphase; deren Code wurde nicht geändert. Audiocode, Parameter, Messwertpfad, Bedienflächen und sonstige GUI-Logik sind gegenüber `32f4d8a` unverändert.
