# Issue #6 – Technische Abnahme v1.2.0

**Veröffentlichung ausdrücklich freigegeben am 18. November 2026.** Der Auftraggeber bestätigt, dass keine bestehenden Sessions berücksichtigt werden müssen. Es werden keine zusätzlichen Legacy-/Migrationspfade eingebaut. Die Freigabe ist eine Veröffentlichungsfreigabe; sie wird nicht als durchgeführter subjektiver Hörtest ausgegeben.

- Umsetzung: [PR #7](https://github.com/StromschlagProductions/SpatialLab-vst/pull/7) ist nach main übernommen; Merge-Commit `2dda5b8f3bf692057252c968c45e0ee6520b8d80`.
- [Vollständiger erfolgreicher Prüflauf 44252697195](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/44252697195): `windows-vst3`, `dsp-sanitizers` und `acoustic-audit` alle erfolgreich.
- Getesteter PR-Head: `2a18382590d4712150fcc0960de7b6ac43292654`; tatsächlicher CI-Merge-Preview-Build: `b52c54fd1922f10fa32450bbe6bf725988d68a72`. Das ist **kein** Merge nach main.
- Die bereits auf main vorbereitete Versionsnummer 1.2.0 wird für die freigegebene Veröffentlichung verwendet.

## Prüfungen

Windows: **8/8 CTest-Tests** bestanden (Calibration, Boundaries, Environments, DSP, RetroMotion, historische RoomVoicing, Processor und RetroGUI). Prozessortests vergleichen alle 26 echten DSP-Ausgaben mit Mono→Mono, Mono→Stereo und Stereo→Stereo bei 44,1/48/96 kHz. Host-Parameter, State und GUI-Lifecycle bleiben abgedeckt. Pluginval **Strictness Level 8** erfolgreich. [Pluginval-Protokoll](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/44252697195/artifacts/14404259340).

Linux: vollständige DSP-Suite unter ASan/UBSan erfolgreich. Der zusätzliche Release-Audit erzeugt Vorher-/Nachher-Daten mit derselben unveränderten Quelle. Dazu gehören alle 26 Profile, SIZE/TONE-Ecken, endliche/begrenzte Ausgänge, deterministische Blockverarbeitung, keine Allokationen im beobachteten Audiopfad, Presetwechsel und natürliches Ausschwingen der neutralen IRs. Die bestehenden Originalgrafik-, Vertrags-, VST3-Identitäts-, Retro-/Lichtprüfungen und alle 52 Umgebungsaufnahmen sind ebenfalls erfolgreich.

Die x64-FP-Einstellung der Offline-Renderer entspricht JUCE ScopedNoDenormals. Die CI reproduziert trotz anderer FFmpeg-Version denselben originalen Mono-Quellstream wie die lokalen Messungen, SHA-256 `39f0a865bd8c54ced630340b32cc3ca42c3333d3f916a478612ef1e0a36ae0bc`. Die gerundeten Vorher-/Nachher-Ergebnisse stimmen mit [COMPARISON.md](COMPARISON.md) überein. Es wurden keine Dateien lautheitsangepasst.

## Hördateien und Messdaten

[**Audit-Paket herunterladen**](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/44252697195/artifacts/14404799732) – ZIP ca. 389 MB, GitHub-Anmeldung erforderlich, 14 Tage Aufbewahrung.

- `before/`: eingefrorener 1.1.2-Ausgangs-DSP; `after/`: neuer Entwurf.
- Je Ordner 26 `*-impulse.wav`, 26 `*-wet-send-6.wav` und eine `speech-dry.wav`: zusammen **106 WAVs**, 48 kHz, 32-Bit-Float.
- Quelle und Metadaten unter `source/`; CSV, EDC-/Ankunfts-JSON, Modellpfade, beide CPU-Berichte, Vergleichsbericht und Build-Commit im Paket.
- Nach Download geprüft: WAV-Strukturen, native Längen, endliche Samples, unveränderte identische Dry-Quelle, Übereinstimmung aller Wet-Peaks mit den CSV-Daten und ZIP-CRC beim Lesen. Eine Canyon-Datei wurde zusätzlich unabhängig mit FFmpeg/ffprobe als gültiges Stereo-PCM-f32le mit 48 kHz und 23,22 s erkannt.

[Studio-One-Hörtest und Installationshinweise](IMPLEMENTATION.md#hörprüfung-in-studio-one). Die Wet-Dateien enthalten den Send −6 dB bereits. Für den Dateivergleich Dry parallel bei 0 dB abspielen; nicht nochmals einen Send darauf anwenden. Für den Live-Plugin-Test zunächst Send −6 dB / FX 0 dB, dann gezielt Send 0 dB mit Pegelkontrolle verwenden.

## Windows-Testpaket

[**Vollständiges Windows-x64-VST3-ZIP**](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/44252697195/artifacts/14404993499). Das Actions-Artefakt enthält `SpatialLab-VST3-Windows.zip`, darin genau den vollständigen `SpatialLab.vst3`-Ordner einschließlich nativer Moduldatei und `moduleinfo.json`.

ZIP und Herkunft wurden nach Download geprüft. SHA-256 des inneren Installations-ZIP:

`d30129e0fc5dee5fcdcb40c1f7322af7adcaeb9dd4f503706e6b6041a9a46d36`

[Herkunftsnachweis und CTest-Zusammenfassung](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/44252697195/artifacts/14404884191). Der Fingerabdruck sämtlicher Build-Eingaben stimmt mit dem Branch überein; spätere reine Dokumentations-Commits ändern diese Eingaben nicht. GUI, AudioProcessor/Serialisierung, Preset-Katalog, IDs und Originalgrafiken sind per Hash unverändert.

Die Plugin-Identität bleibt gleich; v1.2.0 ersetzt eine vorhandene Installation. Technische Abnahme und ausdrückliche Veröffentlichungsfreigabe liegen vor. Eine subjektive Abnahme in jeder DAW wird nicht behauptet.

## Veröffentlichungsnachweis

[SPATIAL LAB v1.2.0](https://github.com/StromschlagProductions/SpatialLab-vst/releases/tag/v1.2.0) ist tatsächlich veröffentlicht und als neueste stabile Version gekennzeichnet. [Direkter Windows-x64-VST3-Download](https://github.com/StromschlagProductions/SpatialLab-vst/releases/download/v1.2.0/SpatialLab-v1.2.0-Windows-x64-VST3.zip).

Der [erfolgreiche Veröffentlichungsworkflow 44260748916](https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/44260748916) hat Abnahme, main-Zugehörigkeit, Version, unveränderte Build-Eingaben, Plugin-Identität, ZIP-Struktur und Paket-Prüfsumme erneut geprüft. GitHub meldet `draft=false`, `prerelease=false`; `/releases/latest` zeigt auf `v1.2.0`. Der veröffentlichte ZIP-Digest ist identisch zum oben genannten `d30129e0…a46d36`. Das vollständige Installationspaket und `SHA256SUMS.txt` sind reale Release-Assets. Es wurde kein zusätzlicher DSP-Code nach der Abnahme geändert.
