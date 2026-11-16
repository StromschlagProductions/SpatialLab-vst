"""Nicht-generative, lokale Retusche der eingebrannten Fantasiegrafik.

Nur Asset-Werkzeug, keine Build-/Plugin-Abhängigkeit.
Benötigt opencv-python-headless==4.11.0.86 und numpy==2.2.6.
Die Originaldateien werden weder überschrieben noch umgezeichnet.
"""
from pathlib import Path
import hashlib
import cv2
import numpy as np

root = Path(__file__).resolve().parents[1]
source_path = root / "assets/SpatialLab_Retro_Frontplatte_Referenz.png"
assert hashlib.sha256(source_path.read_bytes()).hexdigest() == "6ccfd1b74607507b6763dffcb15330c74953d20959fc7a046464ec26649855af"
source = cv2.imread(str(source_path), cv2.IMREAD_COLOR)
height, width = source.shape[:2]
selection = np.zeros((height, width), np.uint8)
selection[96:178, 134:211] = 255  # Fantasie-Wellenzeichen
selection[107:169, 210:355] = 255  # Fantasie-Wortmarke
# Schwarze Druckfarbe und deren Antialiasing; rote Originalmarkierungen ausschließen.
spread = np.ptp(source, axis=2)
ink = (source.max(axis=2) < 125) & (spread < 45) & (selection != 0)
mask = cv2.dilate(ink.astype(np.uint8) * 255, cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (11, 11)))
blue, green, red = (source[:, :, i].astype(float) for i in range(3))
red_paint = (red > 1.6 * green) & (red > 1.6 * blue) & (red > 70)
mask[red_paint] = 0
# Beide originalen Schrauben samt Rand/Schlagschatten unangetastet lassen.
for centre in [(140, 101), (147, 180)]:
    cv2.circle(mask, centre, 16, 0, -1)
# Klassische Navier-Stokes-Retusche aus dem direkten Nachbar-Metall, kein KI-Modell.
restored = cv2.inpaint(source, mask, 5.0, cv2.INPAINT_NS)
x, y, w, h = 128, 92, 234, 90
assert not np.any(mask[:y]) and not np.any(mask[y+h:])
assert not np.any(mask[:, :x]) and not np.any(mask[:, x+w:])
patch = np.dstack((restored[y:y+h, x:x+w], mask[y:y+h, x:x+w]))
patch[patch[:, :, 3] == 0, :3] = 0
output = root / "assets/SpatialLab_LogoRegion_Restored.png"
assert cv2.imwrite(str(output), patch)
# Nur rekonstruierte Druckfarb-Pixel sind deckend; keine rechteckige Abdeckfläche.
assert np.all(patch[0, :, 3] == 0) and np.all(patch[-1, :, 3] == 0)
assert np.all(patch[:, 0, 3] == 0) and np.all(patch[:, -1, 3] == 0)
print(f"{output.name}: {w}×{h}, {np.count_nonzero(mask)} lokal retuschierte Pixel")
