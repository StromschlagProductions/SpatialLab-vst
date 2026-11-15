# Phase 2: echte Windows-Screenshots

Geprüfter Code: `0482b7e9275c128dc64935ae3196ad9a2bc89f40` im Branch `develop/retro-gui`.

| Raum | Episode I | Episode IV (Standard) |
| --- | --- | --- |
| Wohnzimmer | [Screenshot](Episode-I-Wohnzimmer.png) | [Screenshot](Episode-IV-Wohnzimmer.png) |
| Küche | [Screenshot](Episode-I-Kueche.png) | [Screenshot](Episode-IV-Kueche.png) |
| Badezimmer | [Screenshot](Episode-I-Badezimmer.png) | [Screenshot](Episode-IV-Badezimmer.png) |

Alle sechs PNGs sind unverändert übernommene echte Windows/GDI-Aufnahmen des sichtbaren Produktionseditors im JUCE-Prüfhost, jeweils 1003×565 Pixel. Keine Montage oder Nachbearbeitung. Beide Originalfrontplatten bleiben unverändert mit 3344×1882 Pixeln eingebettet. Visuell geprüft: drei unterschiedliche bernsteinfarbene Raumzeichnungen, beide Schalterstellungen, Beschriftungen und Positionen der Lichtüberlagerungen. Der Prüfhost speist synthetisches Audio ein; die Anzeige entspricht tatsächlich vorbereiteten 48 kHz/Stereo. Die zusätzlichen 100/125/150-%-JUCE-Render im Build-Artefakt sind keine Bildschirmfotos unterschiedlicher Windows-DPI-Einstellungen. Reale DAW-Abnahme und der erneute Sprachhörtest bleiben ausstehend.

- [Erfolgreicher Windows-/Linux-Build, alle fünf Tests und Sanitizer](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43749906538)
- [Installationspaket mit vollständigem SpatialLab.vst3-Ordner](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43749906538/artifacts/14176690677)
- [Vollständiges Build-Artefakt, zwölf alte/neue Impulsantwort-WAVs, CSV und weitere GUI-Render](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43749906538/artifacts/14175759682)
- [Tatsächlich erzeugte Plugin-Metadaten](../moduleinfo-windows.json)
- [Windows-Abklingmessungen bei 44,1/48 kHz](../room-metrics-windows.csv)
- [Akustische Änderungen und Hörtestanleitung](../AKUSTIK.md)

Das heruntergeladene innere Installations-ZIP wurde per CRC und Inhalt geprüft: ausschließlich `SpatialLab.vst3/`, darin DLL und Ressourcen mit `moduleinfo.json`. SHA-256: `3ff9a8c920899d6e578fac6fcbb6d8fe5f80871a38db32f333a838d40ccd1d26`. Factory und alle Klassen nennen **Stromschlag Audio**; die drei VST3-CIDs entsprechen dem bisherigen Build. Beide Originalgrafiken bestehen die SHA-256-Prüfung. Der anschließende Nachweiscommit ändert ausschließlich Dokumentation und übernimmt diese Prüfbilder; der geprüfte Plugin-Code bleibt identisch.
