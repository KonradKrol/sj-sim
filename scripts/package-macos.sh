#!/usr/bin/env bash
# Packages an already built sj-sim.app into dist/SjSim-<version>-macos-universal.dmg.
# Usage: scripts/package-macos.sh <build-dir> [output-dir]
# For distribution, set SJSIM_MAC_SIGN_IDENTITY to a Developer ID Application
# identity and SJSIM_MAC_NOTARY_PROFILE to a stored notarytool keychain profile.
# Without both settings this produces an ad-hoc signed development build.
set -euo pipefail
umask 022
repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
build_path="$(cd "${repo_root}" && cd "${1:-build/macos}" && pwd)"
output_path="${repo_root}/${2:-dist}"
[[ "$(uname -s)" == Darwin ]] || { echo "This packager runs on macOS." >&2; exit 1; }
sign_identity="${SJSIM_MAC_SIGN_IDENTITY:-}"
notary_profile="${SJSIM_MAC_NOTARY_PROFILE:-}"
if [[ -n "${sign_identity}" && -z "${notary_profile}" || -z "${sign_identity}" && -n "${notary_profile}" ]]; then
    echo "Set both SJSIM_MAC_SIGN_IDENTITY and SJSIM_MAC_NOTARY_PROFILE for distribution." >&2
    exit 1
fi
for tool in python3 lrelease qmake macdeployqt codesign hdiutil sips iconutil /usr/libexec/PlistBuddy; do
    command -v "$tool" >/dev/null || { echo "Missing build tool: $tool" >&2; exit 1; }
done
built_app="${build_path}/sj-sim.app"
[[ -x "${built_app}/Contents/MacOS/sj-sim" ]] || { echo "Build sj-sim first: ${built_app}" >&2; exit 1; }
version="$(python3 - "${repo_root}/source/UI/mainwindow.cpp" <<'PY'
import re,sys
match=re.search(r'const QString appVersion\s*=\s*"([0-9]+\.[0-9]+\.[0-9]+(?:[-+][A-Za-z0-9.-]+)?)"',open(sys.argv[1]).read())
if not match:sys.exit('Cannot determine application version')
print(match[1])
PY
)"

mkdir -p "${output_path}"
workspace="$(mktemp -d "${TMPDIR:-/tmp}/sjsim-macos-XXXXXX")"
trap 'rm -rf -- "${workspace}"' EXIT
app="${workspace}/Sj.Sim.app"
cp -R "${built_app}" "${app}"
resources="${app}/Contents/Resources"
mkdir -p "${resources}/translations"

# Read-only game assets: AppPaths::resources() points to Contents/Resources on macOS.
cp -R "${repo_root}/flags" "${resources}/flags"
cp -R "${repo_root}/data/defaults" "${resources}/Defaults"
cp "${repo_root}/LICENSE" "${resources}/LICENSE"
for language in en cs; do
    lrelease -nounfinished "${repo_root}/translations/translation_${language}.ts" -qm "${resources}/translations/translation_${language}.qm"
done
qt_translations="$(qmake -query QT_INSTALL_TRANSLATIONS)"
for language in pl cs; do cp "${qt_translations}/qtbase_${language}.qm" "${resources}/translations/"; done

# Application icon generated from the shared PNG.
iconset="${workspace}/sj-sim.iconset"
mkdir -p "${iconset}"
for size in 16 32 128 256 512; do
    sips -z "${size}" "${size}" "${repo_root}/sj-sim.png" --out "${iconset}/icon_${size}x${size}.png" >/dev/null
    double=$((size * 2))
    sips -z "${double}" "${double}" "${repo_root}/sj-sim.png" --out "${iconset}/icon_${size}x${size}@2x.png" >/dev/null
done
iconutil -c icns "${iconset}" -o "${resources}/sj-sim.icns"

plist="${app}/Contents/Info.plist"
set_plist() {
    /usr/libexec/PlistBuddy -c "Set :$1 $2" "${plist}" 2>/dev/null || /usr/libexec/PlistBuddy -c "Add :$1 $3 $2" "${plist}"
}
set_plist CFBundleName "Sj.Sim" string
set_plist CFBundleDisplayName "Sj.Sim" string
set_plist CFBundleIconFile "sj-sim.icns" string
set_plist CFBundleShortVersionString "${version}" string
set_plist CFBundleVersion "${version}" string
set_plist NSHighResolutionCapable true bool
set_plist LSMinimumSystemVersion "12.0" string
set_plist NSHumanReadableCopyright "GPL-3.0" string

# Qt frameworks, plugins and the QML modules imported by the interface.
signing_option="-codesign=-"
if [[ -n "${sign_identity}" ]]; then
    signing_option="-sign-for-notarization=${sign_identity}"
fi
macdeployqt "${app}" -qmldir="${repo_root}/source/UI/Quick" -qmldir="${repo_root}/source/UI/Competition/Quick" -always-overwrite "${signing_option}"

# Start the bundled program with an isolated library; the smoke run must not
# leave files inside the bundle. macdeployqt ships only the Cocoa platform plugin, so the run
# uses the normal macOS window session of the build machine.
smoke_log="${workspace}/smoke.log"
if ! env -u QML2_IMPORT_PATH -u QML_IMPORT_PATH -u QT_PLUGIN_PATH -u DYLD_LIBRARY_PATH -u DYLD_FRAMEWORK_PATH \
    QT_QUICK_BACKEND=software "${app}/Contents/MacOS/sj-sim" --deployment-smoke >"${smoke_log}" 2>&1; then
    # Repeated as CI annotations, which are readable without access to the raw log.
    tail -n 40 "${smoke_log}" | while IFS= read -r line; do echo "::error::smoke: ${line//%/%25}"; done
    if [[ -f "${app}/Contents/MacOS/deployment-smoke.log" ]]; then
        while IFS= read -r line; do echo "::error::reached: ${line}"; done < "${app}/Contents/MacOS/deployment-smoke.log"
    fi
    exit 1
fi
rm -f "${app}/Contents/MacOS/deployment-smoke.log"

if [[ -z "${sign_identity}" ]]; then
    codesign --force --deep --sign - "${app}"
fi
codesign --verify --deep --strict "${app}"

staging="${workspace}/dmg"
mkdir -p "${staging}"
mv "${app}" "${staging}/"
ln -s /Applications "${staging}/Applications"
artifact="${output_path}/SjSim-${version}-macos-universal.dmg"
rm -f "${artifact}"
hdiutil create -volname "Sj.Sim ${version}" -srcfolder "${staging}" -ov -format UDZO "${artifact}" >/dev/null
if [[ -n "${sign_identity}" ]]; then
    codesign --force --timestamp --sign "${sign_identity}" "${artifact}"
    xcrun notarytool submit "${artifact}" --keychain-profile "${notary_profile}" --wait
    xcrun stapler staple "${artifact}"
    xcrun stapler validate "${artifact}"
fi
(cd "${output_path}" && shasum -a 256 "$(basename "${artifact}")" > "$(basename "${artifact}").sha256")
echo "Ready: ${artifact}"
