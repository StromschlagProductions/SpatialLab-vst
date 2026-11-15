"""Prüft unveränderte PNG-Dateien aus dem echten JUCE-Prüfhost (keine Bildbearbeitung)."""
import pathlib
import struct
import sys
import zlib

folder = pathlib.Path(sys.argv[1])
expected = {
    "Windows-Editor-Screenshot.png": (1003, 565),
    "JUCE-Editor-Render-100.png": (1003, 565),
    "JUCE-Editor-Render-125.png": (1254, 706),
    "JUCE-Editor-Render-150.png": (1504, 848),
}
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
