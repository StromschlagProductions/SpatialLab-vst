"""Verify visible changes in actual Win32 captures; derive labelled GIFs from those frames."""
from pathlib import Path
import json
import csv
import sys
from PIL import Image, ImageChops, ImageStat

root = Path(sys.argv[1])
report = []
for folder in sorted(root.glob('Episode-*/*/*')):
    frames = [Image.open(p).convert('RGB') for p in sorted(folder.glob('Frame-*.png'))]
    assert len(frames) == 16, f'{folder}: missing actual screen frames'
    width, height = frames[0].size
    scale = min(width/1672, height/941)
    ox, oy = (width-1672*scale)/2, (height-941*scale)/2
    def box(x,y,w,h):
        return tuple(round(v) for v in (ox+x*scale,oy+y*scale,ox+(x+w)*scale,oy+(y+h)*scale))
    regions = {f'tube-{i}':box(x,145,32,65) for i,x in enumerate((1216,1290,1361,1424))}
    regions.update({'power-lens':box(934,190,33,30),'aux-amber':box(845,740,23,30),'aux-blue':box(925,740,23,30)})
    changes = {}
    for name,rect in regions.items():
        crops = [frame.crop(rect) for frame in frames]
        # Compare all observed pairs: independent phases need not peak in the first frame.
        difference = max(sum(ImageStat.Stat(ImageChops.difference(a,b)).mean)/3
                         for i,a in enumerate(crops) for b in crops[i+1:])
        changes[name] = round(difference,3)
        assert difference >= 3.0, f'{folder}/{name}: insufficient visible screen change ({difference:.2f}/255)'
    # Animated presentation only: the original lossless GDI frames remain alongside it.
    times = [float(row['seconds_monotonic']) for row in csv.DictReader((folder/'Capture-Times.csv').open())]
    durations = [max(20,round((b-a)*1000)) for a,b in zip(times,times[1:])] + [333]
    palette = frames[0].quantize(colors=256)
    gif_frames = [frame.quantize(palette=palette) for frame in frames]
    gif_frames[0].save(folder/'Actual-Windows-Sequence.gif',save_all=True,append_images=gif_frames[1:],
                       duration=durations[:len(frames)],loop=0,optimize=False,disposal=2)
    times = [float(row['seconds_monotonic']) for row in csv.DictReader((folder/'Capture-Times.csv').open())]
    tube_frames = [Image.open(p).convert('RGB') for p in sorted(folder.glob('Tubes-*.png'))]
    assert len(tube_frames) == (48 if folder.name == 'Silent' else 16)
    # Separate evidence for every actual upper-right glass tube over the longer silence recording.
    if folder.name == 'Silent':
        for i,x in enumerate((1216,1290,1361,1424)):
            rect = tuple(round(v*scale) for v in (x-1199,145-116,x+32-1199,210-116))
            brightness = [sum(ImageStat.Stat(f.crop(rect)).mean)/3 for f in tube_frames]
            assert max(brightness)-min(brightness) >= 5, f'{folder}/tube-{i}: no visible rise/fall'
            steps = [b-a for a,b in zip(brightness,brightness[1:])]
            assert max(steps)>0.25 and min(steps)<-0.25, f'{folder}/tube-{i}: missing rising or falling phase'
        assert times[-1]-times[0] >= 14, 'Tube evidence must cover several full slow phases'
    tube_palette = tube_frames[0].quantize(colors=256)
    tube_gif = [f.quantize(palette=tube_palette) for f in tube_frames]
    durations = [max(20,round((b-a)*1000)) for a,b in zip(times,times[1:])] + [333]
    tube_gif[0].save(folder/'Actual-Tube-Detail-Sequence.gif',save_all=True,append_images=tube_gif[1:],
                     duration=durations,loop=0,optimize=False,disposal=2)
    report.append({'capture':str(folder.relative_to(root)),'max_pair_mean_absolute_change_255':changes})
    print('OK visible screen animation:',folder,changes)
assert len(report) == 8, 'Expected both skins, both large sizes, silence and audio'
(root/'lighting-visibility.json').write_text(json.dumps(report,indent=2)+'\n')
