#!/usr/bin/env python3
"""Check one-time conversion, data preservation, backups and repeat safety."""
import sys
sys.dont_write_bytecode=True
import importlib.util
import json
from pathlib import Path
import tempfile

spec = importlib.util.spec_from_file_location('conversion', Path(__file__).with_name('convert-four-perk-levels.py'))
conversion = importlib.util.module_from_spec(spec)
spec.loader.exec_module(conversion)

with tempfile.TemporaryDirectory() as directory:
    file = Path(directory) / 'pack.json'
    old = {'format':'sj-sim-athlete-pack','profiles':[
        {'id':'athlete','overall':83,'form':57,'country':'POL','perks':[1,2,-3], 'photoPath':'POL_Test.png'},
        {'id':'current','perkLevels':4,'overall':76,'perks':[3,-4,0]}]}
    original = json.dumps(old).encode()
    file.write_bytes(original)
    assert conversion.convert_file(file) and file.read_bytes()==original
    assert conversion.convert_file(file, True)
    current = json.loads(file.read_text())
    assert current['profiles'][0]['perks']==[1,2,-4]
    assert current['profiles'][0]['overall']==83 and current['profiles'][0]['form']==57
    assert current['profiles'][0]['photoPath']=='POL_Test.png'
    assert current['profiles'][1]==old['profiles'][1]
    assert file.with_name('pack.json.three-levels.bak').read_bytes()==original
    assert not conversion.convert_file(file, True)
    experiment={'schema':2,'athletes':2,'variants':[{'changes':{'athlete.perk.2':3},'athleteEdits':[{'index':0,'perk1':-3}]}],
                'defaults':{'quality.regularNegative.0':1,'quality.regularNegative.1':.8,'quality.regularNegative.2':.6,'quality.regularNegative.3':.4}}
    converted=conversion.convert(experiment)
    assert converted['perkLevels']==4 and not conversion.needs_conversion(converted)
    assert converted['variants'][0]['changes']['athlete.perk.2']==4
    assert converted['variants'][0]['athleteEdits'][0]['perk1']==-4
    assert converted['defaults']['quality.regularNegative.2']==.7
    assert converted['defaults']['quality.regularNegative.4']==.4
    mixed={'perkLevels':4,'schema':2,'athletes':2,'variants':[{'changes':{'athlete.perk.2':3,'quality.regularNegative.2':.7}}],
           'roster':[{'simulation-v2':old['profiles'][0]}]}
    current=conversion.convert(mixed)
    assert current['variants']==mixed['variants']
    assert current['roster'][0]['simulation-v2']['perks']==[1,2,-4]
    assert not conversion.needs_conversion(current)
    mixed['labScenarios']={'old':experiment}
    nested=conversion.convert(mixed)
    assert nested['variants']==mixed['variants']
    assert nested['labScenarios']['old']['variants'][0]['changes']['athlete.perk.2']==4
    assert not conversion.needs_conversion(nested)
    invalid=Path(directory) / 'invalid.json'
    broken={'profiles':[{'id':'bad','overall':80,'perks':[0,0,5]}]}
    invalid.write_text(json.dumps(broken))
    before=invalid.read_bytes()
    try:
        conversion.convert_file(invalid,True)
        raise AssertionError('Invalid old levels must be rejected')
    except ValueError:
        pass
    assert invalid.read_bytes()==before and not invalid.with_name('invalid.json.three-levels.bak').exists()
    assert not conversion.needs_conversion({'hello':'unrelated data','perks':[3,3,3]})
print('PASS: four-level conversion preserves ratings, photos, sides, extremes, new data and exact backups; resampling and repeated runs are safe')
