#!/usr/bin/env python3
"""Linux/qmake benchmark and regression checks; never saves the loaded season."""
import argparse
import os
from pathlib import Path
import shlex
import subprocess

parser = argparse.ArgumentParser()
parser.add_argument('save', type=Path)
parser.add_argument('--build-dir', type=Path, default=Path('build-quick-ui'))
args = parser.parse_args()
root = Path(__file__).resolve().parents[1]
build = args.build_dir.resolve()
save = args.save.resolve()
lines = (build / 'Makefile').read_text().splitlines()
def flags(key):
    return shlex.split(next(line.split('=', 1)[1].strip() for line in lines if line.startswith(key + ' ')))
objects = [str(path) for path in sorted((build / '.obj').glob('*.o'))
           if path.name not in ('main.o', 'quick-ui-smoke.o')]
output = build / 'performance-checks'
output.mkdir(exist_ok=True)
env = os.environ.copy()
env.setdefault('QT_QPA_PLATFORM', 'offscreen')
env.setdefault('QT_QUICK_BACKEND', 'software')
runtime = root / 'build-qml-preview/qt-runtime'
if runtime.exists():
    env['LD_LIBRARY_PATH'] = str(runtime / 'lib') + ':' + env.get('LD_LIBRARY_PATH', '')
    env['QML2_IMPORT_PATH'] = str(runtime / 'qml') + ':' + env.get('QML2_IMPORT_PATH', '')
for name in ('save-load-performance', 'season-performance-regressions',
             'gate-preview-performance', 'season-auto-performance'):
    executable = output / name
    subprocess.run([env.get('CXX', 'g++'), '-std=c++17', '-O2', '-fPIC', *flags('INCPATH'),
                    str(root / 'tests' / (name + '.cpp')), *objects,
                    *[flag for flag in flags('LIBS') if '$' not in flag], '-o', str(executable)],
                   cwd=build, check=True)
    subprocess.run([str(executable), str(save)], cwd=root, env=env, check=True)
