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
    "Audio Module Class": "ABCDEF019182FAEB5374507253704C62",
    "Component Controller Class": "ABCDEF011234ABCD5374507253704C62",
    "Plugin Compatibility Class": "ABCDEF01C0DEF00D5374507253704C62",
}
for category, cid in expected.items():
    matches = [item for item in info["Classes"] if item["Category"] == category]
    assert len(matches) == 1 and matches[0]["CID"] == cid, f"{category}: Plugin-Identität verändert"
    assert matches[0]["Vendor"] == "Stromschlag Audio", f"{category}: Hersteller falsch"
    print(f"OK {category}: Stromschlag Audio, unveränderte CID {cid}")
