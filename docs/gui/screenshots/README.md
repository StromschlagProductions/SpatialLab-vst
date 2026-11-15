# Prüfbilder der implementierten JUCE-Oberfläche

Code-Stand: `49befc5dd97a13dbaf90ee8fbe6ff8552b1d0c61`.
[Erfolgreicher Windows-/Linux-CI-Lauf](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43727258661).
[Originales Build-Artefakt mit VST3 und Bildern](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43727258661/artifacts/14167326568).

Die Dateien wurden unverändert aus diesem CI-Artefakt übernommen. Es fand keine Pixelbearbeitung, Montage oder Bildgenerierung statt. Der native Prüfhost öffnete denselben Editor wie die VST3 und speiste ausschließlich für die Vorschau synthetisches Audio in den bestehenden Prozessor. Gezeigt: Wohnzimmer, neutrale Größe/Klang, Stereo, tatsächlich vorbereitete 48 kHz.

| Datei | Nachweis | Abmessungen |
| --- | --- | --- |
| [Windows-Editor-Screenshot.png](Windows-Editor-Screenshot.png) | **Echter Bildschirm-Screenshot** der sichtbaren Editorfläche, Windows/GDI-BitBlt | 1003×565 |
| [JUCE-Editor-Render-100.png](JUCE-Editor-Render-100.png) | `createComponentSnapshot`, 100 % | 1003×565 |
| [JUCE-Editor-Render-125.png](JUCE-Editor-Render-125.png) | `createComponentSnapshot`, 125 % | 1254×706 |
| [JUCE-Editor-Render-150.png](JUCE-Editor-Render-150.png) | `createComponentSnapshot`, 150 % | 1504×848 |

Die drei Render sind direkt vom Produktionseditor gezeichnet, aber **keine Bildschirmfotos bei drei unterschiedlichen Windows-DPI-Einstellungen**. Reale DAW-/DPI-Prüfungen bleiben Teil der manuellen Abnahme. Die Bilder wurden visuell auf Ausrichtung, doppelte Knöpfe und Lesbarkeit geprüft; sie belegen keine Hör-/DAW-Freigabe.

Im selben Lauf bestanden unverändert die bisherigen DSP- und Prozessor-Tests, die neuen GUI-Isolationstests und die Linux-Sanitizer. Die PNG-Prüfung bestätigt Abmessungen, CRCs und genau eine Aufnahme pro Datei.

SHA-256 des echten Screenshots: `7783f2771c6c02af904a64bdfc29e19df7c7656f23e9f162e1c85ad935de73e9`.

![Echter Windows-Screenshot des Produktionseditors](Windows-Editor-Screenshot.png)
