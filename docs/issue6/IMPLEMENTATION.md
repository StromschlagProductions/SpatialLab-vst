# Issue #6 – Entwurf und Prüfstand

**Unveröffentlichter DSP-Entwurf, Hörfreigabe ausstehend. Kein Merge und kein Release.**

[Analyse vor Änderung](ANALYSIS.md) · [Messliteratur und Grenzen](REFERENCES.md) · [Vorher/Nachher](COMPARISON.md) · [vollständige Modellpfade](model_paths.csv).

## Was sich ändert

Alle **26** bestehenden Umgebungen erhalten eine eigene neutrale akustische Modellierung. Es gibt keine zusätzlichen Presets, Bedienelemente oder automatisierbaren Parameter. Es wird weder ein globaler Ausgangsgain noch ein Limiter, Loudness-Matching oder Oversampling eingeführt.

### 19 Innenräume, Fahrzeuge und Cockpits

`AcousticGeometry.h` beschreibt je Preset Breite, Tiefe, Höhe, Quellhöhe, neutralen Direktabstand, sechs Energie-Absorptionskoeffizienten, Streuanteil und eine Nachhall-Obergrenze. Die Quelle liegt bei `(0.42 W, 0.25 D, z)`, der Empfänger um den Direktabstand entlang der Tiefe versetzt. Zwei Empfangspunkte bei ±9 cm erzeugen separate linke/rechte Wege. Das ist eine Stereo-Verteilung, kein HRTF-/Binauralmodell.

Die sechs Erstreflexionen entstehen über Spiegelquellen an den Begrenzungsflächen. Ihre zusätzlichen Laufzeiten betragen `(L_ref − L_direct) / 343 m/s`; der Direktschall wird **nicht** ausgegeben. Der Druckfaktor jedes Pfades beträgt `(L_direct / L_ref) × sqrt((1−α) × (1−s))`, mit Energie-Absorption `α` und aus dem gerichteten Erstpfad gestreutem Anteil `s`. Zufällige Vorzeichenwechsel wurden entfernt. Eine geglättete Tiefpassfärbung wirkt nun auch auf die frühe Energie und folgt TONE.

Die ursprünglichen unterschiedlichen FDN-Laufzeiten, Diffusionsrotationen und Crossfeeds bleiben als Diffusionsentwurf erhalten; sie sind **keine** exakten Schallwege der Boxgeometrie. Die nominale Nachhallzeit wird mit `0.161 V / Σ(S_i α_i)` abgeschätzt und auf einen konservativen, preset-spezifischen Bereich begrenzt. Sabine ist gerade bei kleinen Kabinen, stark absorbierenden Räumen oder unregelmäßigen Höhlen nur eine Entwurfshilfe. Auch Dachboden, Treppenhaus und Höhle verwenden vereinfachte mittlere Box-Abmessungen. Es werden keine Messgenauigkeit oder physikalisch exakte Moden behauptet. Die späte FDN-Energie ist eine abgestimmte synthetische Ergänzung, keine vollständige Strahlungsenergiebilanz.

Damit werden Kabinen durch kurze Begrenzungsflächen statt durch einen langen Hall hörbar. Schlafzimmer und Dachboden bleiben absorbierend, Küche/Bad härter. Kapelle und Kathedrale behalten eigene lange FDN-Verteilungen; die Höhle unterscheidet sich durch unregelmäßige Laufzeiten, stärkere Streuung und tiefere Frequenzdämpfung. Die drei Cockpits bleiben kreative Klangmodelle mit unterschiedlichen Größen, Dämpfungen und Diffusionsverteilungen.

### Sieben Außenräume

Jeder diskrete Outdoor-Pfad speichert Direktweg, reflektierten Gesamtweg, Absorption, Streuung, Panorama und Frequenzgrenze. Die relative Laufzeit und der Pegel werden aus denselben Weg-/Materialgleichungen abgeleitet. Der neutrale Modellabstand beträgt 2 m, beim Canyon 4 m. Diese Abstände sind **interne Bezugsgeometrien für statische Presets**; es wird keine Distanz- oder Bewegungssimulation und kein neuer Parameter eingeführt. Die lineare Panoramaverteilung wurde durch eine vorab berechnete Verteilung mit konstanter Energie ersetzt; das ist keine pauschale Pegelanhebung.

