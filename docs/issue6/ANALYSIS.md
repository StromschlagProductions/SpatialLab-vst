# Issue #6 – Analyse vor der Neukalibrierung

Ausgangspunkt: `1792e18`, Version 1.1.2. Branch `fix/issue-6-acoustic-calibration`. Zum Zeitpunkt dieser historischen Analyse waren Merge und Release nicht freigegeben; die spätere Veröffentlichungsfreigabe ist in [ACCEPTANCE.md](ACCEPTANCE.md) dokumentiert. Die folgenden Messungen wurden **vor** Änderungen am Produktions-DSP erstellt. Der unveränderte Ausgangs-DSP einschließlich Katalog liegt unter `Tests/Reference/Issue6/`; `BASELINE.json` nennt Herkunft und SHA-256 der ursprünglichen Dateien.

## Messkonzept und Reproduktion

Der ursprüngliche Offline-Ausgangslauf wurde abschließend mit derselben x64-Denormal-Einstellung wie JUCE reproduziert; dies betrifft ausschließlich unhörbare numerische Restwerte. `Tools/AcousticAudit.cpp` ruft den tatsächlich verwendeten `Engine` auf, ohne Ersatzhall. 26 Presets, 48 kHz, SIZE/TONE jeweils 0,5, 100 % Wet. Je Preset: Einheitsimpuls mit identischer Anregung beider Eingänge (wie Mono→Stereo im Prozessor), linksseitiger Impuls und identische Mono-Sprache. IR-Fenster 16 s. Sprache: originale, lokal durch FFmpeg/CMU Flite (`slt`) synthetisierte Erzählung; keine fremde Aufnahme. Quelle, Softwareversion, unveränderte Quelldatei-Prüfsumme und Pegel: [source.json](source.json). Kein Normalisieren, Limiting, automatisches Gain-Matching oder Oversampling.

Die Quelle dauert 11,22 s; RMS −15,61 dBFS, Sample-Peak −3,98 dBFS. Der Renderer multipliziert den Plugin-Eingang für den Sprachtest ausschließlich mit `10^(-6/20)`; FX-Rückweg 0 dB. Auswertung:

- RMS/Leq: ungewichtete digitale mittlere Energie über 0 bis Quellenende, gemittelt über beide Ausgänge. Zusätzlich eigenes RMS-Fenster einschließlich 12 s angehängter Stille. Kein physikalischer dB-SPL-Leq, kein LUFS. Differenzen nur bei gleichem Fenster vergleichen.
- IR-Energie: Summe der quadrierten Samples, über beide Kanäle gemittelt; dB bezogen auf die Energie des Einheitsimpulses. **Kein Sprach-RMS und kein künstlich gleich lang normalisiertes IR-RMS.**
- EDC: rückwärts integrierte Energie, relativ zur Gesamtenergie. T20-Schätzung durch Regression zwischen −5/−25 dB, auf −60 dB extrapoliert. Nur bei R² ≥ 0,9 und mindestens 20 ms Fitspanne als Wert ausgegeben. Ein leerer Wert bedeutet ungeeignete/stufige Kurve, nicht fehlenden Hall. Nicht mit einem gemessenen Raum-RT60 gleichsetzen.
- Frühe Ankunft: erstes Sample über −80 dB relativ zum jeweiligen IR-Peak. Stärkste lokale Reflexionsmaxima und vollständige EDC werden als JSON exportiert.
- Stereo: L/R-Korrelation ohne Zeitversatz; Monoverlust = Energie von `(L+R)/2` relativ zur mittleren Stereo-Kanalenergie. Linksseitige Anregung wird separat gemessen. FFT-Bandenergien sind zusätzliche Diagnostik. −300 dB ist der logarithmische Rechenboden bei Nullenergie, kein physikalischer Messdynamikbereich.

Alle Ergebnisse für alle 26 Presets: [before_metrics.csv](before_metrics.csv). Große IR-/Sprachdateien gehören in Build-/Actions-Artefakte, nicht in Git.

```sh
cmake -S . -B build-audit -DSPATIALLAB_BUILD_PLUGIN=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build-audit --parallel 2
python Tools/issue6_source.py build-audit/source
build-audit/SpatialLabAcousticBaseline build-audit/before build-audit/source/speech.f32
python Tools/acoustic_audit.py build-audit/before --source build-audit/source/speech.f32 --wav
```

Die Quelle wird einmal erzeugt und für beide Renderer bytegleich verwendet. Flite-/FFmpeg-Versionen können sich zwischen Rechnern unterscheiden; für einen identischen Vergleich immer dieselbe Quelldatei aus einem Audit-Artefakt verwenden.

## Befund

| Preset | Wet/Quelle bei Send −6 dB | Erste IR-Ankunft | IR-Energie |
|---|---:|---:|---:|
| Wohnzimmer | −10,86 dB | 5,29 ms | −3,00 dB |
| Kleinwagen | −16,57 dB | 0,71 ms | siehe CSV |
| Offene Landschaft | −44,39 dB | 190 ms | siehe CSV |
| Wald | −26,47 dB | 23 ms | −27,99 dB |
| Canyon | −19,52 dB | 310 ms | −19,44 dB |
| Tunnel | −20,38 dB | 27 ms | −19,05 dB |
| Unterführung | −20,53 dB | 7 ms | −17,64 dB |

Der vorhandene Outdoor-Zweig besitzt lediglich 2–8 gefilterte Einzelreflexionen mit schwachem Feedback. Es gibt keinen dichten späten Reflexionsanteil für Tunnel/Unterführung. Beim Canyon liegen vier stark seitlich alternierende Taps bei 310/570/890/1210 ms: im Modell fehlen nahe Felsflächen, während die Zeitfolge verzögerungsartig wirkt. Die offene Landschaft hat ausschließlich entfernte extrem leise Reflexionen. Die Indoor-Erstreflexionen enthalten positive und negative Vorzeichen; ihre Zeiten sind nicht an eine dokumentierte Quell-/Empfängergeometrie gebunden. Das ist keine nachgewiesene universelle Fehlkalibrierung, aber eine schlechte Grundlage für nachvollziehbare Sprachraum-Presets.

## Konsequenz für den Entwurf

Keine pauschale Verstärkung. Für jeden Raum werden neutrale Modellgeometrie, Quelle/Empfänger und Oberflächenannahmen dokumentiert. Erstreflexionszeiten aus `Δt=(L_ref−L_direct)/343`; amplitudenbezogene Wegdämpfung und Absorption/Streuung statt willkürlicher Polaritätswechsel. Die bestehende stabile Indoor-FDN-Struktur bleibt Grundlage der späten Energie. Kleine Fahrzeuge erhalten kurze wahrnehmbare Kabinenrückwürfe; Tunnel/Unterführung brauchen zusätzlich dichte, frequenzabhängig gedämpfte Reflexionen. Der Canyon erhält unregelmäßige nahe Rückwürfe und nur wenige geometrisch begründete spätere Ereignisse, keine wechselnde Pingpong-Folge. Die offene Landschaft bleibt bewusst nahezu trocken.

Der tatsächliche Pegel und die Stabilität werden danach erneut mit denselben Quelldaten gemessen. Die Presets müssen **nicht** gleich laut werden. SIZE/TONE, Parameter-IDs, Raumreihenfolge/-Keys, Plugin-Identität und Schema 5 bleiben bestehen; die Raumakustik bestehender Sessions ändert sich beabsichtigt. Kein versteckter Legacy-Modus.
