#!/usr/bin/env bash
set -euo pipefail
repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
preview_dir="${repo_root}/build-qml-preview"
if [[ ! -x "${preview_dir}/sj-sim" ]]; then
    echo 'Najpierw uruchom scripts/build-qml-preview.sh.' >&2
    exit 1
fi
if [[ "${1:-}" == "--app" ]]; then
    shift
    exec "${repo_root}/scripts/with-qt-runtime.sh" "${preview_dir}/sj-sim" "$@"
fi
if [[ "${1:-}" == "--widgets" ]]; then
    shift
    exec "${repo_root}/scripts/with-qt-runtime.sh" "${preview_dir}/sj-sim" --widgets-competition "$@"
fi
exec "${repo_root}/scripts/with-qt-runtime.sh" "${preview_dir}/sj-sim" --quick-competition "$@"
