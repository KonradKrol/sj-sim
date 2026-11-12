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
if [[ ! -d flags ]]; then cp -a "${repo_root}/flags" .; fi
rm -rf Defaults
cp -R "${repo_root}/data/defaults" Defaults
mkdir -p translations
for language in en cs; do
    lrelease -nounfinished "${repo_root}/translations/translation_${language}.ts" -qm "translations/translation_${language}.qm"
done
