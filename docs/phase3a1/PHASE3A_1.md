# SPATIAL LAB Phase 3A.1 – Preset-Browser und Displaytausch

[Windows-VST3-Testbuild herunterladen](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43896594181/artifacts/14239954666) · [38 echte Windows-Screenshots](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43896594181/artifacts/14242141140) · [Erfolgreicher Windows-/Linux-Lauf](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43896594181)

## Bedienung

Das große mittlere CRT enthält jetzt einen direkten Preset-Browser. Bernsteinfarbene Schrift auf schwarzem Hintergrund, ohne zusätzliche Überschrift, mit etwa zwölf sichtbaren Zeilen statt drei. Die Schrift bleibt bei 18 nativen Pixeln für Presets und 16 für Kategorien; die Ansicht skaliert proportional mit dem Fenster.

- **Mausrad über dem Browser:** Liste vertikal verschieben, ungefähr drei Zeilen je konventioneller Mausrad-Rastung. Kleine Trackpad-Deltas scrollen entsprechend fein. Das Rad wählt keinen Raum und verändert weder SIZE noch TONE.
- **Preset anklicken:** Genau die bisherige Umgebung auswählen. Der aktive Eintrag ist bernsteinfarben hinterlegt. Ein weiterer Klick auf den aktiven Eintrag löst keine neue Auswahl aus.
- **Kategorie anklicken:** Auf-/zuklappen. Kategorien sind unabhängig; ein Doppelklick schaltet nicht versehentlich zweimal um. Es gibt INNENRÄUME, FAHRZEUGE, SPEZIALRÄUME und SCI-FI mit unveränderten 9/4/3/3 Presets. Keine leere oder auswählbare Outdoor-Kategorie.
- **Scrollleiste rechts:** Position ablesen, Leiste anklicken oder den Schieber ziehen. Die Liste ist oben/unten begrenzt.
- **Host-Automation/Session-Wiederherstellung:** Passende Kategorie öffnen und den aktiven Eintrag vollständig sichtbar machen. Auch ein Recall desselben Raums öffnet dessen zuvor geschlossene Kategorie. Nur soweit nötig scrollen; bereits sichtbare Einträge verursachen keinen Sprung zum Anfang.
- **Vorhandene Pfeile links:** Weiterhin voriger/nächster Raum, zyklisch über dieselben 19 Umgebungen. Direkte Wahl und Pfeile verwenden denselben bestehenden Raumparameter.
- **SIZE/TONE:** Bisherige Maus-/Mausradbedienung bleibt erhalten; Raumwechsel setzen beide unverändert auf 50 % zurück.

Klappzustände und Scrollposition bleiben während der Lebensdauer des geöffneten Editors erhalten. Nach erneutem Öffnen starten alle vier Kategorien geöffnet und der aktive Raum wird angezeigt. Diese Ansichtseinstellungen werden nicht im Audio-Session-State gespeichert. Es gibt keinen Suchfilter und keine zusätzliche Tastatur-Navigation.

## Kleines linkes Display

Die bisherige Raumdarstellung ist links in den alten Preset-Anzeigebereich umgezogen. Alle 19 vorhandenen Zeichenroutinen bleiben unverändert und werden mit einem einzigen proportionalen Transform verkleinert. Der vollständige Geometriebereich einschließlich Cockpitkanten, Treppen und Höhlenkonturen liegt innerhalb der Glasfläche. Darüber steht der aktive Raumname. Die vorhandene wet-gesteuerte CRT-Wellenform bleibt mit gleicher Phase/Form animiert, nun ebenfalls proportional verkleinert unter der Raumgrafik.

Die ursprüngliche Rasterbeschriftung über dem linken Display, Rahmen, Nieten und Frontplatte bleiben unangetastet. Der Browser liegt ausschließlich innerhalb der großen CRT-Glasfläche. Referenzkoordinaten im 1672×941-Layout: Browser x=542/y=322/w=435/h=284; kleine Glasfläche x=145/y=290/w=233/h=155. Unterstützt sind weiterhin 1003×565 bis 1672×941, Standard 1504×846, proportional und mit den bisherigen Größen-/Skin-Präferenzen.

## Technische Abgrenzung

Ausgangspunkt ist Phase-3A-Nachweiscommit `c3cbe6519db626da734edc60b7daa6a478a060e6`. `Tools/check_phase3a1_scope.py` prüft SHA-256 für RoomEngine, RoomCatalog, OutdoorPlan, PluginProcessor (.h/.cpp), RetroMotion, GuiTelemetry, GuiPreferences und beide Original-PNGs gegen diesen Stand. Für Text wird ausschließlich Git-CRLF/LF kanonisiert; PNGs werden ohne jede Konvertierung geprüft. Damit bleiben DSP/19 Abstimmungen, semantische Raumindizes, normierter Host-Transport, Session-Schema, Hersteller/IDs, Wet-Signalführung, Glimmfunktionen, Telemetrie und Präferenzen bytegleich.

