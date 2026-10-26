#!/usr/bin/env python3
"""Regenerate athlete patch frames; AI pictograms are composed by AthletePerkArt.qml."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1] / "source/UI/Quick/icons/athlete"
COLORS = {
    "training": '#3e72cb', "reserve": '#29826b', "flight": '#7964c8',
    "small": '#9c5e2a', "steady": '#227785', "volatile": '#ad527d',
}

for key, color in COLORS.items():
    for tier in range(3):
        rim = '#f1cf87' if tier == 2 else '#d6e3e8' if tier == 1 else '#8b9fa9'
        svg = f'''<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 96 96">
<defs><linearGradient id="cloth" x2=".8" y2="1"><stop stop-color="{color}"/><stop offset="1" stop-color="#172735"/></linearGradient></defs>
<path d="M17 6h62l8 10v49L48 92 9 65V16Z" fill="#20313b"/>
<path d="M19 10h58l6 9v44L48 87 13 63V19Z" fill="url(#cloth)"/>
<path d="M22 14h52l5 8v39L48 82 17 61V22Z" fill="none" stroke="{rim}" stroke-width="2" stroke-dasharray="2 2"/>
'''
        if tier == 2:
            svg += '<path d="m38 77 10 6 10-6m-20 5 10 6 10-6" fill="none" stroke="#f1cf87" stroke-width="3" stroke-linejoin="round"/>'
        svg += '</svg>\n'
        (ROOT / f'patch-{key}-{tier}.svg').write_text(svg, encoding='utf-8')
