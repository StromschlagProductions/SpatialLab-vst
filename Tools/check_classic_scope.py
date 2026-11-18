"""v1.3 changes GUI branding only: approved v1.2 DSP/state/catalog/artwork stay immutable."""
from pathlib import Path
import hashlib
root=Path(__file__).resolve().parents[1]
expected={'Source/GuiPreferences.h': '1b210c6a5332c8045661b3de4668ab1eb2c9d530e346f289c3f0afbc2bf95b8e',
 'Source/GuiTelemetry.h': '88739d42d19dcff1d89e7ea10e855035ac38605b04fb5eeb1e6c4df6ee121ace',
 'Source/PluginProcessor.cpp': 'ed48e458e8c4f2a1fe234463b3bfafef9f8842314f654bd3e4b6e8559bde6b19',
 'Source/PluginProcessor.h': '6dac9c11c3179db7925faafb12a285e1106db7eb778ce7993c534bed0ae2e70d',
 'Source/RetroMotion.h': 'df470bc5ae5a7bca21bd2b09dad55879a42c344ff1728f667f415c5474c2218e',
 'Source/RoomCatalog.h': 'c9890713bd0f81b5e80ed9ac1e789708dfa5d85f5cddd97325e4978cb8cb7261',
 'assets/SpatialLab_Episode_I.png': 'b91940feaa4e112db83308a4c62d612022220569a950b4c974c4fb4af6366570',
 'assets/SpatialLab_Episode_IV.png': '0a37f19784c6ee8b696e9a3aa865dceaed91774ee5fc5fcc42b941d43433307a',
 'assets/Stromschlag_Audio_Original_Logo.png': 'b199e6c323d08ba042bbceb3b827e293c5c7a51faf90308c849f15b0b433c9a6'}
expected.update({'Source/PresetBrowser.h': '189e03cba4fc4589db8bfed60e291b46c99a51396d3b42e73b66c0265709ece7', 'Source/ClassicTheme.h': '17243227d5b27ae1c6d1bd5d64188da7d737c79238c470b64195c39286b49a63', 'Source/RoomEngine.h': '0068324cce19e6ed48bee311a2138ef9b1dd027bf6ad8481316d16002f69f726', 'Source/OutdoorEngine.h': 'c130e4835dfdf30e87ed4bb5f940bac69a84be270e27962eee7f12eb1b10060d', 'Source/OutdoorPlan.h': '982667754e1fc56acc919939c1fc699e466464e91dd1ee167657542d3c0bb973', 'Source/AcousticGeometry.h': '2ab914b08bd3ebdb7f99b7cacde9f8cfbd4b65e9b7541390c7819816fbb129fc', 'Source/OutdoorDiffuse.h': '3e2e1fdcfcefb59af2ed3df3d607afdc93f5a34f4fd60e03eea8b67cd1e0698a'})
expected['Source/PluginEditor.h']='efee3664f49476217afffb704b81d72bde4d4ef6e69c1093c622a1de33127049'
for name,digest in expected.items():
    data=(root/name).read_bytes()
    if not name.endswith('.png'):data=data.replace(b"\r\n",b"\n")
    assert hashlib.sha256(data).hexdigest()==digest,name+": approved release bytes changed"
    print("OK unchanged release:",name)

# Retain independent hashes of the pre-audit production engines. Never bless changed
# reference data by recomputing its expected digest from itself.
import json
baseline=root/'Tests/Reference/Issue6'
manifest=json.loads((baseline/'BASELINE.json').read_text())
for name,digest in manifest['original_sha256'].items():
    restored=(baseline/name).read_bytes().replace(b"\r\n",b"\n").replace(b'namespace spatiallab_issue6_baseline',b'namespace spatiallab')
    assert hashlib.sha256(restored).hexdigest()==digest,'Frozen audit baseline changed: '+name
print('OK frozen issue-6 before-DSP and protected public contract')