Der Browser ist eine reine GUI-Komponente. Klicks werden an die vorhandene `selectRoom`-Funktion weitergereicht; keine zweite Preset-Liste im Prozessor, keine neue Nummerierung, keine neuen Audioparameter. Ein GUI-seitiger Parameter-Listener setzt ausschließlich ein lockfreies atomares Benachrichtigungsflag. Die bestehende Editor-Timer-Schleife übernimmt Sichtbarmachen und Neuzeichnen auf dem Message-Thread. Listener-Abmeldung beim Schließen, keine zusätzlichen Timer oder Hintergrundberechnungen. Das jetzt statische mittlere Display wird nicht unnötig für die kleine Wellenform neu gezeichnet.

Die Kompatibilitätsgrenzen von Phase 3A bleiben unverändert: Alte diskrete Raumanker bleiben erhalten; die bereits dokumentierte Bedeutung früherer interpolierter Zwischenwerte wird in 3A.1 nicht erneut verändert. Phase 3B bleibt vollständig unverändert.

## Prüfplan und Nachweise

Lokal: Syntaxprüfung der tatsächlichen Editor-/GUI-Testquellen gegen die festgelegten JUCE-8.0.6-Header mit GCC; vier bestehende Release-Tests bestanden; Phase-3A-Dateischutz bestanden. Eine zusätzliche lokale Linux-GUI-Laufzeitabnahme wurde nicht ausgeführt. Maßgeblich sind die echten GUI-Laufzeitprüfungen unter Windows in Actions; Linux-CI prüft weiterhin den DSP mit Sanitizern.

Die erweiterte Windows-GUI-Prüfung bedient die tatsächlichen Komponenten mit JUCE-Mausereignissen: Direktwahl aller 19 Räume bei drei Größen, bestehender Makro-Reset, erneuter Klick auf aktive Zeile, Wheel/Scroll-Grenzen/Schieber, Kategorien ohne Audio-State-Änderung, Host-Auswahl und Same-Room-Recall, Erhalt manuellen Scrollens, beide Knob-Wheels und alternative Pfeile. Die Bildprüfung vergleicht weiterhin 19 unterschiedliche Geometrien je Skin, jetzt im kleinen Display ohne Raumname. Bisherige Audio-/State-/Identitäts-/Lifecycle- und Beleuchtungsprüfungen bleiben aktiv.

Actions erstellt erneut 38 echte native Windows-Screenshots beider Skins, den vollständigen Windows-VST3-Ordner im Installations-ZIP und technische Mess-/Testprotokolle. Keine HTML-Simulation, keine neuen Frontplatten, kein Release, kein Merge nach main. Abschlussergebnisse und Download-Links folgen unten.


## Abgeschlossene Abnahme

Geprüfter Branch-Code: `deb6ba331ef1e9904a2b219ded4b42e4c0b07889`. Actions baut den virtuellen PR-Prüfcommit `129c007b550fb3482edd5c45a53f19d06febb22d`; beide haben denselben Tree `a973fd4930a9889472456ff6df0443a1e821bdaa`. Kein Merge des Entwicklungsbranches in main. Ein nachfolgender Nachweiscommit ändert nur diesen Bericht. Die parallel eingegangene README-Ergänzung des Auftraggebers (`01409fa`) ist vollständig erhalten.

| Prüfung | Ergebnis |
| --- | --- |
| Windows Release, JUCE 8.0.6 / CMake 3.31.6 / VS 2022 x64 | 6/6 Tests bestanden, 21,15 s |
| Linux ASan/UBSan einschließlich Leak-Prüfung | 4/4 Tests bestanden, 55,78 s |
| Lokaler Release-DSP-Vorlauf | 4/4 bestanden, 3,20 s; GCC-Syntaxprüfung von Editor und GUI-Tests gegen die tatsächlichen JUCE-8.0.6-Header bestanden |
| Neue Browser-Tests | Alle 19 Direktwahlen bei 1003×565, 1504×846 und 1672×941; Hervorhebung/semantischer Index/Makro-Reset und vollständige Sichtbarkeit korrekt |
| Navigation | Kategorien ohne Audio-State-Änderung, Mausrad und beide Grenzen, Schieberziehen, Scroll-Erhalt, Wiederholklick, alternative Pfeile bestanden |
| Host/Session | Automationsauswahl und Wiederherstellen desselben Raums öffnen die Kategorie und zeigen den aktiven Eintrag; Knob-Wheels weiterhin funktionsfähig |
| Originalgrafiken und Phase-3A-Dateien | Alle zehn Schutz-Hashes bestanden, keine DSP-/Zuordnungs-/Identitäts-/Präferenz-/Animationsquellenänderung |
| Raumdarstellungen | 19 unterschiedliche Bildsignaturen pro Skin im verkleinerten Grafikbereich, ohne Raumüberschrift; alle 19 Grafiken visuell auf erhaltene Konturen geprüft |
| Erhaltene Beleuchtung | Alle acht Zustände aus Episode I/IV, beiden großen Größen und Stille/Audio bestanden; Restglut und statische übrige Leuchtfelder erhalten |
| Audio-Gegenprobe | Sämtliche 38 neutralen Float32-Stereo-IR-WAV-Dateien des neuen Windows-Laufs sind bytegleich zu den heruntergeladenen Phase-3A-IRs aus Lauf 43865790113 |
| Lieferung | Alle 38 nativen Desktop-PNGs 1504×846, ZIP-/PNG-Prüfungen nach Download bestanden; beide Installations-ZIP-Ebenen CRC-fehlerfrei; komplettes Bundle, x64-PE und alle drei Strm/SpLb-CIDs verifiziert |

