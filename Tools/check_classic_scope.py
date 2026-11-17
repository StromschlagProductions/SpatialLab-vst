"""Classic is GUI-only: freeze release DSP, state, identity, presets and original artwork."""
from pathlib import Path
import hashlib
root=Path(__file__).resolve().parents[1]
expected={'Source/GuiPreferences.h': '1b210c6a5332c8045661b3de4668ab1eb2c9d530e346f289c3f0afbc2bf95b8e',
 'Source/GuiTelemetry.h': '88739d42d19dcff1d89e7ea10e855035ac38605b04fb5eeb1e6c4df6ee121ace',
 'Source/OutdoorEngine.h': 'ef1d72b908f7eea6d9225845135ab890aa6312ddc0cb3cccff35f29b799aa7cf',
 'Source/OutdoorPlan.h': '614fa88553457e76c639d309279aa57906f37cb4b6d024e2bf5e47f8da07f111',
 'Source/PluginProcessor.cpp': 'd3ccda0d83e5f7a32d67ba02142ef42567e98f4086a42a068c048465945dd76d',
 'Source/PluginProcessor.h': '6dac9c11c3179db7925faafb12a285e1106db7eb778ce7993c534bed0ae2e70d',
 'Source/RetroMotion.h': 'df470bc5ae5a7bca21bd2b09dad55879a42c344ff1728f667f415c5474c2218e',
 'Source/RoomCatalog.h': 'c9890713bd0f81b5e80ed9ac1e789708dfa5d85f5cddd97325e4978cb8cb7261',
 'Source/RoomEngine.h': '837a81a8605183e76cd8accebff97d304e3964f074758d7f194c2d916e887574',
 'assets/SpatialLab_Episode_I.png': 'b91940feaa4e112db83308a4c62d612022220569a950b4c974c4fb4af6366570',
 'assets/SpatialLab_Episode_IV.png': '0a37f19784c6ee8b696e9a3aa865dceaed91774ee5fc5fcc42b941d43433307a',
 'assets/Stromschlag_Audio_Original_Logo.png': 'b199e6c323d08ba042bbceb3b827e293c5c7a51faf90308c849f15b0b433c9a6'}
for name,digest in expected.items():
    data=(root/name).read_bytes()
    if not name.endswith('.png'):data=data.replace(b"\r\n",b"\n")
    assert hashlib.sha256(data).hexdigest()==digest,name+": approved release bytes changed"
    print("OK unchanged release:",name)
