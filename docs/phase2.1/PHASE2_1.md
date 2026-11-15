# SPATIAL LAB Phase 2.1

## Hersteller und Installation

Aktuelle Identität: **Stromschlag Audio**, Hersteller-Code `Strm` (0x5374726D), Plugin-Code `SpLb` (0x53704C62), Bundle-ID `com.stromschlagaudio.spatiallab`. Damit entspricht die angeforderte Herstelleridentität der gemeinsamen Konfiguration von RADIOPHONE, LOCATIONS, DREAMCATCHER und DOPPELGANGER; diese Repositories werden nicht verändert. Der vorherige Code `StPr` und die alte Bundle-ID werden nicht als kompatible Plugin-Identität beibehalten. Ein möglicherweise noch vorhandener alter kosmetischer Episode-Wert darf weiterhin übernommen werden.

JUCE leitet die VST3-CIDs aus Hersteller-/Plugin-Code ab. Erwartete neue CIDs:

| Klasse | Neue CID |
| --- | --- |
| Audio Module Class | `ABCDEF019182FAEB5374726D53704C62` |
| Component Controller Class | `ABCDEF011234ABCD5374726D53704C62` |
| Plugin Compatibility Class | `ABCDEF01C0DEF00D5374726D53704C62` |

Der Namenswechsel allein hatte die alten CIDs unverändert gelassen. Dass deren Hersteller-Code die Studio-Pro-Sortierung verursacht hat, ist weiterhin eine Hypothese. Der neue Build prüft den tatsächlichen Factory-/Klassenhersteller und die neuen CIDs in der vom Modul erzeugten `moduleinfo.json` sowie die generierten Compilerdefinitionen und Bundle-ID. Studio-Pro-Cacheverhalten wird dadurch nicht vorweggenommen.

### Alte Testversion sauber ersetzen (Windows)

1. Studio Pro vollständig beenden. Benötigte Testprojekte sowie den bisherigen `SpatialLab.vst3`-Ordner außerhalb aller Scan-Verzeichnisse sichern. Die neue Identität wird als neues Plugin behandelt; vorhandene Testinstanzen gegebenenfalls manuell neu einsetzen.
2. Den **kompletten** alten `SpatialLab.vst3`-Ordner entfernen, nicht nur die darin liegende DLL. Übliche Pfade: `C:\Program Files\Common Files\VST3` und `%LOCALAPPDATA%\Programs\Common\VST3`; zusätzlich alle in Studio Pro tatsächlich konfigurierten eigenen Scan-Pfade und gegebenenfalls `C:\Program Files (x86)\Common Files\VST3` prüfen. Eine Suche nach `SpatialLab.vst3` in diesen Pfaden deckt Mehrfachinstallationen auf (Bundle und enthaltene gleichnamige DLL nicht als zwei Installationen zählen).
3. Aus dem neuen Installationsartefakt das innere `SpatialLab-VST3-Windows.zip` entpacken. Genau den vollständigen Ordner `SpatialLab.vst3` in **einen** Scan-Pfad kopieren, vorzugsweise `C:\Program Files\Common Files\VST3\SpatialLab.vst3`. Keine zusätzliche Verschachtelung und keine lose DLL installieren.
4. Studio Pro starten und über dessen Plugin-Verwaltung einen vollständigen erneuten Scan der VST3-Verzeichnisse auslösen. Bei weiterhin altem Eintrag zunächst Pfade, doppelte Bundles und das tatsächlich kopierte Build-Datum prüfen; Studio Pro anschließend erneut starten. Ein verborgenes altes Plugin bei Bedarf gezielt aus der Plugin-Liste entfernen beziehungsweise gezielt neu scannen, sofern die installierte Hostversion dies anbietet.
5. Als Hersteller muss **Stromschlag Audio** erscheinen. Wenn der Host trotz nachweislich korrektem neuen Bundle die alte Sortierung behält, zuerst Hostversion, Scan-Protokoll und betroffene Pfade sichern und gezielt untersuchen. Keine pauschale Löschung globaler Plugin-Caches; diese betreffen auch andere Plugins.

## Beleuchtung

Bisher lagen viele Überlagerungen auf einem bereits hell leuchtenden Raster und hatten nur wenige Prozent wirksame Deckkraft; die Änderung innerhalb weniger Sekunden war entsprechend klein. Nun wird die vorhandene Helligkeit innerhalb der Gläser/Linsen sowohl gedämpft als auch verstärkt. Vier Röhren erhalten unterschiedliche langsame Verläufe, warme innere Glühstrukturen und im verwitterten Skin gelegentliche weich gerundete kurze Dips mit Nachglimmen. Die dritte Röhre bleibt dunkler, lebt aber ebenfalls. Episode I schwankt ruhiger.

