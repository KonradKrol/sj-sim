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
# Seed a fresh development installation without overwriting anyone's packs.
if [[ ! -e "${build_dir}/userData/GlobalDatabase/packs.json" ]]; then
    cp "${repo_root}/data/beta/packs.json" "${build_dir}/userData/GlobalDatabase/packs.json"
fi
if [[ ! -e "${build_dir}/userData/Packs" ]]; then
    cp -R "${repo_root}/data/beta/Packs" "${build_dir}/userData/Packs"
fi
mkdir -p translations
for language in en cs; do
    lrelease -nounfinished "${repo_root}/translations/translation_${language}.ts" -qm "translations/translation_${language}.qm"
done
