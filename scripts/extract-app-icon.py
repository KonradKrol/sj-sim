#!/usr/bin/env python3
"""Reuse the largest PNG already embedded in the Windows icon, without resampling."""
from pathlib import Path
import struct
import sys

source = Path(sys.argv[1]).read_bytes()
if len(source) < 6 or source[:4] != b'\0\0\1\0':
    sys.exit('Invalid application ICO')
count = struct.unpack_from('<H', source, 4)[0]
frames = []
for i in range(count):
    if 6 + 16 * (i + 1) > len(source):
        sys.exit('Truncated ICO directory')
    w, h, _, _, _, _, size, offset = struct.unpack_from('<BBBBHHII', source, 6 + 16 * i)
    w, h = w or 256, h or 256
    data = source[offset:offset + size]
    if w == h and len(data) == size and data.startswith(b'\x89PNG\r\n\x1a\n'):
        frames.append((w, data))
if not frames:
    sys.exit('Application ICO contains no square PNG icon')
Path(sys.argv[2]).write_bytes(max(frames, key=lambda item: item[0])[1])
