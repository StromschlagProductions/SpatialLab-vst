# Continuous Integration (GitHub Actions)

Dieses Dokument beschreibt, welche Prüfungen wann laufen, und warum. Ziel: wenig Actions-Minuten im Entwicklungsalltag, aber unveränderte Qualität der abschließenden Windows-VST3-Prüfung.

## Was läuft wann

| Auslöser | Job | Runner | Kosten | Inhalt |
|---|---|---|---|---|
| Push auf `main` oder `develop/**` (ohne reine Docs-Änderungen) | `dsp-sanitizers` | Ubuntu | 1x | DSP-Tests mit ASan und UBSan |
| Manuell über **Actions → Build and test → Run workflow** | `windows-vst3` | Windows | 2x | Vollständige Abnahme: Build, Tests, GUI-Captures, Impulsantworten, Vergleiche, Benchmark, Installations-ZIP |
| Nur Docs-Änderungen (`docs/**`, `**.md`) | keiner | – | 0 | – |

Der Windows-Lauf wird **nicht** bei Zwischencommits gestartet. Er ist bewusst manuell, weil er etwa doppelt so viele Minuten verbraucht wie der Linux-Lauf.

Die Concurrency-Regel bricht einen älteren Lauf desselben Branches ab, sobald ein neuer Commit gepusht wird.

Es gibt keinen automatischen Release und kein automatisches Merge.

## Entwicklung mit lokalen Builds

Während der Entwicklung bitte zuerst lokal prüfen:

- **DSP-Tests unter Linux** (entspricht dem Job `dsp-sanitizers`, ohne Sanitizer-Flags):
  ```
  cmake -S . -B build -DSPATIALLAB_BUILD_PLUGIN=OFF -DCMAKE_BUILD_TYPE=Release
  cmake --build build --parallel
  ctest --test-dir build --output-on-failure
  ```
- **Windows-Build** nur, wenn Oberfläche oder Plugin-Identität geändert wurden, mit Visual Studio 2022 oder `cmake -G "Visual Studio 17 2022" -A x64`.

## Abnahme (vor jedem Test-Download und vor Freigaben)

1. Branch auf `develop/**` pushen; der Linux-Lauf prüft den Stand automatisch.
2. Unter **Actions → Build and test → Run workflow** den Branch wählen und starten.
3. Erst wenn dieser Windows-Lauf grün ist, das Artefakt `SpatialLab-VST3-Windows-Installieren` verwenden.

## Artefakte

Alle Artefakte werden 14 Tage aufbewahrt, damit der Speicherplatz nicht wächst. Für dauerhafte Downloads ist später ein Release nötig, das aber manuell angelegt wird.
