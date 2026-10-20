#!/usr/bin/env bash
set -euo pipefail
repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
preview_dir="${repo_root}/build-qml-preview"
if [[ ! -x "${preview_dir}/sj-sim" ]]; then
    echo 'Najpierw uruchom scripts/build-qml-preview.sh.' >&2
    exit 1
fi
if [[ -d "${preview_dir}/qt-runtime" ]]; then
    export LD_LIBRARY_PATH="${preview_dir}/qt-runtime/lib${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}"
    export QML2_IMPORT_PATH="${preview_dir}/qt-runtime/qml${QML2_IMPORT_PATH:+:${QML2_IMPORT_PATH}}"
fi
if [[ "${1:-}" == "--app" ]]; then
    shift
    exec "${preview_dir}/sj-sim" "$@"
fi
if [[ "${1:-}" == "--widgets" ]]; then
    shift
    exec "${preview_dir}/sj-sim" --widgets-competition "$@"
fi
exec "${preview_dir}/sj-sim" --quick-competition "$@"
