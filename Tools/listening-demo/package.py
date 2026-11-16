"""Assemble a file:// compatible demonstration from actual VST3 renders and CI evidence."""
import csv, hashlib, html, json, math, shutil, struct, sys, wave
from pathlib import Path
import numpy as np
root=Path(sys.argv[1]); build=Path(sys.argv[2]); bundle=Path(sys.argv[3])
root.mkdir(exist_ok=True,parents=True)
source=json.loads((build/'listening-source/source.json').read_text(encoding='utf-8-sig'))
(root/'source.json').write_text(json.dumps(source,ensure_ascii=False,indent=2),encoding='utf8')
assert (root/'audio/speech-dry.wav').read_bytes()==(build/'listening-source/speech-source.wav').read_bytes(), 'Dry was altered'
def read(p):
    with wave.open(str(p),'rb') as w:
        assert w.getsampwidth()==2 and w.getnchannels() in (1,2)
        rate=w.getframerate(); channels=w.getnchannels()
        a=np.frombuffer(w.readframes(w.getnframes()),dtype='<i2').astype(float).reshape(-1,channels)/32768
    return rate,a
stats=[]
for kind in ('speech','bass'):
    rate,dry=read(root/f'audio/{kind}-dry.wav')
    assert rate==(48000 if kind=='speech' else 44100)
    if dry.shape[1]==1: dry=np.repeat(dry,2,axis=1)
    for room in range(3):
        r,wet=read(root/f'audio/{kind}-room-{room}-wet.wav'); r2,mix=read(root/f'audio/{kind}-room-{room}-mix.wav')
        assert r==r2==rate and wet.shape==mix.shape
        padded=np.zeros_like(wet);padded[:len(dry)]=dry
        error=float(np.max(np.abs(mix-padded-wet)))
        assert error<4/32768, 'Mix differs from dry + wet, or hidden gain'
        assert np.max(np.abs(mix))<.9999 and np.sum(wet*wet)>1e-7
        assert np.max(np.abs(wet[0]))<1/32768
        entry=dict(source=kind,room=room,rate=rate,peakWet=float(np.max(np.abs(wet))),peakMix=float(np.max(np.abs(mix))),rmsWet=float(np.sqrt(np.mean(wet*wet))),mixErrorLSB=error*32768)
        if kind=='bass':
            # Middle steady interval, same generated tones, stereo power (no mono sum).
            x=wet[rate:rate*8]; original=dry[rate:rate*8]
            entry['bassWetTransferDB']={str(f):float(10*np.log10((np.abs(np.sum(x*np.exp(-2j*np.pi*f*np.arange(len(x))/rate)[:,None],axis=0))**2).mean()/(np.abs(np.sum(original*np.exp(-2j*np.pi*f*np.arange(len(x))/rate)[:,None],axis=0))**2).mean())) for f in (45,80,160)}
        stats.append(entry)
for episode in ('I','IV'):
    shutil.copyfile(build/f'gui-preview/Episode-{episode}/1504x846/Silent/Windows-Editor-Screenshot.png',root/f'Episode-{episode}.png')
    shutil.copyfile(build/f'gui-preview/Episode-{episode}/1504x846/Silent/Actual-Windows-Sequence.gif',root/f'Episode-{episode}-animation.gif')
for name in ('room-diagnostics.csv','dsp-benchmark.csv','ctest-results.txt','render-metrics.csv','toolchain.txt'):
    shutil.copyfile(build/name,root/name)
metadata=dict(commit=sys.argv[4],run=sys.argv[5],source=source,sendDB=-6,sendLinear=10**(-6/20),fxDB=0,size=.5,tone=.5,processing='Actual built Windows VST3 loaded by JUCE host; stereo in/out; no GUI; 256-sample blocks; 3 seconds tail; no normalization/limiter',moduleSHA256=hashlib.sha256((bundle/'Contents/x86_64-win/SpatialLab.vst3').read_bytes()).hexdigest(),measurements=stats)
metadata['files']={str(p.relative_to(root)):hashlib.sha256(p.read_bytes()).hexdigest() for p in root.rglob('*') if p.is_file()}
(root/'manifest.json').write_text(json.dumps(metadata,ensure_ascii=False,indent=2),encoding='utf8')
rooms=('Wohnzimmer','Küche','Badezimmer')
sections=[]
for kind,title in [('speech','Sprache · 48 kHz'),('bass','Bass · 44,1 kHz')]:
    rows=f'<button data-file="audio/{kind}-dry.wav">Original Dry</button> <a href="audio/{kind}-dry.wav" download>WAV</a>'
    for i,room in enumerate(rooms):
        rows+=f'<p><b>{room}</b> '
        for mode,label in [('wet','Wet solo'),('mix','Dry + Wet')]:
            file=f'audio/{kind}-room-{i}-{mode}.wav'
            rows+=f'<button data-file="{file}">{label}</button> <a href="{file}" download>WAV</a> '
        rows+='</p>'
    sections.append(f'<section><h2>{title}</h2>{rows}<p class="playing">Noch keine Aufnahme gewählt</p><audio controls preload="none"></audio></section>')
