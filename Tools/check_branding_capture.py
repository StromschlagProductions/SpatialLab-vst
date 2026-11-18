"""Verify actual preview-host metadata against the CMake project, not fixed GUI text."""
import json,pathlib,re,sys
root=pathlib.Path(__file__).resolve().parents[1]
version=re.search(r'project\(SpatialLab VERSION ([0-9.]+)',(root/'CMakeLists.txt').read_text())[1]
parts=version.split('.');public='.'.join(parts[:2]) if len(parts)==3 and parts[2]=='0' else version
files=list(pathlib.Path(sys.argv[1]).rglob('Branding.json'));assert len(files)>=4,'All three looks and both Retro skins required'
seen=set();episodes=set()
for file in files:
 data=json.loads(file.read_text(encoding='utf-8-sig'));assert data['version']==version and data['public_version']==public
 assert data['subtitle']=='Audio Drama Spatial Environments'
 assert data['manufacturer_line']=='STROMSCHLAG AUDIO · v'+public
 assert (file.parent/'Windows-Editor-Screenshot.png').is_file(),'Real Windows screenshot missing'
 seen.add(data['look'])
 if data['look']==0:episodes.add(data['episode'])
assert seen=={0,1,2} and episodes=={'I','IV'}
print('PASS actual Windows captures: Retro I/IV + Classic Dark/Light; public version',public,'subtitle and manufacturer from CMake')
