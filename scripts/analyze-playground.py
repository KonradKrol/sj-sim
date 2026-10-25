#!/usr/bin/env python3
"""Summarize Sj.Sim playground reports using only Python's standard library."""
import argparse
import json
from pathlib import Path


def metric(obj, key, field="mean"):
    return obj.get(key, {}).get(field)


def formatted(value, precision=2):
    return "—" if value is None else f"{value:.{precision}f}"


def load_report(path):
    source = Path(path)
    if source.is_dir():
        source /= "report.json"
    report = json.loads(source.read_text(encoding="utf-8"))
    if report.get("schema") != 1 or not isinstance(report.get("variants"), list):
        raise ValueError(f"Unsupported report: {source}")
    return source, report


def check_report(report):
    errors = []
    completed = report["completed"]
    for case in report["variants"]:
        name = case["name"]
        if case["competitions"] != completed:
            errors.append(f"{name}: paired competition count differs")
        for key in ("distance", "points"):
            distribution = case[key]
            if sum(b["count"] for b in distribution["histogram"]) != distribution["count"]:
                errors.append(f"{name}: {key} histogram count differs")
        if case["distance"]["count"] + case["dsq"] != case["jumps"]:
            errors.append(f"{name}: valid jumps + DSQ do not match all jumps")
        for athlete in case["athletes"]:
            if sum(b["count"] for b in athlete["places"]) != athlete["starts"]:
                errors.append(f"{name}: athlete {athlete['index']} place count differs")
            if report.get("competitionType") != "team" and athlete["jumps"] < athlete["starts"]:
                errors.append(f"{name}: athlete {athlete['index']} has fewer jumps than starts")
        for sample in case["samples"]:
            if not 0 <= sample["competition"] < report["config"]["competitions"]:
                errors.append(f"{name}: invalid retained competition index")
    return errors


def summarize(path, report, selected):
    focus = report["config"]["focus"] if selected is None else selected
    if not 0 <= focus < report["config"]["athletes"]:
        raise ValueError("Focus index outside roster (indices are zero-based)")
    base = report["variants"][0]
    rows, notes = [], []
    for index, case in enumerate(report["variants"]):
        athlete = case["athletes"][focus]
        paired = athlete.get("pairedPlace", case["pairedPlace"] if focus == report["config"]["focus"] else {})
        row = {
            "variant": case["name"], "athlete": athlete["name"],
            "competitions": case["competitions"], "starts": athlete["starts"],
            "mean_place": metric(athlete, "place"),
            "place_sd": metric(athlete, "place", "sd"),
            "win_percent": 100 * athlete["wins"] / athlete["starts"] if athlete["starts"] else None,
            "podium_percent": 100 * athlete["podiums"] / athlete["starts"] if athlete["starts"] else None,
            "mean_distance": metric(athlete, "distance"),
            "mean_jump_points": metric(athlete, "points"),
            "mean_total": metric(athlete, "total"),
            "global_fall_percent": 100 * case["falls"] / case["jumps"] if case["jumps"] else None,
            "wind_distance_correlation": case.get("windDistanceCorrelation"),
            "wind_points_correlation": case.get("windPointsCorrelation"),
            "paired_place_delta": paired.get("mean"),
            "paired_place_ci": [paired.get("ciLow"), paired.get("ciHigh")],
            "paired_n": paired.get("count"),
        }
        rows.append(row)
        if not index:
            continue
        prefix = case["name"] + ": "
        low, high = paired.get("ciLow"), paired.get("ciHigh")
        if low is not None and high is not None:
            if high < 0:
                notes.append(prefix + f"średnie miejsce poprawiło się o {formatted(-paired['mean'])}; 95% przedział różnicy [{formatted(low)}, {formatted(high)}].")
            elif low > 0:
                notes.append(prefix + f"średnie miejsce pogorszyło się o {formatted(paired['mean'])}; 95% przedział różnicy [{formatted(low)}, {formatted(high)}].")
            else:
                notes.append(prefix + "przedział różnicy miejsc obejmuje zero — próba nie rozstrzyga kierunku zmiany.")
        changed = [key for key, value in case["parameters"].items() if value != base["parameters"].get(key)]
        if len(changed) > 1:
            notes.append(prefix + "zmieniono wiele czynników (" + ", ".join(changed) + "); nie przypisuj całego efektu jednemu.")
        if case["jumps"] and base["jumps"]:
            extra = 100 * (case["falls"] / case["jumps"] - base["falls"] / base["jumps"])
            if abs(extra) >= 1:
                notes.append(prefix + f"udział upadków zmienił się o {extra:+.2f} p.p. względem bazy (opis próby, bez osobnego testu istotności).")
        distance_corr, points_corr = case.get("windDistanceCorrelation"), case.get("windPointsCorrelation")
        if distance_corr is not None and points_corr is not None and distance_corr * points_corr < 0:
            notes.append(prefix + "wiatr ma korelacje o przeciwnych znakach z odległością i notą. Sprawdź osobno rekompensatę w kontrolowanym wariancie.")
    return {"source": str(path), "experiment": report["config"]["name"],
            "engine_build": report.get("engineBuild"), "completed": report["completed"],
            "canceled": report["canceled"], "focus_index": focus, "rows": rows,
            "observations": notes, "checks": check_report(report)}


