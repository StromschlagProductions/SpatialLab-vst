"""Phase 2.1 must not modify the approved phase-2 audio code (bcc2c0a)."""
import hashlib
from pathlib import Path
expected = {'Source/RoomEngine.h': '171c1a748426e12e19e40384b98ed9eb1b1c6cc6e688253eef9709fecb3e46ab', 'Source/PluginProcessor.cpp': '82e9811a97146ffe313740eee7520510da1399883dfdd7552b07d88307b008bc', 'Source/PluginProcessor.h': '8f82c4e7a0b2b4f0417127e07d6748171f447998398a12293251943033332d33'}
for path, digest in expected.items():
    assert hashlib.sha256(Path(path).read_bytes().replace(b"\r\n", b"\n")).hexdigest() == digest, path
    print("OK unchanged approved audio:", path)
