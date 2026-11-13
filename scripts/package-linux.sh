#!/usr/bin/env bash
set -euo pipefail
umask 022
repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
build_path="$(cd "${repo_root}" && realpath -m -- "${1:-build-quick-ui}")"
output_path="$(cd "${repo_root}" && realpath -m -- "${2:-dist}")"
dist_path="${repo_root}/dist"
if [[ "$(realpath -m -- "${dist_path}")" != "${dist_path}" || ( "${output_path}" != "${dist_path}" && "${output_path}" != "${dist_path}/"* ) ]]; then
    echo "Output directory must be dist or one of its subdirectories." >&2; exit 1
fi
[[ "$(uname -m)" == x86_64 ]] || { echo "This packager targets Linux x86_64." >&2; exit 1; }
executable="${build_path}/sj-sim"
[[ -x "${executable}" ]] || executable="${build_path}/release/sj-sim"
[[ -x "${executable}" ]] || { echo "Build sj-sim first: ${build_path}" >&2; exit 1; }
for tool in python3 lrelease qmake; do command -v "$tool" >/dev/null || { echo "Missing build tool: $tool" >&2; exit 1; }; done
version="$(python3 - "${repo_root}/source/UI/mainwindow.cpp" <<'PY'
import re,sys
match=re.search(r'const QString appVersion\s*=\s*"([0-9]+\.[0-9]+\.[0-9]+(?:[-+][A-Za-z0-9.-]+)?)"',open(sys.argv[1]).read())
if not match:sys.exit('Cannot determine application version')
print(match[1])
PY
)"
tool_dir="$(python3 "${repo_root}/scripts/fetch-linuxdeploy.py")"
mkdir -p "${output_path}"
workspace="$(mktemp -d "${dist_path}/.linux-package-XXXXXX")"
trap 'rm -rf -- "${workspace}"' EXIT
appdir="${workspace}/SjSim.AppDir"
mkdir -p "${appdir}/usr/bin/translations" "${appdir}/usr/share/mime/packages" "${appdir}/usr/share/doc/sj-sim"
cp "${executable}" "${appdir}/usr/bin/sj-sim"
cp -R "${repo_root}/flags" "${appdir}/usr/bin/flags"
cp -R "${repo_root}/data/defaults" "${appdir}/usr/bin/Defaults"
cp "${repo_root}/LICENSE" "${appdir}/usr/share/doc/sj-sim/LICENSE"
cp "${repo_root}/packaging/linux/sjsim-pack.xml" "${appdir}/usr/share/mime/packages/"
printf 'Sj.Sim %s\nPackaging source commit: %s\n' "${version}" "$(git -C "${repo_root}" rev-parse HEAD)" > "${appdir}/usr/share/doc/sj-sim/BUILD.txt"
for language in en cs; do lrelease -nounfinished "${repo_root}/translations/translation_${language}.ts" -qm "${appdir}/usr/bin/translations/translation_${language}.qm"; done
qt_translations="$(qmake -query QT_INSTALL_TRANSLATIONS)"
for language in pl cs; do cp "${qt_translations}/qtbase_${language}.qm" "${appdir}/usr/bin/translations/"; done

# Our Linux development fallback supplies a complete, matching Qt 5 QML runtime.
# CI and normal complete Qt installations use their own SDK without this wrapper.
export QMAKE="${QMAKE:-$(command -v qmake)}"
if [[ "$($QMAKE -query QT_VERSION)" == 5.* && ! -d "$($QMAKE -query QT_INSTALL_QML)/QtQuick.2" && -d "${repo_root}/build-qml-preview/qt-runtime/qml/QtQuick.2" ]]; then
    export SJ_REAL_QMAKE="${QMAKE}"
    export SJ_QT_QML_ROOT="${repo_root}/build-qml-preview/qt-runtime/qml"
    export SJ_QT_BIN_ROOT="${repo_root}/build-qml-preview/qt-dev/root/usr/lib/qt5/bin"
    export QMAKE="${repo_root}/packaging/linux/qmake-runtime-wrapper.py"
    export LD_LIBRARY_PATH="${repo_root}/build-qml-preview/qt-runtime/lib${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}"
fi
export QML_SOURCES_PATHS="${repo_root}/source/UI/Quick"
export EXTRA_QT_MODULES="svg${EXTRA_QT_MODULES:+;${EXTRA_QT_MODULES}}"
export EXTRA_PLATFORM_PLUGINS="libqoffscreen.so${EXTRA_PLATFORM_PLUGINS:+;${EXTRA_PLATFORM_PLUGINS}}"
export APPIMAGE_EXTRACT_AND_RUN=1
export LDAI_OUTPUT="${workspace}/SjSim-${version}-linux-x86_64.AppImage"
export VERSION="${version}"
export ARCH=x86_64
chmod -R a+rX "${appdir}"
python3 "${repo_root}/scripts/extract-app-icon.py" "${repo_root}/sj-sim.ico" "${workspace}/sj-sim.png"
"${tool_dir}/linuxdeploy-x86_64.AppImage" --appdir "${appdir}" \
    --executable "${appdir}/usr/bin/sj-sim" --desktop-file "${repo_root}/packaging/linux/sj-sim.desktop" \
    --icon-file "${workspace}/sj-sim.png" --plugin qt
# Deployment tools can preserve restrictive permissions from their own runtime.
chmod -R a+rX "${appdir}"
"${tool_dir}/linuxdeploy-x86_64.AppImage" --appdir "${appdir}" --output appimage
[[ -s "${LDAI_OUTPUT}" ]] || { echo "AppImage was not produced." >&2; exit 1; }
chmod +x "${LDAI_OUTPUT}"
artifact="${output_path}/$(basename "${LDAI_OUTPUT}")"
mv -f -- "${LDAI_OUTPUT}" "${artifact}"
(cd "${output_path}" && sha256sum "$(basename "${artifact}")" > "$(basename "${artifact}").sha256")
echo "Ready: ${artifact}"
