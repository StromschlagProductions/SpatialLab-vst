"""Phase 3A deliberately extends DSP; frozen phase-2 reference stays immutable."""
import hashlib
from pathlib import Path
p=Path("Tests/Reference/RoomEnginePhase2.h")
assert hashlib.sha256(p.read_bytes().replace(b"\r\n",b"\n")).hexdigest()=="88e9deab72dbbc4d15cc62431cb37116b589b4a868f57b523daa2e24ed08a52a"
print("OK frozen phase-2 DSP reference; Environments tests verify original 3 rooms bit-for-bit")
