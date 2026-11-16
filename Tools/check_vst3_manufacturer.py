"""Verifiziert die vom tatsächlich gebauten VST3-Modul erzeugten Hersteller-/Klassenmetadaten."""
import json
import pathlib
import re
import sys

# JUCE/VST3's moduleinfo writer emits trailing commas; tolerate that SDK syntax.
text = pathlib.Path(sys.argv[1]).read_text(encoding="utf-8-sig")
info = json.loads(re.sub(r",\s*(?=[}\]])", "", text))
assert info["Factory Info"]["Vendor"] == "Stromschlag Audio", "Falscher Factory-Hersteller"
expected = {
    "Audio Module Class": "ABCDEF019182FAEB5374726D53704C62",
    "Component Controller Class": "ABCDEF011234ABCD5374726D53704C62",
    "Plugin Compatibility Class": "ABCDEF01C0DEF00D5374726D53704C62",
}
for category, cid in expected.items():
    matches = [item for item in info["Classes"] if item["Category"] == category]
    assert len(matches) == 1 and matches[0]["CID"] == cid, f"{category}: Neue Strm/SpLb-Identität stimmt nicht"
    assert matches[0]["Vendor"] == "Stromschlag Audio", f"{category}: Hersteller falsch"
    print(f"OK {category}: Stromschlag Audio, Strm/SpLb CID {cid}")

assert "Stromschlag Productions" not in text and "53745072" not in text, "Alte Herstelleridentität im Modul"
print("OK neue Herstelleridentität Strm / SpLb; kein alter Hersteller im Modul")

# Verify JUCE's compiled manufacturer/plugin definitions, not just CMake intent.
if len(sys.argv) > 2:
    project = pathlib.Path(sys.argv[2]).read_text(encoding="utf-8-sig")
    assert re.search(r"JucePlugin_ManufacturerCode=0x5374726[dD]", project), "Built manufacturer code is not Strm"
    assert re.search(r"JucePlugin_PluginCode=0x53704[cC]62", project), "Built plugin code is not SpLb"
    assert "com.stromschlagaudio.spatiallab" in project, "Built bundle ID incorrect"
    print("OK compiled Strm, SpLb and com.stromschlagaudio.spatiallab")