def markdown(summaries):
    lines = ["# Analiza playgroundu", "",
             "Różnice sparowane pochodzą wyłącznie z wariantów tego samego przebiegu. Porównanie osobnych raportów jest opisowe.", "",
             "Przedziały 95% są przybliżeniem dla niezależnych symulowanych konkursów. Nie dowodzą zgodności modelu z rzeczywistymi zawodami.", ""]
    for summary in summaries:
        lines += ["## " + summary["experiment"], "", f"Źródło: `{summary['source']}`. Ukończone powtórzenia: {summary['completed']}. Przerwano: {summary['canceled']}.", "",
                  "| Wariant | Śr. miejsce | Zwycięstwa | Śr. odległość | Śr. nota skoku | Upadki całej stawki | Δ miejsca |", "| --- | ---: | ---: | ---: | ---: | ---: | ---: |"]
        for row in summary["rows"]:
            name = row["variant"].replace("|", "\\|").replace("\n", " ")
            lines.append(f"| {name} | {formatted(row['mean_place'])} | {formatted(row['win_percent'])}% | {formatted(row['mean_distance'])} m | {formatted(row['mean_jump_points'])} pkt | {formatted(row['global_fall_percent'])}% | {formatted(row['paired_place_delta'])} |")
        lines += ["", "Skoczek: " + summary["rows"][0]["athlete"] + ".", ""]
        lines += ["- " + note for note in summary["observations"]]
        lines += ["", "Spójność danych: " + ("OK" if not summary["checks"] else "; ".join(summary["checks"])), ""]
    lines += ["Histogramy i surowe próbki są w report.json; CSV zawiera agregaty i szczegóły. Suma punktów może obejmować różną liczbę skoków i wagi serii. Korelacja jest opisowa; efekt parametru badaj przez warianty.", ""]
    return "\n".join(lines)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("reports", nargs="+", help="report.json files or report directories")
    parser.add_argument("--focus", type=int, help="zero-based athlete index; default: configured focus")
    parser.add_argument("--json", action="store_true", help="machine-readable summary")
    parser.add_argument("--output", type=Path, help="write summary to a new file")
    args = parser.parse_args()
    try:
        summaries = [summarize(path, report, args.focus) for path, report in map(load_report, args.reports)]
        content = json.dumps(summaries, ensure_ascii=False, indent=2, allow_nan=False) if args.json else markdown(summaries)
        if args.output:
            with args.output.open("x", encoding="utf-8") as file:
                file.write(content)
        else:
            print(content)
        return 1 if any(summary["checks"] for summary in summaries) else 0
    except (OSError, ValueError, KeyError, TypeError) as error:
        parser.error(str(error))


if __name__ == "__main__":
    raise SystemExit(main())
