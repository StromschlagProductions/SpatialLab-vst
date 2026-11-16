"""Check native Windows knob captures: fixed bevel, moving inner face, both skins."""
from pathlib import Path
import sys
from PIL import Image
root=Path(sys.argv[1])
for skin in ('I','IV'):
    frames=[Image.open(root/f'Episode-{skin}/{value}/Windows-Editor-Screenshot.png').convert('RGB') for value in ('0','0.25','0.5','0.75','1')]
    assert all(f.size==(1672,941) for f in frames)
    for cy in (581,774):
        face_changes=0
        for y in range(cy-56,cy+57):
            for x in range(202,315):
                radius=((x-258)**2+(y-cy)**2)**0.5
                pixels=[f.getpixel((x,y)) for f in frames]
                if 48<radius<55: assert len(set(pixels))==1, f'{skin}/{cy}: scale reflection/bevel moves'
                if radius<42 and len(set(pixels))>1: face_changes+=1
        assert face_changes>100, f'{skin}/{cy}: knob face not rotating'
        print('OK fixed outer bevel and moving inner face',skin,cy,face_changes)
