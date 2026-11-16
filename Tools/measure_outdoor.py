"""Temporal measurements of EnvironmentTests float32 WAV exports, no normalization."""
import array, csv, math, struct, sys
from pathlib import Path
writer=csv.writer(sys.stdout)
writer.writerow(['rate','key','first_above_minus80dB_relative_s','last_above_minus80dB_relative_s','energy95_time_s','peak','energy'])
for file in sorted(Path(sys.argv[1]).glob('*.wav')):
    key=file.stem.split('-',1)[1]
    if key not in ['open-land','forest','street','courtyard','canyon','tunnel','underpass']:continue
    data=file.read_bytes()
    assert data[:4]==b'RIFF' and struct.unpack_from('<HH',data,20)==(3,2)
    rate=struct.unpack_from('<I',data,24)[0]
    values=array.array('f');values.frombytes(data[44:])
    if sys.byteorder!='little':values.byteswap()
    assert all(math.isfinite(v) for v in values)
    peak=max(abs(v) for v in values); threshold=peak*1e-4
    first=last=None; energies=[]
    for i in range(0,len(values),2):
        l,r=values[i:i+2]; energies.append(l*l+r*r)
        if max(abs(l),abs(r))>threshold:
            if first is None:first=i//2
            last=i//2
    total=sum(energies); accumulated=0; t95=0
    for i,e in enumerate(energies):
        accumulated+=e
        if accumulated>=total*.95:t95=i/rate;break
    writer.writerow([rate,key,first/rate,last/rate,t95,peak,total])
