"""Prüft die freigegebenen Original-PNGs unverändert, ohne Bildbearbeitung."""
from pathlib import Path
import hashlib
import struct

root = Path(__file__).resolve().parents[1]
for name, checksum in {
    "SpatialLab_Episode_I.png": "b91940feaa4e112db83308a4c62d612022220569a950b4c974c4fb4af6366570",
    "SpatialLab_Episode_IV.png": "0a37f19784c6ee8b696e9a3aa865dceaed91774ee5fc5fcc42b941d43433307a",
}.items():
    data = (root / "assets" / name).read_bytes()
    assert data[:8] == b"\x89PNG\r\n\x1a\n" and data[12:16] == b"IHDR", name
    assert struct.unpack_from(">II", data, 16) == (3344, 1882), name
    assert hashlib.sha256(data).hexdigest() == checksum, f"{name}: Original verändert"
    print(f"OK {name}: 3344x1882, Original-SHA-256 unverändert")
