"""Create one original TTS source, with no level normalization, for both renderers."""
import argparse, hashlib, json, pathlib, subprocess
import numpy as np
TEXT='Behind the heavy door the house was quiet. I put the keys on the table and listened. A glass touched the sink. Then a voice came from the next room. Leave the light on. We will be back before morning.'
p=argparse.ArgumentParser();p.add_argument('folder',type=pathlib.Path);a=p.parse_args();a.folder.mkdir(parents=True,exist_ok=True)
text=a.folder/'speech-text.txt';text.write_text(TEXT)
raw=a.folder/'speech.f32'
subprocess.run(['ffmpeg','-hide_banner','-loglevel','error','-y','-f','lavfi','-i',f'flite=textfile={text.resolve()}:voice=slt','-ar','48000','-ac','1','-f','f32le',str(raw)],check=True,timeout=60)
x=np.fromfile(raw,dtype='<f4');assert len(x)>48000 and np.isfinite(x).all()
meta={'text':TEXT,'origin':'Original text synthesized locally by CMU Flite (slt), not a third-party recording','rate':48000,'channels':1,'format':'float32 little endian','source_peak_dbfs':float(20*np.log10(np.max(np.abs(x)))),'source_rms_dbfs':float(10*np.log10(np.mean(x*x))),'samples':len(x),'sha256':hashlib.sha256(raw.read_bytes()).hexdigest(),'ffmpeg':subprocess.check_output(['ffmpeg','-version'],text=True).splitlines()[0],'normalization':'none; ffmpeg resampling only; same bytes for before and after'}
(a.folder/'source.json').write_text(json.dumps(meta,indent=2));print(json.dumps(meta,indent=2))
