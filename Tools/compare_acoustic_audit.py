"""Compare identical-source before/after renders, without loudness matching."""
import argparse,csv,json,pathlib
p=argparse.ArgumentParser();p.add_argument('root',type=pathlib.Path);a=p.parse_args()
before={r['key']:r for r in csv.DictReader((a.root/'before/metrics.csv').open())}
after={r['key']:r for r in csv.DictReader((a.root/'after/metrics.csv').open())}
assert len(before)==len(after)==26 and before.keys()==after.keys()
source=json.loads((a.root/'source/source.json').read_text())
lines=['# Reproduzierbarer Vorher-/Nachher-Vergleich','',f"48 kHz, SIZE/TONE 50 %, Send −6 dB, FX 0 dB. Identische unveränderte Mono-Quelle: SHA-256 `{source['sha256']}`. RMS-Fenster: 0 bis Quellenende, mittlere Kanalenergie. Keine Normalisierung.",'','| Umgebung (Key) | Wet/Quelle vorher dB | nachher dB | Änderung dB | Monoverlust vorher dB | nachher dB | Ankunft vorher ms | nachher ms | Wet-Peak nachher dBFS | Dry+Wet-Peak bei Send 0 dB* |','|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|']
for key,r in after.items():
 b=before[key];old=float(b['wet_to_source_db']);new=float(r['wet_to_source_db']);delta=new-old
 lines.append(f"| {key} | {old:.2f} | {new:.2f} | {delta:+.2f} | {float(b['mono_loss_db']):.2f} | {float(r['mono_loss_db']):.2f} | {float(b['onset_ms']):.2f} | {float(r['onset_ms']):.2f} | {float(r['wet_peak_dbfs']):.2f} | {float(r['dry_plus_send0_peak_dbfs']):.2f} |")
 assert float(r['wet_peak_dbfs'])<0 and float(r['dry_plus_send6_peak_dbfs'])<0,'Reference speech clips at normal send'
 assert float(r['tail_last_second_db'])<-60,'Truncated nondecaying neutral tail'
for key,minimum in [('compact-car',6),('tunnel',6),('underpass',5)]:
 assert float(after[key]['wet_to_source_db'])-float(before[key]['wet_to_source_db'])>minimum,'Targeted return remains too weak'
assert float(after['open-land']['wet_to_source_db'])<-35,'Open country became an audible room'
assert float(after['canyon']['onset_ms'])<70,'Canyon still lacks near reflections'
lines+=['','*Send-0-Peaks sind aus dem linearen Wet-Signal durch Rücknahme der bekannten −6-dB-Eingangsskalierung berechnet. Sie sind keine zusätzliche Lautheitsangleichung. Positive Summenpeaks sind möglich: Bei Send 0 dB benötigt die DAW-Summe gegebenenfalls Headroom; der Effekt enthält keinen Limiter.','', 'C50 und EDC beziehen sich auf Wet ohne Direktsignal. Ein stufiger Outdoor-Decay rechtfertigt keine einzelne universelle RT60-Zahl. Die Messungen sind keine Hörfreigabe und kein STI-/Sprachverständlichkeitsnachweis.']
(a.root/'COMPARISON.md').write_text('\n'.join(lines)+'\n')
print('PASS: 26 before/after pairs, finite tails, reference speech headroom, targeted return improvements and near canyon reflections')
