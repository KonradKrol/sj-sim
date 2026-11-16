#!/usr/bin/env bash
# Prefer the QML modules belonging to the linked Qt; use the old dev runtime only when needed.
set -euo pipefail
repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
legacy="${repo_root}/build-qml-preview/qt-runtime"
[[ $# -gt 0 ]] || { echo 'Usage: with-qt-runtime.sh executable [arguments...]' >&2; exit 2; }
executable="$1"; shift
for variable in LD_LIBRARY_PATH QML2_IMPORT_PATH QML_IMPORT_PATH; do
    IFS=: read -r -a entries <<< "${!variable-}"
    filtered=""
    for entry in "${entries[@]}"; do
        [[ -z "$entry" || "${entry%/}" == "$legacy/lib" || "${entry%/}" == "$legacy/qml" ]] && continue
        filtered="${filtered:+${filtered}:}${entry}"
    done
    if [[ -n "$filtered" ]]; then export "${variable}=${filtered}"; else unset "$variable"; fi
done
qt_major=""; core_dir=""; core_path=""
while read -r library arrow location remainder; do
    if [[ "$library" == libQt5Core.so.5 || "$library" == libQt6Core.so.6 ]]; then
        qt_major="${library:5:1}"
        if [[ "$arrow" == '=>' && -f "$location" ]]; then core_path="$(readlink -f -- "$location")"; core_dir="$(dirname "$core_path")"; fi
        break
    fi
done < <(ldd "$executable" 2>/dev/null || true)
complete_qml() {
    local root="$1" major="$2" module
    if [[ "$major" == 5 ]]; then
        for module in QtQuick.2 QtQuick/Controls.2 QtQuick/Templates.2 QtQuick/Layouts QtQuick/Window.2; do
            [[ -f "$root/$module/qmldir" ]] || return 1
        done
    else
        for module in QtQuick QtQuick/Controls QtQuick/Templates QtQuick/Layouts QtQuick/Window; do
            [[ -f "$root/$module/qmldir" ]] || return 1
        done
    fi
}
qml_root=""
if [[ -n "$core_dir" ]]; then
    for candidate in "$core_dir/../qml" "$core_dir/qt${qt_major}/qml"; do
        if complete_qml "$candidate" "$qt_major"; then qml_root="$(cd "$candidate" && pwd -P)"; break; fi
    done
fi
if [[ -z "$qml_root" && "$qt_major" == 5 ]] && complete_qml "$legacy/qml" 5; then
    qml_root="$legacy/qml"
    export LD_LIBRARY_PATH="$legacy/lib${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}"
fi
# Supply only the missing Shapes module, never another copy of Controls or Quick.
core_version="${core_path##*.so.}"
if [[ "$qt_major" == 5 && "$core_version" == 5.*.* && -n "$qml_root" && ! -f "$qml_root/QtQuick/Shapes/qmldir" && -f "$legacy/qml/QtQuick/Shapes/qmldir" && -f "$legacy/lib/libQt5QuickShapes.so.$core_version" ]]; then
    supplement="$repo_root/build-qt-supplement"
    mkdir -p "$supplement/qml/QtQuick/Shapes" "$supplement/lib"
    cp -au "$legacy/qml/QtQuick/Shapes/." "$supplement/qml/QtQuick/Shapes/"
    cp -au "$legacy/lib/"libQt5QuickShapes.so* "$supplement/lib/"
    printf '%s\n' "$core_version" > "$supplement/qt-version"
    qml_root="$supplement/qml:$qml_root"
    export LD_LIBRARY_PATH="$supplement/lib${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}"
fi
if [[ -n "$qml_root" ]]; then
    export QML2_IMPORT_PATH="$qml_root"
    export QML_IMPORT_PATH="$qml_root"
fi
exec "$executable" "$@"