- **Offene Landschaft:** stark absorbierender/streuender Boden und eine entfernte schwache Begrenzung. Kein diffuser Hallanteil. Ein nahezu trockenes Ergebnis bleibt beabsichtigt.
- **Wald:** 16 unregelmäßige schwache Streupfade, zunehmende Höhendämpfung und ein leiser kurzer diffuser Anteil. Keine zufallsabhängigen Ergebnisse.
- **Straße/Gasse:** Boden, nahe Fassaden, zurückgesetzte Begrenzungen und schwächere Mehrfachwege; kurzer diffuser Rest.
- **Innenhof:** weitere Fassaden-/Mehrfachwege und längerer diffuser Anteil als in der Straße.
- **Canyon:** nahe unregelmäßige Felskonturen ab etwa 25 ms; zwei schwächere entfernte Konturen. Keine alternierende rhythmische Tap-Folge und **kein Tap-Feedback**. Die Gesamtpegel müssen nicht größer als beim alten verzögerungsartigen Canyon werden.
- **Tunnel:** harte frühe Boden-/Decken-/Seitenreflexionen und ein eigenständiges dichtes gedämpftes Feld. Neutrale Modellmaße: 6 m Breite, 4,2 m Höhe, Quelle/Empfänger 1,5 m hoch; laterale Abstände sind asymmetrisch. Axiale Endreflexionen werden nicht als geschlossene Tunnelenden erfunden.
- **Unterführung:** nahe Boden-/Untersichtsreflexionen, unterbrochene Seitenflächen und kürzerer diffuser Rest als im Tunnel.

`OutdoorDiffuse.h` ergänzt ein vorallokiertes Vierlinien-FDN mit orthogonaler Householder-Streuung, geglätteten Schleifengewinnen unter 1 und frequenzabhängiger Dämpfung. Es wird ausschließlich mit bereits reflektierter Energie angeregt. Kein Direktsignal. Alle neuen diskreten Pfade verzichten auf Tap-Feedback; wiederkehrende diffuse Energie entsteht kontrolliert im FDN. Die längeren Pfade sind eigene Entwurfsannahmen und kein universeller Canyon-/Tunnel-Messwert.

## Echtzeit und CPU

Alle Delay-Puffer werden in `prepare` angelegt. Presetwechsel, SIZE/TONE und die eigentliche Verarbeitung allokieren nicht. Koeffizienten werden bei tatsächlichen Zieländerungen berechnet; Panorama-Gewichte bereits bei der Presetauswahl. Die ruhende Outdoor-Verarbeitung liest jeden Stereo-Tap nur einmal; doppelte Read-Heads werden nur während einer Größenüberblendung benötigt. Geschlossene GUI-Fenster, Timer und Animationen bleiben unverändert.

Offline-Renderer und CPU-Benchmark verwenden auf x64 dieselbe Flush-to-zero/Denormals-are-zero-Einstellung wie der JUCE-Prozessor. Das vermeidet irreführende Offline-CPU-Spitzen bei unhörbaren Restwerten. Der zusätzliche dichte Anteil und mehr Wald-Taps benötigen Rechenzeit; Vorher-/Nachher-CSV-Dateien werden deshalb im selben CI-Job nacheinander erzeugt. Die Wandzeitwerte sind hardware-/lastabhängig und keine Echtzeitgarantie. Im lokalen neutralen Vorlauf betrug die mediane geschätzte Belegung eines Kerns etwa 0,21 % vorher und 0,20 % nachher, das Maximum etwa 1,08 % / 1,02 %. Das ist kein genereller Geschwindigkeitsnachweis; der Runner ist nicht lastisoliert. Vollständige Rohdaten: [vorher](cpu_before.csv), [nachher](cpu_after.csv).

## Kompatibilität

GUI, Originalfrontplatten, Raumkatalog einschließlich Reihenfolge/Keys, Hersteller-/VST3-IDs, drei Parameter samt IDs/Normalisierung und Session-Schema 5 bleiben unverändert. Ein Datei-Hash-Schutz prüft diese Verträge. Der Produktions-AudioProcessor und dessen Serialisierung sind unverändert; die bestehenden Prozessortests vergleichen alle 26 Engine-Ausgaben mit den drei tatsächlichen Mono-/Stereo-Buslayouts und wurden auf 96 kHz erweitert.

**Bestehende Sessions bleiben technisch ladbar, klingen mit diesem Entwurf aber absichtlich anders.** Es gibt keinen versteckten Legacy-Modus, kein automatisches Gain-Matching und keine Sound-Migration. Für Rückkehr zum alten Klang die bisherige offizielle Version und Projektkopien aufbewahren. Der Branch übernimmt die auf main bereits vorbereitete Versionsnummer 1.2.0. Dieser Draft ist damit noch keine offizielle Veröffentlichung und aufgrund gleicher Plugin-Identität nicht als unabhängiges zweites Plugin installierbar.

## Tests und Messdateien

