"""Protect the approved Phase 3A audio/identity/automation/artwork during GUI-only 3A.1.
Baseline: c3cbe6519db626da734edc60b7daa6a478a060e6.
"""
from pathlib import Path
import hashlib

ROOT = Path(__file__).resolve().parents[1]
EXPECTED = {'Source/RoomEngine.h': '6cdbd14e3e63d133290d44137eacedb6016f14655d965cfc6834728d510df4c8', 'Source/RoomCatalog.h': 'c29419f17e59dd9c7501f5c89fef7777e256f36dfff1047cf559bff2afca094a', 'Source/OutdoorPlan.h': 'c62d61efd01b56f772de12dda246a430da4e5d5c69ab7dfd260fc1cb87288897', 'Source/PluginProcessor.h': '5ab1b466799b75a4ba14f41a0323869a22feb1fda38043cab775bfcb7f4f349b', 'Source/PluginProcessor.cpp': '008b80d13a89c2e070cf9973e0635d48b13a1347c0e8536507efe11e6423b07f', 'Source/RetroMotion.h': 'df470bc5ae5a7bca21bd2b09dad55879a42c344ff1728f667f415c5474c2218e', 'Source/GuiTelemetry.h': '88739d42d19dcff1d89e7ea10e855035ac38605b04fb5eeb1e6c4df6ee121ace', 'Source/GuiPreferences.h': '1b210c6a5332c8045661b3de4668ab1eb2c9d530e346f289c3f0afbc2bf95b8e', 'assets/SpatialLab_Episode_I.png': 'b91940feaa4e112db83308a4c62d612022220569a950b4c974c4fb4af6366570', 'assets/SpatialLab_Episode_IV.png': '0a37f19784c6ee8b696e9a3aa865dceaed91774ee5fc5fcc42b941d43433307a'}
for name, digest in EXPECTED.items():
    data = (ROOT / name).read_bytes()
    # Git's Windows checkout may use CRLF; compare source as canonical Git LF.
    # Artwork stays strictly binary, without any conversion.
    if not name.endswith(".png"):
        data = data.replace(b"\r\n", b"\n")
    assert hashlib.sha256(data).hexdigest() == digest, name + ": Phase 3A bytes changed"
    print("OK Phase 3A unchanged:", name)