report=f'''<h2>Technischer Bericht</h2><p>Alle Hörbeispiele wurden durch die tatsächliche gebaute VST3 gerendert; im Browser läuft kein Hallalgorithmus. SIZE/TONE jeweils 50 %. Raumwechsel vor dem Rendern; je Raum frische Instanz. Stereo-Ein-/Ausgang, Sprache dual mono. Sprache 48 kHz; Bass 44,1 kHz. Wet solo enthält bereits den Send-Faktor 10^(−6/20) = 0,501187. Dry + Wet = unverändertes Dry + genau dieses Wet, FX-Faktor 1. Die Lautstärke wird nicht angeglichen. WAV-Export PCM 16 Bit, drei Sekunden Ausklang; Originalsprache bytegleich kopiert. Browser-/Systemlautstärke gilt gemeinsam.</p><p>Sprache: neuer eigener Text, lokal mit Windows System.Speech erzeugt. Stimme {html.escape(source['voice'])}, {html.escape(source['culture'])}, Lautstärke 60/100, Sprechtempo 0. Dieser Quellpegel wird vor der Aufnahme gewählt, keine nachträgliche Normalisierung. Text: „{html.escape(source['text'])}“</p><p>Bass: eigene deterministische Sinusmischung 45 Hz × 0,16 + 80 Hz × 0,08 + 160 Hz × 0,04; identisch links/rechts, 9 Sekunden, 50 ms Einblendung/100 ms Ausblendung. Keine fremden Musik-/Hörspielaufnahmen.</p><p>Automatisch geprüft: Original-Dry unverändert, Mix-Summe innerhalb vier 16-Bit-LSB, keine Übersteuerung, endlicher nicht stummer Wet-Output, kein direkter Wet-Anteil an Sample null. Die bestehenden DSP-, Prozessor-, GUI- und Sanitizertests laufen zusätzlich. Diese Tests ersetzen keine subjektive Klangfreigabe.</p><p>Der unveränderte DSP hat keinen Hochpass. Verzögerte Reflexionen können einzelne Bassfrequenzen abschwächen. Ein Wet-Insert enthält keinen direkten Originalbass. Der Send-Vergleich bewahrt die Originalspur; Wet und Dry können dennoch interferieren. Die folgenden Tonmessungen zeigen die konkrete Übertragung der drei erzeugten Bassfrequenzen, keine allgemeine Aussage über jeden Tiefbassklang.</p>'''
report+='<table><tr><th>Raum</th><th>45 Hz</th><th>80 Hz</th><th>160 Hz</th></tr>'
for e in stats:
    if e['source']=='bass':report+='<tr><td>'+rooms[e['room']]+'</td>'+''.join(f'<td>{e["bassWetTransferDB"][str(f)]:.2f} dB</td>' for f in (45,80,160))+'</tr>'
report+='</table><p>Werte Wet relativ zu Dry inklusive −6 dB Send, über den stationären Abschnitt 1–8 Sekunden, Stereoenergie. <a href="manifest.json">Messwerte, Peaks, Quellen und SHA-256</a> · <a href="ctest-results.txt">Windows-Testprotokoll</a> · <a href="room-diagnostics.csv">Tieftondiagnose</a> · <a href="dsp-benchmark.csv">DSP-CPU-Messung</a></p>'
report+=f'<p>Code {html.escape(sys.argv[4])} · <a href="https://github.com/StromschlagProductions/SpatialLab-vst/actions/runs/{html.escape(sys.argv[5])}">Build und Linux-Sanitizer</a>. Diese Aufnahme entstand im Windows-Prüfhost, nicht in einer DAW.</p>'
page='''<!doctype html><html lang="de"><meta charset="utf-8"><meta name="viewport" content="width=device-width"><title>SPATIAL LAB · Hörvergleich</title><style>body{font:17px system-ui;background:#171510;color:#f2e7d1;max-width:1100px;margin:auto;padding:24px;line-height:1.6}a{color:#ffbd62}button{font:inherit;background:#efb34a;color:#201706;padding:8px 14px;border:0;border-radius:5px;margin:4px;cursor:pointer}button[aria-pressed=true]{outline:3px solid white}section{border:1px solid #776344;padding:18px;margin:24px 0}audio{width:100%}img{max-width:100%;height:auto}summary{cursor:pointer}td,th{padding:8px;text-align:left}p.playing{color:#ffc971}</style><h1>SPATIAL LAB · Hörvergleich</h1><p>ZIP vollständig entpacken und diese <b>index.html</b> doppelklicken. Keine VST3-Installation, kein Server und keine Internetverbindung zum Abspielen nötig. Kopfhörer oder Lautsprecher verwenden; Lautstärkeunterschiede gehören zum unveränderten Vergleich.</p><details><summary>Echte Oberfläche · Episode I</summary><img src="Episode-I.png" alt="Echter Windows-Screenshot Episode I"><p><a href="Episode-I-animation.gif">Echte Animation</a></p></details><details><summary>Echte Oberfläche · Episode IV</summary><img src="Episode-IV.png" alt="Echter Windows-Screenshot Episode IV"><p><a href="Episode-IV-animation.gif">Echte Animation</a></p></details>'''+''.join(sections)+report+'''<script>document.querySelectorAll('section').forEach(section=>{const audio=section.querySelector('audio');section.querySelectorAll('button').forEach(button=>button.onclick=()=>{document.querySelectorAll('audio').forEach(a=>{if(a!==audio)a.pause()});const time=audio.currentTime||0;audio.pause();section.querySelectorAll('button').forEach(b=>b.setAttribute('aria-pressed',b===button?'true':'false'));section.querySelector('.playing').textContent=button.parentElement.querySelector('b')?.textContent+' · '+button.textContent;audio.src=button.dataset.file;audio.onloadedmetadata=()=>{audio.currentTime=Math.min(time,Math.max(0,audio.duration-.01));audio.play().catch(()=>{})};audio.load()})});</script></html>'''
(root/'index.html').write_text(page,encoding='utf8')
(root/'README.txt').write_text('ZIP vollständig entpacken. index.html mit Chrome, Edge, Firefox oder Safari öffnen. Alle Unterordner beibehalten. Keine VST3-Installation erforderlich. Keine Normalisierung. manifest.json dokumentiert Quellen und Pegel.',encoding='utf8')
print('PASS actual-VST3 audio, dry copy, clipping checks, mix arithmetic, browser bundle:',root)