Die historische Phase-1/Phase-2-Abstimmungsprüfung und der bitgleiche Vergleich der bisherigen 19 Räume laufen weiter gegen den **eingefrorenen Vorgänger-DSP**. Ihre alten Timing-/Pegel-Sollwerte sind keine verdeckten Sollwerte für die beabsichtigt neuen Presets. `Environments` prüft weiterhin alle aktuellen 26 IRs, natürliche neutrale Tails, unterschiedliche Signaturen und SIZE/TONE-Wirkung. `Calibration` ergänzt alle 26 Presets bei 44,1/48/96 kHz, Reglerextreme, blockunabhängige deterministische Verarbeitung, Stereo-Anregung, Nullallokationen, Presetwechsel und endliche/begrenzte Ausgänge. Die bestehenden Boundary-, DSP-, GUI-/Lifecycle-, Host-/State- und Identitätsprüfungen bleiben erhalten. Pluginval bleibt bei Level 8.

Das Actions-Artefakt `SpatialLab-Issue6-Acoustic-Audit` enthält pro DSP-Version sämtliche Impuls-WAVs, Wet-Sprach-WAVs bei Send −6 dB, einen unveränderten Dry-Export, vollständige Mess-CSV, EDC-/Ankunfts-JSON, Modellpfade, Quellenmetadaten und beide CPU-Berichte. Die WAVs sind unverstärkte 32-Bit-Float-Dateien mit 48 kHz. Die zwölf Sekunden Stille hinter der Quelle lassen den Hall ausklingen. Es wird keine neue HTML-Demo erstellt. Große Medien werden nicht eingecheckt; Aufbewahrung des Audit-Artefakts: 14 Tage.

Lokale Release-DSP-Prüfungen: 6/6 bestanden. JUCE-Quellen/Prozessortests syntaktisch gegen JUCE 8.0.6 geprüft. Der abschließende Windows-/Linux-/pluginval-Lauf und sein Artefakt werden nach der Abnahme unten verlinkt; vorab wird kein Erfolg behauptet.

## Hörprüfung in Studio One

1. Ein neues Testprojekt mit 48 kHz erstellen; die mitgelieferte originale `speech-dry.wav` importieren. Kanal- und Masterfader auf 0 dB; keine Dynamik-/Lautheitsbearbeitung, kein Auto-Gain. Unveränderte Quelle verwenden. Danach denselben Test bei 44,1 kHz durchführen; beim Import die DAW regulär resampeln lassen.
2. SPATIAL LAB auf einem Stereo-FX-Kanal einsetzen, FX-Fader 0 dB. Post-Fader-Send der Mono-Spur zunächst auf **−6 dB**. Plugin-Raum wählen, SIZE/TONE jeweils **50 %** kontrollieren. Der Plugin-Ausgang ist ausschließlich Wet; die trockene Spur bleibt parallel hörbar.
3. Zuerst Wohnzimmer/Küche/Bad vergleichen, dann Kleinwagen/großen PKW/Transporter/LKW, danach Tunnel/Unterführung, Wald, Straße/Innenhof und Canyon. Dieselbe Quelle und dieselben Faderstände beibehalten. Wet solo abhören, danach wieder mit Dry. Auch die FX-Summe mono prüfen. Keine automatische Normalisierung und keine Anpassung auf identische Raumlautheit.
4. Für Vorher/Nachher ohne Installationswechsel Dry plus die jeweilige Datei `before/<key>-wet-send-6.wav` oder `after/<key>-wet-send-6.wav` auf parallelen Spuren bei 0 dB abspielen, exakt ab Projektstart ausrichten. **Hier nicht nochmals senden**: Die Wet-Dateien enthalten den −6-dB-Send bereits. Nur jeweils eine Wet-Version aktiv lassen. Alternativ die alte/neue Plugin-Datei nacheinander in einer Projektkopie testen; identische VST3-IDs erlauben keine zuverlässige parallele Installation beider Builds.
5. Anschließend den Send gezielt auf **0 dB** stellen und die Pegel beobachten. Einige Dry+Wet-Summen können mit dieser Quelle über 0 dBFS steigen; das ist zusätzliche Summenenergie, keine automatische Begrenzung. Für diesen Zusatztest erforderlichen Master-Headroom offen dokumentieren und beide Versionen gleich behandeln. Keine Pegelanhebung als Raumkorrektur verstecken.
6. Auf Sprachverständlichkeit, Kabinenfärbung, Boden-/Wandrückwürfe, Natürlichkeit des Canyon-Echos, Monofaltung, metallisches Klingeln und Presetwechsel achten. Auch SIZE/TONE-Ecken ausprobieren. Die offene Landschaft darf sehr unauffällig bleiben. Subjektive Hörfreigabe und eventuelle weitere Korrekturen sind ausdrücklich offen; Messwerte alleine ersetzen sie nicht.