Die bestehenden Zeichenfunktionen `drawRoomGeometry`, `drawExtendedRoom`, `drawLighting`, `drawRotarySlider`, `drawEpisodeSwitch` sowie `setEpisodeIV` wurden zusätzlich mit Phase 3A verglichen und sind unverändert. Auch der Algorithmusblock der wet-gesteuerten Wellenform ist unverändert; nur Zeichenort/Skalierung und zugehöriger Repaint-Bereich wurden getauscht.

## Downloads und Ansicht

- [Windows-VST3-Testbuild](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43896594181/artifacts/14239954666): Actions-ZIP entpacken, darin `SpatialLab-VST3-Windows.zip` vollständig entpacken. Bei geschlossener DAW den ganzen Ordner `SpatialLab.vst3` nach `C:\Program Files\Common Files\VST3\` kopieren. Keine einzelne Moduldatei herauslösen.
- [Alle 38 echten Screenshots](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43896594181/artifacts/14242141140): `Episode-I/Room-0..18/Windows-Editor-Screenshot.png` und `Episode-IV/...`. Raum 0 zeigt den Listenanfang mit offenen Innenräumen/Fahrzeugen; Raum 18 den unteren Bereich mit offenen Spezialräumen und Sci-Fi. Damit sind beide Skins und beide Listenabschnitte vorhanden. Keine HTML- oder nachgezeichnete GUI-Simulation.
- [Vollständige Build-Provenienz und technische Protokolle](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43896594181/artifacts/14240923974): Modul, Toolchain, Tests, Beleuchtungsbericht und Messdaten. Der Artefaktname trägt den virtuellen PR-Prüfcommit.
- [Erneuter unveränderter IR-Export](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43896594181/artifacts/14241143538): nur bestehende Messpipeline, keine neue Demo oder Akustikänderung.

SHA-256 des inneren Installations-ZIP: `94ae19128e439441b3c985e53da119f9e36f90eebc3a9e5ee911dca6159660ec`.

SHA-256 des Windows-x64-Moduls: `ceb1bf9f85c0ff0b17c9b3f5dc61f11572d1d11a7483360ca4c8871306c59044`.

Die Downloads erfordern angemeldeten Repository-Zugriff und unterliegen der bestehenden Actions-Aufbewahrung. Native Screenshots und Audio-Testmedien werden nicht dauerhaft eingecheckt.

## Verbleibende Grenzen

- Kein Suchfeld und keine ergänzende Tastatur-Navigation. Die vorgesehenen Wege sind Mausrad, Klick, Scrollschieber und vorhandene Pfeile.
- Kategorien/Scrollposition werden nicht über das Schließen des Editors hinweg gespeichert. Der aktive Raum bleibt Teil des unveränderten Audio-State und wird beim Öffnen sichtbar gemacht.
- Die GUI-Verkleinerung reduziert bewusst den Detailgrad der dekorativen Zeichnungen. Alle Konturen und insbesondere die drei Cockpitformen bleiben unterscheidbar; der Preset-Browser hat Vorrang.
- Ein kurzer Bediencheck im tatsächlichen Fender-Studio-Pro-Host bleibt sinnvoll. Die automatisierte Bedienprüfung verwendet echte JUCE-Komponenten mit synthetischen Mausereignissen; sie ist kein vollständiger DAW-/Hardware-Maustest. Die bestehenden Phase-3A-Automations-/Downgrade-Grenzen bleiben unverändert.

Draft-PR #2 bleibt offen. Kein Release, kein Merge in main und keine Änderungen an Phase 3B.
