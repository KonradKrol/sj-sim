#!/usr/bin/env python3
"""Check the real AppImage payload and launch its deployed Qt UI with isolated data."""
import json
import os
from pathlib import Path
import stat
import subprocess
import sys
import tempfile

artifact = Path(sys.argv[1]).resolve()
with tempfile.TemporaryDirectory(prefix='sjsim-appimage-check-') as temporary:
    subprocess.run([str(artifact), '--appimage-extract'], cwd=temporary, stdout=subprocess.DEVNULL, check=True, timeout=60, umask=0o022)
    root = Path(temporary) / 'squashfs-root'
    bad = [str(p.relative_to(root)) for p in root.rglob('*') if not p.is_symlink() and ((p.is_file() and not p.stat().st_mode & stat.S_IROTH))]
    assert not bad, ('Unreadable deployed resources', bad[:10])
    defaults = root / 'usr/bin/Defaults'
    athletes = list((defaults / 'Packs/Athletes').glob('*/pack.json'))
    hills = list((defaults / 'Packs/Hills').glob('*.json'))
    assert len(athletes) == len(hills) == 1
    assert len(json.loads(athletes[0].read_text())['profiles']) == 83
    assert len(json.loads(hills[0].read_text())['profiles']) == 128
    assert len(list((defaults / 'Calendars').glob('*.json'))) == 6
    assert not (root / 'usr/bin/userData').exists()
    assert not (root / 'usr/bin/Saves').exists()
    print('PASS AppImage payload: starter packs, six calendars, no user saves, files readable by other users')
env = dict(os.environ)
for key in ('LD_LIBRARY_PATH', 'QML2_IMPORT_PATH', 'QML_IMPORT_PATH', 'QT_PLUGIN_PATH', 'QT_QPA_PLATFORM_PLUGIN_PATH'):
    env.pop(key, None)
env.update(APPIMAGE_EXTRACT_AND_RUN='1', QT_QPA_PLATFORM='offscreen', QT_QUICK_BACKEND='software')
result = subprocess.run([str(artifact), '--deployment-smoke'], env=env, capture_output=True, text=True, timeout=60)
if result.returncode:
    print(result.stdout, result.stderr, file=sys.stderr)
    sys.exit(result.returncode)
assert 'Deployment UI check: PASS' in result.stderr + result.stdout
print('PASS AppImage startup: deployed Qt Quick UI and fresh isolated library')
