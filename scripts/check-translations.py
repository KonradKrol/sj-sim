#!/usr/bin/env python3
"""Report incomplete Qt translations and broken substitution arguments."""
import collections
import pathlib
import re
import sys
import xml.etree.ElementTree as ET

root = pathlib.Path(__file__).resolve().parent.parent
failed = False
catalog_keys = {}
for language in ("en", "cs"):
    tree = ET.parse(root / "translations" / f"translation_{language}.ts")
    missing = []
    mismatched = []
    catalog_keys[language] = set()
    if not tree.getroot().get('language', '').startswith(language):
        print(f'{language}: incorrect TS language metadata')
        failed = True
    for context in tree.getroot().findall("context"):
        for message in context.findall("message"):
            source = message.findtext("source") or ""
            catalog_keys[language].add((context.findtext('name'),source,message.findtext('comment') or ''))
            translation = message.find("translation")
            if (translation is None or translation.get("type") == "unfinished"
                    or not "".join(translation.itertext()).strip()):
                missing.append((context.findtext("name"), source))
                continue
            expected = collections.Counter(re.findall(r"%(?:L?\d+|n)", source))
            forms = translation.findall("numerusform") or [translation]
            for form in forms:
                actual = collections.Counter(re.findall(r"%(?:L?\d+|n)", "".join(form.itertext())))
                if expected != actual:
                    mismatched.append((context.findtext("name"), source))
    print(f"{language}: {len(missing)} unfinished; {len(mismatched)} with mismatched placeholders")
    for context, source in mismatched:
        print(f"  {context}: {source!r}")
    failed |= bool(missing or mismatched)
if catalog_keys['en'] != catalog_keys['cs']:
    print('English and Czech catalogs contain different source keys')
    failed = True
sys.exit(1 if failed else 0)
