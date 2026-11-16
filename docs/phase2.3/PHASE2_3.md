# Phase 2.3 – Offline-Hörvergleich und zwei Glimmstellen

Der Offline-Prüfhost lädt **die tatsächlich gebaute Windows-VST3** über JUCE AudioPluginFormatManager, untersucht deren Plugin-Factory und erzeugt je Raum eine frische Plugin-Instanz. Er verwendet keine nachgebaute Reverb-Engine. Rendern erfolgt ohne Editor, Stereo in/out, 256 Samples pro Block, SIZE/TONE 0,5. Der Plugin-DSP, Prozessor, Presetwerte und Parameter bleiben gegenüber Phase 2.2 unverändert und bestehen weiterhin die bestehenden Hash-Prüfungen.

## Quellen und Pegel

Sprache: eigens verfasster englischer Text, lokale Windows-Systemstimme über System.Speech, 48 kHz/16 Bit/Mono. Stimme, Kultur, Text und Aufnahmepegel Volume=60/100 stehen im Quellmanifest. Der Aufnahmepegel ist eine vorab dokumentierte Quellentscheidung, keine nachträgliche Lautheitsanpassung. Dry wird bytegleich aus der erzeugten WAV kopiert; für das Stereo-Plugin wird es unverändert auf beide Eingänge gelegt.

Bass: eigenes deterministisches Signal mit 45/80/160 Hz, Amplituden 0,16/0,08/0,04, identisch links/rechts, 44,1 kHz, neun Sekunden, 50 ms Ein-/100 ms Ausblendung. Keine fremden Musik- oder Hörspielaufnahmen.

Wet solo wird bereits mit `10^(-6/20)` Send verarbeitet. Dry + Wet addiert die unveränderte Originalspur und genau diese Wet-Antwort; FX-Gain 1. Kein Dry-Anteil im Plugin, keine automatische Normalisierung, kein Limiter, keine Raum-Lautheitsangleichung. Export als browserübliche PCM16-WAV, drei Sekunden Hallausklang. Bei Übersteuerung bricht die Erzeugung ab, statt heimlich Pegel zu senken. Das unveränderte Sprach-Dry ist kürzer als die Exporte mit zusätzlicher Hallpause.

Die Paketprüfung kontrolliert SHA-256 der Originalquelle, WAV-Formate, nicht stummen Wet-Output, Pegelgrenzen und die arithmetische Dry/Wet-Summe mit einer Toleranz von vier PCM16-LSB (Quantisierung beim Export). Peaks, RMS, SHA-256 der Medien und Protokolle und die SHA-256 der tatsächlich geladenen VST3-DLL stehen im Manifest. Die Bass-Tonmessung benutzt den stationären Abschnitt 1–8 Sekunden und umfasst den −6-dB-Send, ohne Mono-Zusammenfassung. Die bestehenden breitbandigen Diagnosen liegen zusätzlich bei. Ein Testbericht ist keine subjektive klangliche Freigabe.

## Öffnen und Wiederverwenden

Das Actions-Artefakt enthält `SpatialLab-Hoervergleich.zip`. Dieses vollständig entpacken und `index.html` doppelklicken. Audio, Screenshots und Bericht liegen lokal daneben; keine VST3-Installation, kein Webserver, kein Internet zum Abspielen. Die Seite verwendet ausschließlich relative Dateipfade und eingebauten HTML/JavaScript-Code, keinen Fetch-Zugriff, CDN oder Web-Audio-Hall. Beim Vergleich wird dieselbe Abspielposition übernommen; über die Vergleichstasten spielt höchstens eine Quelle gleichzeitig. Browser-/Systemlautstärke gilt gemeinsam.

Die Quelldateien für Host, Quellerzeugung und Paketgenerator sind unter `Tools/ListeningRender.cpp` und `Tools/listening-demo/` versioniert. Große WAVs, GIFs und fertige Pakete werden ausschließlich als Actions-Artefakte gespeichert. Eine eigene rechtefreie Aufnahme kann statt der Systemsprachquelle im Prüfablauf verwendet werden, sofern sie das dokumentierte WAV-Format besitzt; den Quellnachweis entsprechend aktualisieren.

Die Online-Verteilung erfolgt zunächst ausschließlich als authentifizierter Download aus dem privaten GitHub-Repository. Eine Website wurde nicht veröffentlicht. GitHub Pages ist für diese Freigabe keine automatisch private Hosting-Lösung; eine öffentliche Aktivierung erfolgt nicht ohne Zustimmung.

## Lichtänderung

Genau zwei zusätzliche reine GUI-Helligkeitsverläufe: die vorhandenen orangefarbenen Öffnungen oben mittig hinter dem Gitter und die orangefarbene Linse rechts bei ACOUSTIC ENGINE ROOM SIMULATION. Einmalig berechnete Masken wählen nur vorhandene warme Leuchtpixel innerhalb der Öffnungen beziehungsweise Linse. Metall, Gitter, dunkle Linsendrähte und Beschriftungen werden ausgespart. Es gibt keine hinzugefügten Leuchtkreise und keine neue Lichtquelle.

