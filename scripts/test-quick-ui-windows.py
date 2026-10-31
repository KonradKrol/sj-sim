#!/usr/bin/env python3
"""Run the Windows UI smoke executable with isolated executable-relative data."""
import argparse
import os
from pathlib import Path
import shutil
import subprocess
import tempfile


def main():
    repo = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--executable", type=Path,
                        default=repo / "build-win-smoke/release/sj-sim-ui-smoke.exe")
    parser.add_argument("--qt-kit", type=Path,
                        default=repo / "build-tools/Qt/6.8.3/msvc2022_64")
    parser.add_argument("--log", type=Path, default=repo / "build-win-smoke/ui-test.log")
    parser.add_argument("--timeout", type=int, default=2400)
    parser.add_argument("--renders", type=Path, help="Copy test screenshots to this directory")
    parser.add_argument("arguments", nargs=argparse.REMAINDER)
    args = parser.parse_args()
    executable, kit = args.executable.resolve(), args.qt_kit.resolve()
    env = os.environ.copy()
    env.update(QT_QPA_PLATFORM="offscreen", QT_QUICK_BACKEND="software",
               QT_FORCE_STDERR_LOGGING="1", QT_PLUGIN_PATH=str(kit / "plugins"),
               QML_IMPORT_PATH=str(kit / "qml"), QML2_IMPORT_PATH=str(kit / "qml"))
    env["PATH"] = str(kit / "bin") + os.pathsep + env.get("PATH", "")
    flags = args.arguments[1:] if args.arguments[:1] == ["--"] else args.arguments
    args.log.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="sj-ui-runtime-") as folder:
        root = Path(folder)
        isolated = root / executable.name
        shutil.copy2(executable, isolated)
        for name in ("userData", "flags", "translations"):
            shutil.copytree(executable.parent / name, root / name)
        with args.log.open("wb") as log:
            result = subprocess.run([str(isolated), *flags], cwd=root, env=env,
                                    stdout=log, stderr=subprocess.STDOUT,
                                    timeout=args.timeout)
        if args.renders and (root / "ui-renders").exists():
            shutil.copytree(root / "ui-renders", args.renders, dirs_exist_ok=True)
        print(f"UI smoke exit {result.returncode}; log: {args.log}")
        return result.returncode


if __name__ == "__main__":
    raise SystemExit(main())
