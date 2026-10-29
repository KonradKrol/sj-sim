#!/usr/bin/env python3
"""Exercise laboratory exports and CLI/UI experiment compatibility against a built game."""
import argparse
import copy
import json
from pathlib import Path
import subprocess
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("executable", type=Path)
    args = parser.parse_args()
    executable = args.executable.resolve()
    with tempfile.TemporaryDirectory(prefix="sj-balance-cli-") as folder:
        root = Path(folder)

        def run(*arguments, success=True):
            result = subprocess.run([str(executable), *map(str, arguments)],
                                    capture_output=True, timeout=120)
            assert (result.returncode == 0) == success, (
                result.returncode, result.stderr.decode("utf-8", errors="replace"))
            return result

        def read(path):
            return json.loads(path.read_text(encoding="utf-8"))

        experiment = root / "experiment.json"
        run("--balance-lab-example", experiment)
        wrapper = read(experiment)
        assert wrapper["kind"] == "sj-sim-balance-experiment"
        config = wrapper["config"]
        config.update(competitions=32, athletes=12, focus=0, rounds=[12, 6],
                      threads=1, sampleLimit=3, targets=[])
        config["variants"][2]["changes"]["athlete.overall"] = 95
        wrapper["options"].update(resolution=3, view="contest")
        experiment.write_text(json.dumps(wrapper), encoding="utf-8")
        reports = []
        for threads in (1, 4):
            output = root / f"run-{threads}"
            run("--balance-lab", "--config", experiment, "--threads", threads,
                "--output", output)
            result = read(output / "balance-analysis.json")
            assert result["full"] and result["report"]["completed"] == 32
            assert len(result["summaries"]) == 3
            assert result["report"]["variants"][2]["pairedDistance"]["mean"] > 5
            assert (output / "athletes.csv").stat().st_size > 100
            reports.append(result["report"])
        for first, second in zip(reports[0]["variants"], reports[1]["variants"]):
            assert first["samples"] == second["samples"]
            assert first["distance"]["histogram"] == second["distance"]["histogram"]
            assert abs(first["distance"]["mean"]-second["distance"]["mean"]) < 1e-9

        run("--balance-lab", "--config", experiment, "--preview", "--matrix",
            "--output", root / "matrix")
        matrix = read(root / "matrix/balance-analysis.json")
        assert len(matrix["grid"]) == 9 and matrix["columns"] == matrix["rows"] == 3
        assert all("error" not in cell for cell in matrix["grid"])
        assert matrix["report"]["completed"] <= 12

        replay_path, analysis_path = root / "replay.json", root / "wind.json"
        run("--balance-lab", "--config", experiment, "--replay", "2:0",
            "--output", replay_path)
        replay = read(replay_path)
        assert replay["jumps"] and any(j.get("detail", {}).get("exposures")
                                       for j in replay["jumps"])
        run("--balance-lab", "--config", experiment, "--analysis", 2,
            "--output", analysis_path)
        assert read(analysis_path)["timeline"]
        invalid = copy.deepcopy(wrapper)
        invalid["config"]["variants"][2]["changes"].update({
            "generator.preset.0.strength.min": 5,
            "generator.preset.0.strength.max": 1})
        experiment.write_text(json.dumps(invalid), encoding="utf-8")
        run("--balance-lab", "--config", experiment, "--output", root / "invalid",
            success=False)
        assert not (root / "invalid").exists()
        print(f"PASS: {len(config['defaults'])} parameters; three variants; paired runs "
              "on 1/4 threads; map; replay; diagnostics; invalid generator rejected")


if __name__ == "__main__":
    main()
