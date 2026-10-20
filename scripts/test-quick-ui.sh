#!/usr/bin/env bash
set -euo pipefail
repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
"${repo_root}/scripts/build-quick-ui.sh"
cd "${repo_root}/build-quick-ui"
if [[ -d "${repo_root}/build-qml-preview/qt-dev/root/usr/lib/x86_64-linux-gnu/qt5" ]]; then
    export QMAKEPATH="${repo_root}/build-qml-preview/qt-dev/root/usr/lib/x86_64-linux-gnu/qt5${QMAKEPATH:+:${QMAKEPATH}}"
fi
qmake "${repo_root}/sj-sim.pro" CONFIG+=quick_ui_smoke -o Makefile.smoke
make -f Makefile.smoke -j"${SJ_BUILD_JOBS:-4}"
if [[ -d "${repo_root}/build-qml-preview/qt-runtime" ]]; then
    export LD_LIBRARY_PATH="${repo_root}/build-qml-preview/qt-runtime/lib${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}"
    export QML2_IMPORT_PATH="${repo_root}/build-qml-preview/qt-runtime/qml${QML2_IMPORT_PATH:+:${QML2_IMPORT_PATH}}"
fi
export QT_QPA_PLATFORM="${QT_QPA_PLATFORM:-offscreen}"
export QT_QUICK_BACKEND="${QT_QUICK_BACKEND:-software}"
exec ./sj-sim-ui-smoke "$@"
