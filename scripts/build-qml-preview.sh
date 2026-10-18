#!/usr/bin/env bash
set -euo pipefail
repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
build_dir="${repo_root}/build-qml-preview"
mkdir -p "${build_dir}"
cd "${build_dir}"
qmake "${repo_root}/sj-sim.pro" CONFIG+=competition_quick
make -j"${SJ_BUILD_JOBS:-4}"
# Keep the preview's own data on subsequent builds; never overwrite user edits.
for directory in userData flags; do
    if [[ ! -d "${build_dir}/${directory}" ]]; then
        cp -a "${repo_root}/${directory}" "${build_dir}/"
    fi
done
mkdir -p translations
lrelease "${repo_root}/translations/translation_en.ts" -qm translations/translation_en.qm
printf 'Uruchom: %s/scripts/run-qml-preview.sh\n' "${repo_root}"
