#!/usr/bin/env python3
"""Check runtime selection without requiring installed Qt 6 or changing the real SDK."""
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile

repo = Path(__file__).resolve().parents[1]
wrapper = repo / 'scripts/with-qt-runtime.sh'
legacy = repo / 'build-qml-preview/qt-runtime'
with tempfile.TemporaryDirectory(prefix='sjsim-qt-launcher-') as temporary:
    root = Path(temporary)
    tools = root / 'bin'
    tools.mkdir()
    fake_ldd = tools / 'ldd'
    fake_ldd.write_text('#!/usr/bin/env python3\nimport os\nv=os.environ["FAKE_QT_MAJOR"];print("libQt"+v+"Core.so."+v+" => "+os.environ["FAKE_QT_CORE"]+" (0x123)")\n')
    fake_ldd.chmod(0o755)
    for major in (5, 6):
        for complete in (False, True):
            library = root / f'qt-{major}-{complete}' / 'lib'
            library.mkdir(parents=True)
            core = library / f'libQt{major}Core.so.{major}'
            core.touch()
            qml = library / f'qt{major}/qml'
            modules = ('QtQuick.2', 'QtQuick/Controls.2', 'QtQuick/Templates.2', 'QtQuick/Layouts', 'QtQuick/Window.2') if major == 5 else ('QtQuick', 'QtQuick/Controls', 'QtQuick/Templates', 'QtQuick/Layouts', 'QtQuick/Window')
            if complete:
                for module in modules:
                    path = qml / module
                    path.mkdir(parents=True, exist_ok=True)
                    (path / 'qmldir').touch()
            env = dict(os.environ, PATH=str(tools)+':'+os.environ['PATH'], FAKE_QT_MAJOR=str(major), FAKE_QT_CORE=str(core),
                       QML2_IMPORT_PATH=str(legacy/'qml'), QML_IMPORT_PATH=str(legacy/'qml'), LD_LIBRARY_PATH=str(legacy/'lib')+':/non-qt-library')
            code = 'import json,os,sys;print(json.dumps({"qml":os.getenv("QML2_IMPORT_PATH",""),"qml6":os.getenv("QML_IMPORT_PATH",""),"ld":os.getenv("LD_LIBRARY_PATH",""),"arg":sys.argv[1]}))'
            result = subprocess.run([str(wrapper), sys.executable, '-c', code, 'argument with spaces'], env=env, text=True, capture_output=True, check=True)
            values = json.loads(result.stdout)
            assert values['arg'] == 'argument with spaces'
            if complete:
                assert values['qml'] == values['qml6'] == str(qml)
                assert str(legacy) not in values['ld']
            elif major == 6:
                assert str(legacy) not in values['qml']+values['qml6']+values['ld']
            elif all((legacy/'qml'/module/'qmldir').exists() for module in modules):
                assert values['qml'] == values['qml6'] == str(legacy/'qml')
                assert values['ld'].startswith(str(legacy/'lib')+':')
print('PASS Qt 5/6 native module selection, safe Qt 5 fallback, stale environment cleanup and argument forwarding')
