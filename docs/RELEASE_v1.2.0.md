# SPATIAL LAB v1.2 – Überarbeitete Raumakustik für Hörspiele

Alle **26 Umgebungen** sind für Sprachräume neu abgestimmt. SPATIAL LAB bleibt ein **100-%-Wet-Send-Effekt** mit Raumwahl, SIZE und TONE.

## Änderungen

- 19 Innenräume, Fahrzeuge und Cockpits mit geometrisch begründeten frühen Reflexionen, Wegdämpfung, Absorption/Streuung und frequenzabhängig gedämpftem Nachhall.
- Deutlichere kurze Kabinenrückwürfe für Fahrzeuge; dichterer, gedämpfter Reflexionscharakter für Tunnel und Unterführung.
- Wald mit vielen schwachen Streupfaden; Straße und Innenhof mit frühen Fassaden-/Bodenrückwürfen.
- Canyon mit nahen unregelmäßigen Felsreflexionen statt einer regelmäßigen späten Pingpong-Folge. Offene Landschaft bleibt bewusst nahezu trocken.
- Keine pauschale Ausgangsanhebung, kein Loudness-Matching, kein zusätzlicher Audioregler, kein Limiter und kein Oversampling.
- Unverändert: Retro mit Episode I/IV, Classic Dunkel/Hell, Originalfrontplatten, 26 Preset-Keys/-Indizes, Hersteller-/VST3-IDs, Parameter-IDs und Schema 5. Keine zusätzlichen Legacy-/Session-Migrationen.

## Messungen und Prüfung

Die tatsächlichen DSP-Versionen wurden mit derselben unveränderten Original-Sprachquelle bei 48 kHz und SIZE/TONE 50 % verglichen. Bei Send −6 dB / FX 0 dB beträgt die Änderung des Wet-RMS in dieser Referenzmessung rund +11 dB im Kleinwagen und Tunnel, +8 dB in der Unterführung. Das sind unterschiedliche presetbezogene Änderungen, keine globale Verstärkung. Der Canyon beginnt jetzt bei etwa 25 ms statt 310 ms, ohne dadurch pauschal lauter zu werden.

Vollständige Windows-/Linux-Abnahme erfolgreich: acht Windows-CTest-Tests, Linux ASan/UBSan, pluginval Strictness Level 8, 26 Presets bei 44,1/48/96 kHz, Reglerextreme, deterministische Verarbeitung, Presetwechsel, Nullallokationen und bestehende GUI-/Retro-/Identitätsprüfungen. Der zusätzliche Audit enthält 106 echte DSP-WAVs und vollständige Vorher-/Nachher-Messdaten.

[Prüfbericht und Hördateien](https://github.com/StromschlagProductions/SpatialLab-vst/blob/main/docs/issue6/ACCEPTANCE.md) · [Messvergleich](https://github.com/StromschlagProductions/SpatialLab-vst/blob/main/docs/issue6/COMPARISON.md) · [Modellannahmen und Studio-One-Hörtest](https://github.com/StromschlagProductions/SpatialLab-vst/blob/main/docs/issue6/IMPLEMENTATION.md).

## Download und Installation

`SpatialLab-v1.2.0-Windows-x64-VST3.zip` enthält direkt den vollständigen Ordner `SpatialLab.vst3`. Die SHA-256-Prüfsumme steht in `SHA256SUMS.txt`.

1. DAW schließen und ZIP vollständig entpacken.
2. Den gesamten Ordner `SpatialLab.vst3` nach `C:\Program Files\Common Files\VST3\` kopieren.
3. DAW starten und VST3-Suche bei Bedarf erneut ausführen.

Erforderlich sind Windows x64, ein kompatibler 64-Bit-VST3-Host und das Microsoft Visual C++ Redistributable v14 x64. Hauptsampleraten sind 44,1 und 48 kHz; 96 kHz ist zusätzlich auf Stabilität und Verarbeitung geprüft.

## Einordnung

Die Geometrien und Materialwerte sind eigenständige vereinfachte Modelle, keine Rekonstruktionen exakt vermessener Orte oder pegelkalibrierter Fremd-IRs. Raumlautheiten werden nicht angeglichen. Bei Send 0 dB kann die Dry+Wet-Summe zusätzlichen Master-Headroom benötigen. Messwerte und technische Abnahme ersetzen keine allgemeine subjektive Klanggarantie. Die Veröffentlichung ist ausdrücklich freigegeben.
