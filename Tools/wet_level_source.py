"""One documented, fixed-level original speech reference for all v1.3/v1.4 A/Bs."""
import argparse,hashlib,json,pathlib,subprocess,sys
import numpy as np
p=argparse.ArgumentParser();p.add_argument('folder',type=pathlib.Path);a=p.parse_args()
subprocess.run([sys.executable,str(pathlib.Path(__file__).with_name('issue6_source.py')),str(a.folder)],check=True)
original=a.folder/'speech.f32';x=np.fromfile(original,dtype='<f4')
# Fixed fixture input trim, common to every room and both versions. Never derived
# from rendered peaks; no normalization, limiter, or per-preset compensation.
reference=(x*np.float32(10**(-6/20))).astype('<f4');(a.folder/'speech-48000.f32').write_bytes(reference.tobytes())
subprocess.run(['ffmpeg','-hide_banner','-loglevel','error','-y','-f','f32le','-ar','48000','-ac','1','-i',str(a.folder/'speech-48000.f32'),'-ar','44100','-f','f32le',str(a.folder/'speech-44100.f32')],check=True,timeout=60)
meta={'original':json.loads((a.folder/'source.json').read_text()),'fixture_input_trim_db':-6,'send_db':-6,'fx_db':0,'size':.5,'tone':.5,'normalization':'none; one fixed -6 dB input trim before all A/Bs, resampling only','channels':'mono speech identically routed to both inputs of stereo FX; wet stereo retained','sources':{}}
for rate in [44100,48000]:
 path=a.folder/f'speech-{rate}.f32';y=np.fromfile(path,dtype='<f4');meta['sources'][str(rate)]={'sha256':hashlib.sha256(path.read_bytes()).hexdigest(),'peak_dbfs':float(20*np.log10(np.abs(y).max())),'rms_dbfs':float(10*np.log10(np.mean(y.astype(float)**2))),'samples':len(y)}
(a.folder/'reference.json').write_text(json.dumps(meta,indent=2)+'\n')
