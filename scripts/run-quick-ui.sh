#!/usr/bin/env bash
set -euo pipefail
repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
if [[ -d "${repo_root}/build-qml-preview/qt-runtime" ]]; then
    export LD_LIBRARY_PATH="${repo_root}/build-qml-preview/qt-runtime/lib${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}"
    export QML2_IMPORT_PATH="${repo_root}/build-qml-preview/qt-runtime/qml${QML2_IMPORT_PATH:+:${QML2_IMPORT_PATH}}"
fi
exec "${repo_root}/build-quick-ui/sj-sim" "$@"
