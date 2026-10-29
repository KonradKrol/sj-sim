#!/usr/bin/env python3
"""One-time, backed-up conversion of Sj.Sim's former three-level perk data.

Close Sj.Sim first. Without --apply this only lists files to convert.
Pass exported packs, the application-data directory, and/or simulationSaves.
The game itself only accepts the current four-level format.
"""
import argparse
import copy
import json
import math
import os
from pathlib import Path
import re
import shutil
import tempfile

OLD_COEFFICIENTS = {
    'specialistBias': [0, 5, 9, 14], 'trainingBonus': [0, 1.5, 3, 5],
    'competitionPenalty': [0, .75, 1.5, 2.5],
    'conservedEffortPenalty': [0, 3, 5.5, 8],
    'conservedEffortDeviation': [1, 1.15, 1.35, 1.6],
    'regularNegative': [1, .8, .6, .4], 'regularPositive': [1, .92, .84, .76],
    'unpredictableNegative': [1, 1.2, 1.45, 1.75],
    'unpredictablePositive': [1, 1.07, 1.15, 1.25],
}


def profile(value):
    return (isinstance(value, dict) and isinstance(value.get('perks'), list)
            and 'id' in value and ('overall' in value or ('k' in value and 'hs' in value)))


def experiment(value):
    return (isinstance(value, dict) and value.get('schema') == 2
            and isinstance(value.get('variants'), list) and 'athletes' in value)


def balance(value):
    return (isinstance(value, dict) and value.get('schema') == 1
            and isinstance(value.get('values'), dict) and 'name' in value)


def needs_conversion(value):
    if isinstance(value, list):
        return any(map(needs_conversion, value))
    if not isinstance(value, dict):
        return False
    if (profile(value) or experiment(value) or balance(value)) and value.get('perkLevels') != 4:
        return True
    return any(needs_conversion(v) for v in value.values())


def resample(old):
    result = []
    for level in range(5):
        x = level * 3 / 4
        lo, hi = math.floor(x), min(3, math.ceil(x))
        result.append(round(old[lo] + (old[hi] - old[lo]) * (x - lo), 12))
    return result


def remap_level(value):
    return 4 if value == 3 else -4 if value == -3 else value


def convert(value, old_scale=True):
    if isinstance(value, list):
        return [convert(v, old_scale) for v in value]
    if not isinstance(value, dict):
        return value
    if profile(value) and value.get('perkLevels') == 4:
        return copy.deepcopy(value)
    if (experiment(value) or balance(value)) and value.get('perkLevels') == 4:
        old_scale = False
    result = {k: convert(v, old_scale) for k, v in value.items()}
    if profile(value):
        result['perks'] = [remap_level(v) for v in value['perks']]
        if 'k' in value and len(result['perks']) == 3:
            result['perks'].append(0)
        result['perkLevels'] = 4
    if experiment(value) or balance(value):
        result['perkLevels'] = 4
    for key, v in value.items():
        if old_scale and (re.fullmatch(r'(athlete|field|hill)\.perk\.[0-3]', key) or ('index' in value and re.fullmatch(r'perk[0-3]', key))):
            result[key] = remap_level(v)
    curves = {'quality.' + key: points for key, points in OLD_COEFFICIENTS.items()}
    curves['generator.auto.headProbability'] = [.5, .70, .84, .94]
    for prefix, defaults in curves.items():
        if not old_scale or not any(prefix + '.' + str(i) in value for i in range(4)):
            continue
        old = [value.get(prefix + '.' + str(i), defaults[i]) for i in range(4)]
        if not all(not isinstance(v,bool) and isinstance(v, (float, int)) and math.isfinite(v) for v in old):
            raise ValueError('Non-numeric perk coefficient: ' + prefix)
        for i, v in enumerate(resample(old)):
            result[prefix + '.' + str(i)] = v
    return result


def convert_file(path, apply=False):
    original = path.read_bytes()
    data = json.loads(original)
    if not needs_conversion(data):
        return False
    updated = convert(data)
    if needs_conversion(updated):
        raise ValueError('Incomplete conversion: ' + str(path))
    if apply:
        backup = path.with_name(path.name + '.three-levels.bak')
        if backup.exists():
            raise FileExistsError('Backup already exists; original left untouched: ' + str(backup))
        shutil.copy2(path, backup)
        temporary = None
        try:
            with tempfile.NamedTemporaryFile(mode='w', encoding='utf-8', dir=path.parent, delete=False) as f:
                temporary = f.name
                json.dump(updated, f, ensure_ascii=False, indent=2)
                f.write('\n')
                f.flush()
                os.fsync(f.fileno())
            os.chmod(temporary, path.stat().st_mode)
            os.replace(temporary, path)
        finally:
            if temporary and os.path.exists(temporary):
                os.unlink(temporary)
    return True


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('paths', nargs='*', type=Path)
    parser.add_argument('--app-data', action='store_true', help='Include standard Sj.Sim application-data and workspace save directories')
    parser.add_argument('--apply', action='store_true', help='Write converted files, keeping .three-levels.bak copies')
    args = parser.parse_args()
    if args.app_data:
        home=Path.home()
        roots=[home / '.local/share/SJ-Sim/SJ-Sim', home / '.local/share/SJ-Sim', home / 'Library/Application Support/SJ-Sim']
        for variable in ('APPDATA', 'LOCALAPPDATA'):
            if os.environ.get(variable):roots.append(Path(os.environ[variable]) / 'SJ-Sim')
        repo=Path(__file__).resolve().parent.parent
        roots += [repo / folder / 'simulationSaves' for folder in ('', 'build-quick-ui', 'build-win', 'build-win/release')]
        args.paths += [path for path in roots if path.exists()]
    if not args.paths:
        parser.error('Specify a data path or use --app-data when Sj.Sim data exists.')
    count = errors = 0
    files = set()
    for root in args.paths:
        if not root.exists():
            parser.error('Path does not exist: ' + str(root))
        files.update(root.rglob('*.json') if root.is_dir() else [root])
    for path in sorted(files):
        try:
            if convert_file(path, args.apply):
                count += 1
                print(('Converted: ' if args.apply else 'Would convert: ') + str(path))
        except (OSError, ValueError) as error:
            errors += 1
            print(str(path) + ': ' + str(error))
    print(f'{count} files; {errors} errors. ' + ('Backups preserved.' if args.apply else 'No files changed. Use --apply to convert.'))
    raise SystemExit(bool(errors))


if __name__ == '__main__':
    main()