Zwei unabhängige langsame überlagerte Verläufe ohne Audiosignal, Restglut bleibt erhalten. Rendering über den vorhandenen GUI-Timer; keine zusätzlichen Timer und keine Arbeit im Audiothread. Die vier Röhren, ihre Glühfunktion und die CRT-Wellenform bleiben unverändert. Alle anderen LEDs/Tasten bleiben wie in Phase 2.2 statisch. Die Windows-Aufnahmeprüfung fordert sichtbare Unterschiede der beiden neuen Glimmstellen und pixelgleiche statische Leuchtfelder in beiden Skins und Größen.

## Abgeschlossene Abnahme und Download

Geprüfter Code: `9da8ad9852620cfc9559cda780fa5bfe43d0a5c9`. [Windows-Release-Build und Linux-Sanitizer vollständig erfolgreich](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43834864168).

- [Vollständiges Offline-Hörvergleichspaket](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43834864168/artifacts/14211318402) (Actions-Download enthält `SpatialLab-Hoervergleich.zip`). Beide ZIP-Ebenen vollständig entpacken, anschließend `index.html` öffnen; alle Unterordner beibehalten.
- [Windows-VST3-Installationspaket](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43834864168/artifacts/14211732507).
- [Vollständige Build- und Aufnahme-Artefakte](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/43834864168/artifacts/14211932088).
- [Render- und Pegelergebnisse](render-results.json), [Browser-Prüfergebnis](browser-test.json).

Alle fünf Windows-Tests, Linux-ASan/UBSan und Hersteller-/Originaldateiprüfungen bestanden. Die tatsächliche VST3 wurde erfolgreich geladen; jede der drei neutralen Raumantworten wurde für Sprache und Bass erzeugt. Alle Raum-WAVs unterscheiden sich. Dry ist bytegleich kopiert, keine Übersteuerung, maximale Abweichung von `Dry+Wet` zu den separat exportierten Komponenten: **ein PCM16-LSB**, ohne automatische Pegelanpassung. Alle Medienhashes und beide ZIP-CRCs nach Download geprüft.

Die echte Sprache stammt von **Microsoft David Desktop, en-US**, der eigene englische Text steht im Paket. Bass ist selbst erzeugt. Die interne Testseite hat alle 14 Varianten in Chromium abgespielt, beim Umschalten die Position erhalten, normale Lautstärke 1 verwendet, Bilder geladen und keine JavaScript-Fehler gemeldet. Der verwaltete Browser dieser Cloud-Umgebung sperrt `file://` per Administratorrichtlinie; deshalb wurde der Funktionstest über einen ausschließlich lokalen Loopback-Testserver durchgeführt. Das Paket selbst enthält keine Fetch-Aufrufe oder Netzressourcen und verwendet relative HTML-Medienpfade. Die direkte Doppelklick-Abnahme auf einem normalen Zielrechner bleibt deshalb praktisch nachzuprüfen.

Screenshots und kurze echte Windows-Animationen beider Episoden sind im Offline-Paket enthalten. Auch die vollständigen langen Stillesequenzen, beide Größen und Audiosequenzen bestanden die strengen Prüfungen. Die zusätzlichen Glimmstellen verändern sich sichtbar; übrige LEDs/Tasten bleiben pixelgleich. Die Röhrenzeichnung und ihre Glühfunktion sind gegenüber Phase 2.2 erhalten.

Für stabile Desktop-Aufnahmen wird ausschließlich das Prüfhost-Fenster im Vordergrund gehalten; GDI-Pixel werden in Software-Bildspeicher kopiert und vor dem PNG-Lesen freigegeben. Die Aufnahmeprüfung überspringt keine schwarzen Frames. Die lange Sequenz wurde nach dieser Korrektur vollständig bestanden.

Die Seite kann mit `Tools/listening-demo/browser-test.cjs` und installiertem Playwright erneut geprüft werden: Node-Aufruf mit absolutem entpacktem Demo-Ordner; optional `CHROMIUM_PATH` und `DEMO_URL` setzen. Der Test kann zusätzliche Prüfdateien im entpackten Ordner erzeugen, verändert aber keine WAVs oder HTML-Dateien.

Der abschließende Nachweiscommit ergänzt nur Bericht und Browser-Testwerkzeug; geprüfter Plugin-Code und ausgelieferte Medien bleiben identisch. Kein öffentliches Hosting, kein Merge, kein Release.

SHA-256 des heruntergeladenen inneren Demo-ZIPs: `9a677b095336f68703e1a3ab505a2df74245060252c7286470b0632bb4c0d843`. Die Screenshots und Animationen wurden zusätzlich visuell auf Originalmaterial, passgenaue Glimmstellen, freie Gitter/Metallteile und statische übrige Anzeigen geprüft.
