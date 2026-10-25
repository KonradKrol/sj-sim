#!/usr/bin/env python3
"""Exercise playground CLI reproducibility, parallelism, replay, exports and cancellation."""
import argparse
import csv
import json
import os
from pathlib import Path
import shutil
import signal
import subprocess
import tempfile
import time


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--exe", type=Path, default=Path("build-quick-ui/sj-sim"))
    parser.add_argument("--competitions", type=int, default=120)
    parser.add_argument("--keep", action="store_true", help="retain isolated reports for inspection")
    parser.add_argument("--output", type=Path, help="write benchmark summary to a new JSON file")
    args = parser.parse_args()
    executable = args.exe.resolve()
    if not executable.is_file() or not 1 <= args.competitions <= 1000000:
        parser.error("A built executable and a competition count between 1 and 1000000 are required")
    directory = Path(tempfile.mkdtemp(prefix="sj-playground-cli-"))
    environment = dict(os.environ)
    runtime = Path(__file__).resolve().parents[1] / "build-qml-preview/qt-runtime/lib"
    if runtime.is_dir() and os.name != "nt":
        environment["LD_LIBRARY_PATH"] = str(runtime) + os.pathsep + environment.get("LD_LIBRARY_PATH", "")
    def command(*options):
        return subprocess.run([str(executable), *map(str, options)], env=environment,
                              check=True, capture_output=True, text=True, encoding="utf-8", timeout=max(60, args.competitions))
    try:
        command("--playground-example", directory / "example.json")
        base = json.loads((directory / "example.json").read_text(encoding="utf-8"))
        base.update(competitions=args.competitions, sampleLimit=16)
        reports = []
        for threads in (1, 8):
            config = dict(base, threads=threads)
            path = directory / f"config {threads}.json"
            path.write_text(json.dumps(config, ensure_ascii=False), encoding="utf-8")
            command("--playground", "--config", path, "--output", directory / f"report-{threads}")
            reports.append(json.loads((directory / f"report-{threads}" / "report.json").read_text(encoding="utf-8")))
        for a, b in zip(reports[0]["variants"], reports[1]["variants"]):
            assert a["distance"]["histogram"] == b["distance"]["histogram"], "thread-dependent distribution"
            assert a["samples"] == b["samples"], "thread-dependent retained samples"
        index = min(777, args.competitions - 1)
        for number in (0, 1):
            command("--playground", "--config", directory / "config 8.json", "--replay", f"1:{index}", "--output", directory / f"replay-{number}.json")
        assert (directory / "replay-0.json").read_bytes() == (directory / "replay-1.json").read_bytes(), "separate processes changed exact replay order or values"
        canceled_count = None
        if os.name != "nt":
            config = dict(base, competitions=1000000, threads=8)
            path = directory / "cancel.json"
            path.write_text(json.dumps(config), encoding="utf-8")
            process = subprocess.Popen([str(executable), "--playground", "--config", str(path), "--output", str(directory / "canceled")],
                                       env=environment, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, encoding="utf-8")
            try:
                time.sleep(.4)
                process.send_signal(signal.SIGINT)
                stdout, stderr = process.communicate(timeout=30)
            except BaseException:
                process.kill(); process.wait(); raise
            assert process.returncode == 130, (process.returncode, stdout, stderr)
            partial = json.loads((directory / "canceled" / "report.json").read_text(encoding="utf-8"))
            assert partial["canceled"] and partial["completed"] < 1000000
            canceled_count = partial["completed"]
        with (directory / "report-8" / "athletes.csv").open(encoding="utf-8", newline="") as stream:
            assert len(list(csv.DictReader(stream))) == 3 * base["athletes"]
        analyzer = Path(__file__).with_name("analyze-playground.py")
        subprocess.run([os.sys.executable, str(analyzer), str(directory / "report-8"), "--json", "--output", str(directory / "analysis.json")], check=True)
        analysis = json.loads((directory / "analysis.json").read_text(encoding="utf-8"))
        assert not analysis[0]["checks"]
        summary = {"status": "PASS", "competitionsPerVariant": args.competitions, "variants": 3,
                   "serialMs": reports[0]["elapsedMs"], "parallelMs": reports[1]["elapsedMs"],
                   "threads": reports[1]["threadsUsed"], "speedup": reports[0]["elapsedMs"] / max(1, reports[1]["elapsedMs"]),
                   "signalCancellationCompleted": canceled_count, "artifacts": str(directory) if args.keep else None}
        if args.output:
            with args.output.open("x", encoding="utf-8") as stream:
                json.dump(summary, stream, indent=2)
        print(json.dumps(summary, ensure_ascii=False))
    finally:
        if not args.keep:
            shutil.rmtree(directory)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
