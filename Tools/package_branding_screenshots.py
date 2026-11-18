"""Copy real CI Windows screenshots byte-for-byte and generate the release manifest."""
import argparse,hashlib,json,pathlib,struct,shutil
p=argparse.ArgumentParser();p.add_argument('captures',type=pathlib.Path);p.add_argument('repository',type=pathlib.Path);p.add_argument('--run',required=True);a=p.parse_args()
items=[('Retro-Episode-I','retro-i','oberflaeche-retro-neu.png'),('Look-0','retro-iv','oberflaeche-retro.png'),('Look-1','classic-dark','oberflaeche-dunkel.png'),('Look-2','classic-light','oberflaeche-hell.png')]
manifest=None
for folder,look,name in items:
 source=a.captures/folder;data=json.loads((source/'Branding.json').read_text(encoding='utf-8-sig'))
 if manifest is None:manifest={'version':data['public_version'],'internal_version':data['version'],'subtitle':data['subtitle'],'manufacturer_line':data['manufacturer_line'],'source_run':a.run,'capture':'actual JUCE editor, native Windows desktop','screenshots':[]}
 assert data['public_version']==manifest['version'] and data['version']==manifest['internal_version'] and data['subtitle']=='Audio Drama Spatial Environments'
 assert data['manufacturer_line']=='STROMSCHLAG AUDIO · v'+manifest['version']
 raw=(source/'Windows-Editor-Screenshot.png').read_bytes();assert raw[:8]==b'\x89PNG\r\n\x1a\n' and struct.unpack('>II',raw[16:24])==(1504,846)
 relative='docs/'+name;target=a.repository/relative;target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(raw)
 assert target.read_bytes()==raw
 manifest['screenshots'].append({'look':look,'path':relative,'sha256':hashlib.sha256(raw).hexdigest(),'width':1504,'height':846})
(a.repository/'docs/screenshots.json').write_text(json.dumps(manifest,ensure_ascii=False,indent=2)+'\n')
print('Copied four unchanged real Windows captures and wrote docs/screenshots.json for v'+manifest['version'])
