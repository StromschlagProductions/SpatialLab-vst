"""Validate every fixed preset gain and export the full unnormalized A/B report."""
import argparse,csv,pathlib,math
p=argparse.ArgumentParser();p.add_argument('folder',type=pathlib.Path);a=p.parse_args()
rows=list(csv.DictReader((a.folder/'metrics.csv').open()));lookup={(int(r['rate']),r['key'],r['version']):r for r in rows}
keys=[r['key'] for r in rows if r['rate']=='48000' and r['version']=='v1.4'];assert len(keys)==26 and len(rows)==104
lines=['# SPATIAL LAB v1.4 – vollständiger Pegelvergleich','','Produktions-DSP gegen eingefrorenen v1.3-Produktionscode. Identische ursprüngliche CMU-Flite-Sprachquelle; dokumentierter gemeinsamer Eingangstrim −6 dB zur Headroom-Reserve, Send −6 dB, FX 0 dB, SIZE/TONE 50 %, 100 % Wet. Keine Normalisierung oder Begrenzung. Originalquelle und Referenzpegel: source/reference.json.','','Die WAVs enthalten Dry sowie für alle 26 Presets Wet und Dry+Wet vor/nach der Änderung bei 48 kHz. Beide Versionen derselben Datei immer mit identischer Wiedergabelautstärke vergleichen. 44,1 und 48 kHz werden vollständig gemessen.','','| Hz | Preset | Wet/Dry vorher dB | nachher dB | Differenz dB | Mix Peak dBFS |','|---|---|---:|---:|---:|---:|']
worst=-999;untrimmed=-999
for rate in [44100,48000]:
 for index,key in enumerate(keys):
  before=lookup[rate,key,'v1.3'];after=lookup[rate,key,'v1.4'];expected=18 if index==9 else 6 if index<16 else 0
  delta=float(after['wet_to_dry_db'])-float(before['wet_to_dry_db']);assert abs(delta-expected)<.00002,(rate,key,delta)
  assert abs(float(after['ir_energy_db'])-float(before['ir_energy_db'])-expected)<.00002
  assert abs(float(after['ir_peak_dbfs'])-float(before['ir_peak_dbfs'])-expected)<.00002
  assert abs(float(after['late_after_50ms_fraction'])-float(before['late_after_50ms_fraction']))<1e-7
  assert after['onset_ms']==before['onset_ms']
  peak=float(after['mix_peak_dbfs']);assert peak<0,(key,'Reference mix clips',peak);worst=max(worst,peak);untrimmed=max(untrimmed,peak+6)
  if index>=16:
   for field in before:
    if field!='version':assert before[field]==after[field],(key,field,'Protected preset changed')
   if rate==48000:
    for suffix in ['wet-send-minus6','dry-plus-wet']:assert (a.folder/'v1.3'/f'{key}-{suffix}.wav').read_bytes()==(a.folder/'v1.4'/f'{key}-{suffix}.wav').read_bytes()
  lines.append(f"| {rate} | {key} | {float(before['wet_to_dry_db']):.2f} | {float(after['wet_to_dry_db']):.2f} | {delta:.3f} | {peak:.2f} |")
lines+=['',f'Maximaler Stereo-Mix-Peak der Referenz: {worst:.2f} dBFS. Ohne den dokumentierten gemeinsamen Eingangstrim wäre der maximale Peak {untrimmed:.2f} dBFS. Die lineare Engine begrenzt nicht: laute Quellen können nach Dry+Wet-Summierung zusätzliche DAW-Headroom-Reserve benötigen.','', 'Offene Landschaft: +18 dB insgesamt, also zusätzlich +12 dB gegenüber der allgemeinen Korrektur. Keine zusätzlichen Taps, kein diffuser Nachhall. Wald: +6 dB; bestehender kurzer diffuser Anteil unverändert.','', 'Alle akustischen Form-/Decay-Verhältnisse bleiben unverändert. Fahrzeuge, Kapelle/Kathedrale/Höhle und Cockpits sind einschließlich Stereo-WAVs bitidentisch. Die Messung und die bereitgestellten A/Bs ersetzen keine subjektive Hörfreigabe in Studio One.']
(a.folder/'COMPARISON.md').write_text('\n'.join(lines)+'\n');print('PASS 52 before/after comparisons, exact gains, unchanged onset/tail balance, protected WAVs bit-identical, reference mix unclipped')
print('Worst reference mix peak:',worst,'dBFS; original untrimmed source:',untrimmed,'dBFS')
