#!/usr/bin/env python3
"""Refresh the Markdown release manifest from a concrete Git snapshot."""
from collections import defaultdict
from pathlib import Path
import argparse
import re
import subprocess

ROOT = Path(__file__).resolve().parents[1]
REPORT = ROOT / 'docs/RELEASE_NOTES_VS_MAIN.md'


def git(*args):
    return subprocess.check_output(['git', '-c', 'core.quotepath=false', *args], cwd=ROOT, text=True)


def category(path):
    if path.startswith('.agents/'):
        return 'Instrukcje i zasoby narzędzi deweloperskich'
    if path.startswith('source/UI/Competition/Quick/'):
        return 'Wcześniejszy opcjonalny eksperyment konkursowy'
    if path.startswith('source/UI/Quick/') or path == 'quick-ui.qrc':
        return 'Domyślny interfejs Qt Quick i jego zasoby'
    if path.startswith('scripts/'):
        return 'Skrypty budowania, uruchamiania i testowania'
    if path.startswith('tests/'):
        return 'Testy'
    if path.startswith('translations/'):
        return 'Tłumaczenia'
    if path.startswith('docs/') or path.endswith('.md'):
        return 'Dokumentacja etapów i migracji'
    if path.startswith('source/'):
        return 'Kontrolery, modele i logika C++'
    return 'Konfiguracja projektu i pozostałe pliki'


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--base', default='main')
    parser.add_argument('--snapshot', default='HEAD')
    args = parser.parse_args()
    if git('branch', '--show-current').strip() == 'main':
        raise SystemExit('Run this on the feature branch, not main.')
    base = git('rev-parse', args.base).strip()
    snapshot = git('rev-parse', args.snapshot).strip()
    body = REPORT.read_text().split('\n## Załącznik A.', 1)[0]
    body = re.sub(r'Stan porównania kodu i dokumentacji: \*\*[^*]+\*\*',
                  f'Stan porównania kodu i dokumentacji: **{snapshot[:7]}**', body, count=1)
    statuses = dict(line.split('\t', 1)[::-1] for line in git('diff', '--no-renames', '--name-status', base, snapshot).splitlines())
    stats = {}
    for line in git('diff', '--no-renames', '--numstat', base, snapshot).splitlines():
        added, removed, path = line.split('\t', 2)
        stats[path] = (added, removed)
    history = []
    for record in git('log', '--reverse', '--format=%H%x1f%h%x1f%s%x1f%b%x1e', f'{base}..{snapshot}').split('\x1e'):
        if record.strip():
            history.append(record.strip('\r\n').split('\x1f', 3))
    groups = defaultdict(list)
    for path in statuses:
        groups[category(path)].append(path)
    appendix = '\n## Załącznik A. Dokładny zakres Git\n\n'
    appendix += f'- Baza: **{base}**.\n- Snapshot kodu i dokumentacji: **{snapshot}**.\n'
    appendix += f'- Commitów od bazy: **{len(history)}**.\n- Plików zmienionych w tym zakresie: **{len(statuses)}**.\n'
    appendix += '- Statystyki obejmują także fonty, skills i dokumentację; nie oznaczają rozmiaru samej logiki gry.\n'
    appendix += '- Dalsza aktualizacja tego zestawienia jest dodatkiem dokumentacyjnym do wskazanego snapshotu.\n\n'
    appendix += '| Obszar | Plików |\n| --- | ---: |\n'
    for group, paths in sorted(groups.items()):
        appendix += f'| {group} | {len(paths)} |\n'
    appendix += '\nPorównanie można odtworzyć poleceniami:\n\n~~~sh\n'
    appendix += f'git diff --stat {base} {snapshot}\ngit diff --name-status {base} {snapshot}\ngit log --reverse --oneline {base}..{snapshot}\n~~~\n'
    appendix += '\nZestawienie można odświeżyć przez `python3 scripts/update-release-inventory.py --base main --snapshot HEAD`. Opisy zachowania i wyniki testów należy uzupełniać na podstawie faktycznych zmian oraz weryfikacji.\n'
    appendix += '\n## Załącznik B. Wszystkie commity\n\nHistoryczne opisy należy czytać z uwzględnieniem opisanych powyżej rozwiązań zastąpionych podczas prac.\n\n'
    appendix += '| Commit | Zmiana i zapisane uzasadnienie/weryfikacja |\n| --- | --- |\n'
    for _, short, title, description in history:
        detail = title + (' — ' + ' '.join(description.split()) if description.strip() else '')
        appendix += f'| `{short}` | {detail.replace(chr(124), "&#124;")} |\n'
    appendix += '\n## Załącznik C. Pełny manifest zmienionych plików\n\nA: dodany, M: zmieniony, D: usunięty. +/− to liczby linii tekstu; kreska oznacza zasób binarny.\n\n'
    for group, paths in sorted(groups.items()):
        appendix += f'### {group}\n\n| Status | Plik | + | − |\n| --- | --- | ---: | ---: |\n'
        for path in sorted(paths):
            label = f'[{path}](../{path})' if (ROOT / path).exists() else f'`{path}`'
            added, removed = stats[path]
            appendix += f'| {statuses[path]} | {label} | {added} | {removed} |\n'
        appendix += '\n'
    result = body + appendix
    for path in statuses:
        assert path in result, path
    for _, short, _, _ in history:
        assert f'`{short}`' in result, short
    for target in re.findall(r'\]\(([^)]+)\)', result):
        if not target.startswith(('https://', 'http://')):
            assert (REPORT.parent / target).exists(), target
    REPORT.write_text(result.rstrip() + "\n")
    print(f'{len(history)} commits, {len(statuses)} changed files; snapshot {snapshot[:7]}')


if __name__ == '__main__':
    main()
