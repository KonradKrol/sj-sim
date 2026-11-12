#!/usr/bin/env python3
"""Link isolated backend checks against an already built Quick app; optional test filename."""
import os
from pathlib import Path
import shlex
import subprocess
import tempfile
import sys

root = Path(__file__).resolve().parent.parent
build = root / 'build-quick-ui'
lines = (build / 'Makefile').read_text().splitlines()
def flags(key):
    return shlex.split(next(line.split('=', 1)[1].strip() for line in lines if line.startswith(key + ' ')))
objects = [str(path) for path in (build / '.obj').glob('*.o') if path.name not in {'main.o', 'quick-ui-smoke.o'}]
with tempfile.TemporaryDirectory(prefix='sjsim-library-check-') as temporary:
    executable = str(Path(temporary) / 'library-check')
    subprocess.run(['g++', '-O2', '-fPIC', '-std=c++17', *flags('INCPATH'), str(root / 'tests' / (sys.argv[1] if len(sys.argv)>1 else 'resource-library.cpp')), *objects, *[f for f in flags('LIBS') if '$' not in f], '-o', executable], cwd=build, check=True)
    env = dict(os.environ, QT_QPA_PLATFORM='offscreen', QT_QUICK_BACKEND='software')
    runtime = root / 'build-qml-preview/qt-runtime'
    if runtime.exists():
        env['LD_LIBRARY_PATH'] = str(runtime / 'lib') + ':' + env.get('LD_LIBRARY_PATH', '')
        env['QML2_IMPORT_PATH'] = str(runtime / 'qml')
    subprocess.run([executable, str(root)], cwd=build, env=env, check=True)
