#!/usr/bin/env python3
"""Regenerate the hand-authored athlete perk patches, with a consistent pictogram set."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1] / "source/UI/Quick/icons/athlete"
GLYPHS = {
    "training": ('#3e72cb', '<path d="M9 2h6m-3 0v3m6 1 2-2"/><circle cx="12" cy="14" r="8"/><path d="m12 9 1.5 3 3.3.5-2.4 2.3.6 3.2-3-1.5-3 1.5.6-3.2-2.4-2.3 3.3-.5Z"/>'),
    "reserve": ('#29826b', '<path d="M9 2h6m-3 0v3m6 1 2-2"/><circle cx="12" cy="14" r="8"/><path d="M9.5 10v8m5-8v8"/>'),
    "flight": ('#7964c8', '<circle cx="18" cy="5" r="2"/><path d="m16 8-6 4-6-1m10-1-4-4-4 2m4 4 6 5m-6-5-5 3M2 19l19-3M4 22l18-2"/>'),
    "small": ('#9c5e2a', '<path d="M3 3v4c0 7 5 10 13 10h5M3 7v14m0 0h18M7 11v10m5-6v6m5-4v4"/>'),
    "steady": ('#227785', '<path d="M3 20h18M5 16V7m7 9V7m7 9V7M3 7h4m3 0h4m3 0h4"/>'),
    "volatile": ('#ad527d', '<path d="M3 20h18M4 15l4-9 5 11 6-13m-2 0h4v4"/>'),
}

for key, (color, glyph) in GLYPHS.items():
    for tier in range(3):
        rim = '#f1cf87' if tier == 2 else '#d6e3e8' if tier == 1 else '#8b9fa9'
        svg = f'''<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 96 96">
<defs><linearGradient id="cloth" x2=".8" y2="1"><stop stop-color="{color}"/><stop offset="1" stop-color="#172735"/></linearGradient></defs>
<path d="M17 6h62l8 10v49L48 92 9 65V16Z" fill="#20313b"/>
<path d="M19 10h58l6 9v44L48 87 13 63V19Z" fill="url(#cloth)"/>
<path d="M22 14h52l5 8v39L48 82 17 61V22Z" fill="none" stroke="{rim}" stroke-width="2" stroke-dasharray="2 2"/>
<g transform="translate(27 22) scale(1.75)" fill="none" stroke="#f9fcf7" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round">{glyph}</g>
'''
        if tier == 2:
            svg += '<path d="m34 69 14 8 14-8m-28 6 14 8 14-8" fill="none" stroke="#f1cf87" stroke-width="3" stroke-linejoin="round"/>'
        svg += '</svg>\n'
        (ROOT / f'patch-{key}-{tier}.svg').write_text(svg, encoding='utf-8')
