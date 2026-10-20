#!/usr/bin/env bash
set -euo pipefail
repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
build_dir="${repo_root}/build-quick-ui"
mkdir -p "${build_dir}"
cd "${build_dir}"
if [[ -d "${repo_root}/build-qml-preview/qt-dev/root/usr/lib/x86_64-linux-gnu/qt5" ]]; then
    export QMAKEPATH="${repo_root}/build-qml-preview/qt-dev/root/usr/lib/x86_64-linux-gnu/qt5${QMAKEPATH:+:${QMAKEPATH}}"
fi
qmake "${repo_root}/sj-sim.pro" "$@"
make -j"${SJ_BUILD_JOBS:-4}"
for directory in userData flags; do
    if [[ ! -d "${build_dir}/${directory}" ]]; then cp -a "${repo_root}/${directory}" "${build_dir}/"; fi
done
mkdir -p translations
lrelease "${repo_root}/translations/translation_en.ts" -qm translations/translation_en.qm
