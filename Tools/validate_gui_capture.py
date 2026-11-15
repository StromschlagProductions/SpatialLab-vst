"""Prüft unveränderte PNG-Dateien aus dem echten JUCE-Prüfhost (keine Bildbearbeitung)."""
import pathlib
import struct
import sys
import zlib

folder = pathlib.Path(sys.argv[1])
dimensions = tuple(map(int,sys.argv[2].split("x"))) if len(sys.argv)>2 else (1504,846)
expected = {"Windows-Editor-Screenshot.png": dimensions,"JUCE-Editor-Render-100.png": dimensions}
expected.update({p.name: dimensions for p in folder.glob("Frame-*.png")})
for name, dimensions in expected.items():
    data = (folder / name).read_bytes()
    assert data[:8] == b"\x89PNG\r\n\x1a\n", name
    offset = 8
    found_end = False
    while offset < len(data):
        length = struct.unpack_from(">I", data, offset)[0]
        kind = data[offset + 4:offset + 8]
        payload = data[offset + 8:offset + 8 + length]
        crc = struct.unpack_from(">I", data, offset + 8 + length)[0]
        assert zlib.crc32(kind + payload) & 0xFFFFFFFF == crc, f"{name}: CRC"
        if kind == b"IHDR":
            assert struct.unpack_from(">II", payload) == dimensions, f"{name}: Abmessungen"
        offset += length + 12
        if kind == b"IEND":
            assert offset == len(data), f"{name}: angehängte Daten/mehrfache Aufnahme"
            found_end = True
            break
    assert found_end, f"{name}: unvollständig"
    print(f"OK {name}: {dimensions[0]}×{dimensions[1]}, {len(data)} Bytes, genau ein PNG")
