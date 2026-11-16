# SPATIAL LAB Phase 3A.1 – Preset-Browser und Displaytausch

## Bedienung

Das große mittlere CRT enthält jetzt einen direkten Preset-Browser. Bernsteinfarbene Schrift auf schwarzem Hintergrund, ohne zusätzliche Überschrift, mit etwa zwölf sichtbaren Zeilen statt drei. Die Schrift bleibt bei 18 nativen Pixeln für Presets und 16 für Kategorien; die Ansicht skaliert proportional mit dem Fenster.

- **Mausrad über dem Browser:** Liste vertikal verschieben, ungefähr drei Zeilen je konventioneller Mausrad-Rastung. Kleine Trackpad-Deltas scrollen entsprechend fein. Das Rad wählt keinen Raum und verändert weder SIZE noch TONE.
- **Preset anklicken:** Genau die bisherige Umgebung auswählen. Der aktive Eintrag ist bernsteinfarben hinterlegt. Ein weiterer Klick auf den aktiven Eintrag löst keine neue Auswahl aus.
- **Kategorie anklicken:** Auf-/zuklappen. Kategorien sind unabhängig; ein Doppelklick schaltet nicht versehentlich zweimal um. Es gibt INNENRÄUME, FAHRZEUGE, SPEZIALRÄUME und SCI-FI mit unveränderten 9/4/3/3 Presets. Keine leere oder auswählbare Outdoor-Kategorie.
- **Scrollleiste rechts:** Position ablesen, Fassung anklicken oder den Schieber ziehen. Die Liste ist oben/unten begrenzt.
- **Host-Automation/Session-Wiederherstellung:** Passende Kategorie öffnen und den aktiven Eintrag vollständig sichtbar machen. Auch ein Recall desselben Raums öffnet dessen zuvor geschlossene Kategorie. Nur soweit nötig scrollen; bereits sichtbare Einträge verursachen keinen Sprung zum Anfang.
- **Vorhandene Pfeile links:** Weiterhin voriger/nächster Raum, zyklisch über dieselben 19 Umgebungen. Direkte Wahl und Pfeile verwenden denselben bestehenden Raumparameter.
- **SIZE/TONE:** Bisherige Maus-/Mausradbedienung bleibt erhalten; Raumwechsel setzen beide unverändert auf 50 % zurück.

Klappzustände und Scrollposition bleiben während der Lebensdauer des geöffneten Editors erhalten. Nach erneutem Öffnen starten alle vier Kategorien geöffnet und der aktive Raum wird angezeigt. Diese Ansichtseinstellungen werden nicht im Audio-Session-State gespeichert. Es gibt keinen Suchfilter und keine zusätzliche Tastatur-Navigation.

## Kleines linkes Display

Die bisherige Raumdarstellung ist links in den alten Preset-Anzeigebereich umgezogen. Alle 19 vorhandenen Zeichenroutinen bleiben unverändert und werden mit einem einzigen proportionalen Transform verkleinert. Der vollständige Geometriebereich einschließlich Cockpitkanten, Treppen und Höhlenkonturen liegt innerhalb der Glasfläche. Darüber steht der aktive Raumname. Die vorhandene wet-gesteuerte CRT-Wellenform bleibt mit gleicher Phase/Form animiert, nun ebenfalls proportional verkleinert unter der Raumgrafik.

Die ursprüngliche Rasterbeschriftung über dem linken Display, Rahmen, Nieten und Frontplatte bleiben unangetastet. Der Browser liegt ausschließlich innerhalb der großen CRT-Glasfläche. Referenzkoordinaten im 1672×941-Layout: Browser x=542/y=322/w=435/h=284; kleine Glasfläche x=145/y=290/w=233/h=155. Unterstützt sind weiterhin 1003×565 bis 1672×941, Standard 1504×846, proportional und mit den bisherigen Größen-/Skin-Präferenzen.

## Technische Abgrenzung

Ausgangspunkt ist Phase-3A-Nachweiscommit `c3cbe6519db626da734edc60b7daa6a478a060e6`. `Tools/check_phase3a1_scope.py` prüft SHA-256 für RoomEngine, RoomCatalog, OutdoorPlan, PluginProcessor (.h/.cpp), RetroMotion, GuiTelemetry, GuiPreferences und beide Original-PNGs gegen diesen Stand. Damit bleiben DSP/19 Abstimmungen, semantische Raumindizes, normierter Host-Transport, Session-Schema, Hersteller/IDs, Wet-Signalführung, Glimmfunktionen, Telemetrie und Präferenzen bytegleich.

Der Browser ist eine reine GUI-Komponente. Klicks werden an die vorhandene `selectRoom`-Funktion weitergereicht; keine zweite Preset-Liste im Prozessor, keine neue Nummerierung, keine neuen Audioparameter. Ein GUI-seitiger Parameter-Listener setzt ausschließlich ein lockfreies atomares Benachrichtigungsflag. Die bestehende Editor-Timer-Schleife übernimmt Sichtbarmachen und Neuzeichnen auf dem Message-Thread. Listener-Abmeldung beim Schließen, keine zusätzlichen Timer oder Hintergrundberechnungen. Das jetzt statische mittlere Display wird nicht unnötig für die kleine Wellenform neu gezeichnet.

Die Kompatibilitätsgrenzen von Phase 3A bleiben unverändert: Alte diskrete Raumanker bleiben erhalten; die bereits dokumentierte Bedeutung früherer interpolierter Zwischenwerte wird in 3A.1 nicht erneut verändert. Phase 3B bleibt vollständig unverändert.

## Prüfplan und Nachweise

Lokal: Syntaxprüfung der tatsächlichen Editor-/GUI-Testquellen gegen die festgelegten JUCE-8.0.6-Header mit GCC; vier bestehende Release-Tests bestanden; Phase-3A-Dateischutz bestanden. Eine lokale vollständige Linux-GUI-Laufzeit ist in dieser Umgebung mangels X11/Freetype-Entwicklungsumgebung/Display nicht verfügbar. Echte GUI-Laufzeitprüfungen erfolgen unter Windows in Actions.

Die erweiterte Windows-GUI-Prüfung bedient die tatsächlichen Komponenten mit JUCE-Mausereignissen: Direktwahl aller 19 Räume bei drei Größen, bestehender Makro-Reset, erneuter Klick auf aktive Zeile, Wheel/Scroll-Grenzen/Schieber, Kategorien ohne Audio-State-Änderung, Host-Auswahl und Same-Room-Recall, Erhalt manuellen Scrollens, beide Knob-Wheels und alternative Pfeile. Die Bildprüfung vergleicht weiterhin 19 unterschiedliche Geometrien je Skin, jetzt im kleinen Display ohne Raumname. Bisherige Audio-/State-/Identitäts-/Lifecycle- und Beleuchtungsprüfungen bleiben aktiv.

Actions erstellt erneut 38 echte native Windows-Screenshots beider Skins, den vollständigen Windows-VST3-Ordner im Installations-ZIP und technische Mess-/Testprotokolle. Keine HTML-Simulation, keine neuen Frontplatten, kein Release, kein Merge nach main. Abschlussergebnisse und Download-Links werden nach erfolgreicher Prüfung ergänzt.