Die obere blaue/rote Leuchte und die fünf unteren Rechteckleuchten erhalten eigene Verläufe. Zwei Leuchten können langsam abblenden; die vorhandenen grünen/amberfarbenen/blauen Statuslinsen bleiben auf ihren Glasbereich begrenzt. Aktivitäts-/Signallinsen reagieren unterschiedlich auf den bereits veröffentlichten **Wet-Pegel**, nicht auf die bloße Zahl verarbeiteter Audioblöcke. Das ist bewusst kein zusätzlicher Input-Messpfad: der Audiocode bleibt unangetastet. Betriebslicht und Röhren leben auch bei digitaler Stille. Kein freistehender Halo, keine neue Lampe, keine RGB-Farbwechsel.

Animation ausschließlich im GUI-Thread mit bestehendem 30-Hz-Timer und lokal begrenzten Repaints. Keine neue Audioarbeit, Allokation oder Synchronisation im Prozesscallback. Die CRT-Raumgeometrien und Wellenform bleiben unverändert.

## Episode-Schalter

Position, Größe, Bedienung und Beschriftungen bleiben unverändert. Episode I behält die saubere helle Metalloberfläche. Episode IV verwendet matte dunklere Materialverläufe an Fassung, Innenrand, Hebel und Kappe; deterministische kleine Kratzer, dunkle Vertiefungen und vereinzelte bräunliche Oxidationsspuren sind auf diese Teile begrenzt. Nieten, Plattenrand und Legenden werden nicht übermalt. Keine Änderung der Original-PNGs.

## Fenster

Ohne gespeicherte Größe startet der Editor mit **1504×846** logischen Pixeln. Minimum 1003×565, Maximum 1672×941. Die Frontplatte wird weiterhin proportional dargestellt, bei der minimal abweichenden Standardgröße mit subpixelbreitem Rand statt Verzerrung. Der JUCE-Resize-Constrainer verwendet das ursprüngliche Seitenverhältnis. Dies ist unabhängig von Windows-DPI.

Beim Schließen wird die letzte Größe in derselben separaten benutzerbezogenen GUI-Datei wie die Episode-Wahl gespeichert (`windowWidth`, `windowHeight`). Beim nächsten Öffnen wird sie restauriert. Werte außerhalb der Grenzen beziehungsweise mit falschem Seitenverhältnis fallen auf die Standardgröße zurück. Die Einstellung gilt pro Benutzer; bei mehreren Instanzen gewinnt die zuletzt geschlossene. Kein Audio-Session-State und kein Audioparameter wird ergänzt. Die tatsächlich vom Host zugelassene Fenstergröße hängt weiterhin vom Host und verfügbaren Bildschirm ab; die reale Studio-Pro-Wiederherstellung bleibt manuell zu bestätigen.

## Prüfung

`Tools/check_protected_audio.py` kontrolliert die freigegebenen Phase-2-Quelldateien per SHA-256 (plattformbedingte CRLF werden auf Repository-LF normalisiert). Die Original-PNGs bleiben SHA-256-identisch. Bestehende Audio-, Parameter-, GUI- und Sanitizerprüfungen bleiben aktiv. Ergänzt werden Standard-/gespeicherte/ungültige Fenstergrößen und die neue Identitätsprüfung.

Der Windows-Prüfhost erstellt echte GDI-Screenshots und je 16 zeitlich aufeinanderfolgende Original-PNGs für beide Skins bei Standard- und Maximalgröße, jeweils mit digitaler Stille und synthetischem Audio. `check_lighting_capture.py` misst Änderungen innerhalb jeder Röhre und mehrerer Linsen; Mindeständerung zwischen aufgenommenen Bildern: drei Intensitätsstufen im mittleren absoluten RGB-Unterschied. Zusätzlich entstehen GIFs aus diesen echten Frames; deren Palette ist technisch reduziert, die originalen PNGs bleiben unverändert. Die Prüfung ist ein Sichtbarkeitsnachweis, keine Studio-Pro-Abnahme. Der Prüfhost bricht ab, wenn der Desktop nicht die komplette Editorfläche aufnehmen kann.
