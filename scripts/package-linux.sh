#!/usr/bin/env bash
set -euo pipefail

build_dir="${1:-build-linux}"
output_dir="${2:-dist/sj-sim-linux-x64}"
repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
build_path="${repo_root}/${build_dir}"
output_path="${repo_root}/${output_dir}"

if [[ ! -x "${build_path}/sj-sim" && ! -x "${build_path}/release/sj-sim" ]]; then
    echo "sj-sim was not found under ${build_path}" >&2
    exit 1
fi

if [[ "${output_path}" != "${repo_root}/dist/"* ]]; then
    echo "Output directory must be inside ${repo_root}/dist" >&2
    exit 1
fi

rm -rf "${output_path}"
mkdir -p "${output_path}/usr/bin/translations"

executable="${build_path}/sj-sim"
if [[ ! -x "${executable}" ]]; then
    executable="${build_path}/release/sj-sim"
fi
cp "${executable}" "${output_path}/usr/bin/sj-sim"
cp -R "${repo_root}/flags" "${repo_root}/userData" "${output_path}/usr/bin/"
lrelease "${repo_root}/translations/translation_en.ts" \
    -qm "${output_path}/usr/bin/translations/translation_en.qm"

cat > "${repo_root}/sj-sim.desktop" <<'DESKTOP'
[Desktop Entry]
Type=Application
Name=Sj.Sim
Exec=sj-sim
Icon=sj-sim
Categories=Game;Simulation;
DESKTOP

if [[ ! -x "${repo_root}/linuxdeploy-x86_64.AppImage" ]]; then
    echo "linuxdeploy-x86_64.AppImage is required for Linux packaging" >&2
    exit 1
fi

"${repo_root}/linuxdeploy-x86_64.AppImage" \
    --appdir "${output_path}" \
    --desktop-file "${repo_root}/sj-sim.desktop" \
    --icon-file "${repo_root}/sj-sim.png" \
    --plugin qt

tar -C "${repo_root}/dist" -czf "${repo_root}/dist/sj-sim-linux-x64.tar.gz" \
    "$(basename "${output_path}")"
echo "Packaged Linux application at ${output_path}"
