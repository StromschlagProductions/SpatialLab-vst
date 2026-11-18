"""Measurements of production-DSP float32 renders; no output level adjustment.
Dependencies: numpy. WAV export uses the standard library (IEEE float format).
"""
import argparse, csv, json, math, pathlib, struct
import numpy as np
RATE=48000

def db(power): return float(10*np.log10(max(float(power),1e-30)))
def wave(path,x):
    x=np.asarray(x,dtype='<f4');ch=1 if x.ndim==1 else x.shape[1];data=x.tobytes()
    path.write_bytes(b'RIFF'+struct.pack('<I',36+len(data))+b'WAVEfmt '+struct.pack('<IHHIIHH',16,3,ch,RATE,RATE*ch*4,ch*4,32)+b'data'+struct.pack('<I',len(data))+data)
def measure(folder,source=None,wavs=False):
    rows=[];edcs={};arrivals={}
    for file in sorted(folder.glob('*-ir-mono.f32')):
        key=file.name.removesuffix('-ir-mono.f32');x=np.fromfile(file,dtype='<f4').reshape(-1,2).astype(float)
        assert np.isfinite(x).all() and len(x)==RATE*16
        e=np.mean(x*x,axis=1);total=e.sum();tail=np.cumsum(e[::-1])[::-1];edb=10*np.log10(np.maximum(tail/max(total,1e-30),1e-30));t=np.arange(len(x))/RATE
        valid=(edb<=-5)&(edb>=-25);idx=np.flatnonzero(valid)
        t20=r2=None
        if len(idx)>2:
            # Decimate the regression to 1 ms; report fit quality, not a universal RT60.
            ii=idx[::48];fit=np.polyfit(t[ii],edb[ii],1);pred=np.polyval(fit,t[ii]);den=np.sum((edb[ii]-edb[ii].mean())**2)
            r2=float(1-np.sum((edb[ii]-pred)**2)/max(den,1e-30))
            if fit[0]<0 and r2>=.9 and t[idx[-1]]-t[idx[0]]>=.02:t20=float(-60/fit[0])
        peak=float(np.abs(x).max());nz=np.flatnonzero(np.max(np.abs(x),axis=1)>peak*1e-4)
        corr=float(np.sum(x[:,0]*x[:,1])/max(np.sqrt(np.sum(x[:,0]**2)*np.sum(x[:,1]**2)),1e-30))
        mid=x.mean(axis=1);left=np.fromfile(folder/(key+'-ir-left.f32'),dtype='<f4').reshape(-1,2).astype(float)
        row=dict(key=key,ir_peak_dbfs=db(peak*peak),ir_energy_db=db(total),onset_ms=float(nz[0]/48),edc_t20_rt60_s=t20,edc_t20_fit_r2=r2,ir_lr_correlation=corr,mono_loss_db=db(np.sum(mid*mid)/total),left_input_energy_db=db(np.mean(left*left,axis=1).sum()),tail_last_second_db=db(e[-RATE:].sum()/total),c50_wet_db=db(e[:2400].sum()/max(e[2400:].sum(),1e-30)))
        for lo,hi in [(30,120),(120,500),(500,2000),(2000,8000)]:
            # Parseval energy over two-sided FFT with the corresponding positive/negative band.
            f=np.fft.rfftfreq(len(x),1/RATE);h=np.fft.rfft(x,axis=0);mask=(f>=lo)&(f<hi)
            row[f'band_{lo}_{hi}_db']=db(2*np.mean(np.abs(h[mask])**2,axis=1).sum()/len(x))
        if source is not None:
            wet=np.fromfile(folder/(key+'-speech-send-6.f32'),dtype='<f4').reshape(-1,2).astype(float);n=len(source)
            assert len(wet)==n+RATE*12 and np.isfinite(wet).all()
            mix=wet.copy();mix[:n]+=source[:,None]
            row.update(source_rms_dbfs=db(np.mean(source*source)),wet_rms_source_window_dbfs=db(np.mean(wet[:n]**2)),wet_rms_full_window_dbfs=db(np.mean(wet**2)),wet_to_source_db=db(np.mean(wet[:n]**2)/np.mean(source*source)),wet_peak_dbfs=db(np.max(np.abs(wet))**2),dry_plus_send6_peak_dbfs=db(np.max(np.abs(mix))**2),speech_mono_loss_db=db(np.mean(wet.mean(axis=1)**2)/np.mean(wet**2)))
            if wavs:wave(folder/(key+'-wet-send-6.wav'),wet);wave(folder/(key+'-dry-plus-send-6.wav'),mix)
        rows.append(row);edcs[key]=[[float(t[i]),float(edb[i])] for i in range(0,len(x),480)]
        # Local maxima, strongest first, at most one per 1-ms neighbourhood.
        magnitude=np.max(np.abs(x),axis=1);candidates=np.flatnonzero((magnitude[1:-1]>magnitude[:-2])&(magnitude[1:-1]>=magnitude[2:]))+1
        chosen=[]
        for i in candidates[np.argsort(magnitude[candidates])[::-1]]:
            if magnitude[i]<peak*.01 or len(chosen)>=24:break
            if all(abs(i-j)>48 for j in chosen):chosen.append(int(i))
        arrivals[key]=[dict(ms=i/48,peak_dbfs=db(magnitude[i]**2)) for i in sorted(chosen)]
        if wavs:wave(folder/(key+'-impulse.wav'),x)
    assert len(rows)==26
    with (folder/'metrics.csv').open('w') as f:w=csv.DictWriter(f,fieldnames=rows[0]);w.writeheader();w.writerows(rows)
    (folder/'edc.json').write_text(json.dumps(edcs));(folder/'arrivals.json').write_text(json.dumps(arrivals,indent=2))
    return rows
if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('folder',type=pathlib.Path);p.add_argument('--source',type=pathlib.Path);p.add_argument('--wav',action='store_true');a=p.parse_args()
    source=np.fromfile(a.source,dtype='<f4').astype(float) if a.source else None
    rows=measure(a.folder,source,a.wav)
    for r in rows:print(r['key'],round(r['ir_energy_db'],2),round(r['onset_ms'],2),r.get('wet_to_source_db'))
