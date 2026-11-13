#!/usr/bin/env python3
"""Use the local Qt 5 preview runtime only when the system QML SDK is absent."""
import os
import subprocess
import sys

args = sys.argv[1:]
result = subprocess.run([os.environ['SJ_REAL_QMAKE'], *args], capture_output=True, text=True)
if result.returncode:
    sys.stderr.write(result.stderr)
    sys.exit(result.returncode)
values = {'QT_INSTALL_QML': os.environ['SJ_QT_QML_ROOT'], 'QT_HOST_BINS': os.environ['SJ_QT_BIN_ROOT'], 'QT_INSTALL_BINS': os.environ['SJ_QT_BIN_ROOT']}
if '-query' in args:
    keys = args[args.index('-query') + 1:]
    if len(keys) == 1 and keys[0] in values:
        print(values[keys[0]])
        sys.exit(0)
    for line in result.stdout.splitlines():
        key = line.split(':', 1)[0]
        print(key + ':' + values[key] if key in values else line)
else:
    sys.stdout.write(result.stdout)
