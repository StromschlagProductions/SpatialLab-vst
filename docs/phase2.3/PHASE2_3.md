# Phase 2.3 – Offline-Hörvergleich und zwei Glimmstellen

Der Offline-Prüfhost lädt **die tatsächlich gebaute Windows-VST3** über JUCE AudioPluginFormatManager, untersucht deren Plugin-Factory und erzeugt je Raum eine frische Plugin-Instanz. Er verwendet keine nachgebaute Reverb-Engine. Rendern erfolgt ohne Editor, Stereo in/out, 256 Samples pro Block, SIZE/TONE 0,5. Der Plugin-DSP, Prozessor, Presetwerte und Parameter bleiben gegenüber Phase 2.2 unverändert und bestehen weiterhin die bestehenden Hash-Prüfungen.

## Quellen und Pegel

Sprache: eigens verfasster englischer Text, lokale Windows-Systemstimme über System.Speech, 48 kHz/16 Bit/Mono. Stimme, Kultur, Text und Aufnahmepegel Volume=60/100 stehen im Quellmanifest. Der Aufnahmepegel ist eine vorab dokumentierte Quellentscheidung, keine nachträgliche Lautheitsanpassung. Dry wird bytegleich aus der erzeugten WAV kopiert; für das Stereo-Plugin wird es unverändert auf beide Eingänge gelegt.

Bass: eigenes deterministisches Signal mit 45/80/160 Hz, Amplituden 0,16/0,08/0,04, identisch links/rechts, 44,1 kHz, neun Sekunden, 50 ms Ein-/100 ms Ausblendung. Keine fremden Musik- oder Hörspielaufnahmen.

Wet solo wird bereits mit `10^(-6/20)` Send verarbeitet. Dry + Wet addiert die unveränderte Originalspur und genau diese Wet-Antwort; FX-Gain 1. Kein Dry-Anteil im Plugin, keine automatische Normalisierung, kein Limiter, keine Raum-Lautheitsangleichung. Export als browserübliche PCM16-WAV, drei Sekunden Hallausklang. Bei Übersteuerung bricht die Erzeugung ab, statt heimlich Pegel zu senken. Das unveränderte Sprach-Dry ist kürzer als die Exporte mit zusätzlicher Hallpause.

Die Paketprüfung kontrolliert SHA-256 der Originalquelle, WAV-Formate, nicht stummen Wet-Output, Pegelgrenzen und die arithmetische Dry/Wet-Summe mit einer Toleranz von vier PCM16-LSB (Quantisierung beim Export). Peaks, RMS, alle Dateihashes und die SHA-256 der tatsächlich geladenen VST3-DLL stehen im Manifest. Die Bass-Tonmessung benutzt den stationären Abschnitt 1–8 Sekunden und umfasst den −6-dB-Send, ohne Mono-Zusammenfassung. Die bestehenden breitbandigen Diagnosen liegen zusätzlich bei. Ein Testbericht ist keine subjektive klangliche Freigabe.

## Öffnen und Wiederverwenden

Das Actions-Artefakt enthält `SpatialLab-Hoervergleich.zip`. Dieses vollständig entpacken und `index.html` doppelklicken. Audio, Screenshots und Bericht liegen lokal daneben; keine VST3-Installation, kein Webserver, kein Internet zum Abspielen. Die Seite verwendet ausschließlich relative Dateipfade und eingebauten HTML/JavaScript-Code, keinen Fetch-Zugriff, CDN oder Web-Audio-Hall. Beim Vergleich wird dieselbe Abspielposition übernommen; es spielt höchstens eine Quelle gleichzeitig. Browser-/Systemlautstärke gilt gemeinsam.

Die Quelldateien für Host, Quellerzeugung und Paketgenerator sind unter `Tools/ListeningRender.cpp` und `Tools/listening-demo/` versioniert. Große WAVs, GIFs und fertige Pakete werden ausschließlich als Actions-Artefakte gespeichert. Eine eigene rechtefreie Aufnahme kann statt der Systemsprachquelle im Prüfablauf verwendet werden, sofern sie das dokumentierte WAV-Format besitzt; den Quellnachweis entsprechend aktualisieren.

Die Online-Verteilung erfolgt zunächst ausschließlich als authentifizierter Download aus dem privaten GitHub-Repository. Eine Website wurde nicht veröffentlicht. GitHub Pages ist für diese Freigabe keine automatisch private Hosting-Lösung; eine öffentliche Aktivierung erfolgt nicht ohne Zustimmung.

## Lichtänderung

Genau zwei zusätzliche reine GUI-Helligkeitsverläufe: die vorhandenen orangefarbenen Öffnungen oben mittig hinter dem Gitter und die orangefarbene Linse rechts bei ACOUSTIC ENGINE ROOM SIMULATION. Einmalig berechnete Masken wählen nur vorhandene warme Leuchtpixel innerhalb der Öffnungen beziehungsweise Linse. Metall, Gitter, dunkle Linsendrähte und Beschriftungen werden ausgespart. Es gibt keine hinzugefügten Leuchtkreise und keine neue Lichtquelle.

Zwei unabhängige langsame überlagerte Verläufe ohne Audiosignal, Restglut bleibt erhalten. Rendering über den vorhandenen GUI-Timer; keine zusätzlichen Timer und keine Arbeit im Audiothread. Die vier Röhren, ihre Glühfunktion und die CRT-Wellenform bleiben unverändert. Alle anderen LEDs/Tasten bleiben wie in Phase 2.2 statisch. Die Windows-Aufnahmeprüfung fordert sichtbare Unterschiede der beiden neuen Glimmstellen und pixelgleiche statische Leuchtfelder in beiden Skins und Größen.
