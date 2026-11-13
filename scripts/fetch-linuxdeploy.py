#!/usr/bin/env python3
"""Cache official linuxdeploy tools, verifying GitHub's SHA-256 asset digests."""
import hashlib
import json
import os
from pathlib import Path
import sys
import urllib.request

root = Path(__file__).resolve().parent.parent
cache = root / 'build-packaging-tools/linuxdeploy'
cache.mkdir(parents=True, exist_ok=True)
manifest = cache / 'downloads.json'
records = json.loads(manifest.read_text()) if manifest.exists() else {}
for project in ('linuxdeploy', 'linuxdeploy-plugin-qt'):
    name = project + '-x86_64.AppImage'
    target = cache / name
    expected = records.get(name, {}).get('sha256', '')
    if target.exists() and expected and hashlib.sha256(target.read_bytes()).hexdigest() == expected:
        continue
    if os.environ.get('SJ_PACKAGE_OFFLINE') == '1':
        sys.exit(f'Missing verified packaging tool: {name}. Run once with network access.')
    print(f'Downloading official {name}…', file=sys.stderr)
    request = urllib.request.Request(f'https://api.github.com/repos/linuxdeploy/{project}/releases/tags/continuous', headers={'User-Agent': 'SjSim-packaging'})
    with urllib.request.urlopen(request, timeout=60) as response:
        release = json.load(response)
    asset = next(item for item in release['assets'] if item['name'] == name)
    digest = asset.get('digest') or ''
    if not digest.startswith('sha256:'):
        sys.exit(f'GitHub did not provide a SHA-256 digest for {name}.')
    expected = digest.split(':', 1)[1]
    temporary = target.with_suffix('.download')
    try:
        with urllib.request.urlopen(asset['browser_download_url'], timeout=120) as source, temporary.open('wb') as output:
            while chunk := source.read(1024 * 1024):
                output.write(chunk)
        if hashlib.sha256(temporary.read_bytes()).hexdigest() != expected:
            sys.exit(f'Checksum mismatch: {name}. No tool was installed.')
        temporary.chmod(0o755)
        temporary.replace(target)
    finally:
        temporary.unlink(missing_ok=True)
    records[name] = {'url': asset['browser_download_url'], 'sha256': expected, 'release': release['id']}
    pending = manifest.with_suffix('.tmp')
    pending.write_text(json.dumps(records, indent=2) + '\n')
    pending.replace(manifest)
print(cache)
