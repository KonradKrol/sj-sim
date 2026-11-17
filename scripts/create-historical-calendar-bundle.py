#!/usr/bin/env python3
"""Rebuild the owner's bundle from reviewed FIS facts; does not change their library."""
import argparse,copy,json,uuid
from pathlib import Path
P=argparse.ArgumentParser();P.add_argument('input',type=Path);P.add_argument('output',type=Path);a=P.parse_args()
root=Path(__file__).resolve().parents[1];facts=root/'data/community/historical-calendars';magic=b'SJSIM-PACK\n1\n'
raw=a.input.read_bytes();assert raw.startswith(magic);bundle=json.loads(raw[len(magic):])
def uid(key):return str(uuid.uuid5(uuid.NAMESPACE_URL,'sjsim-historical-fis-2023-2025/'+key))
rules={r['name']:r for resource in bundle['resources'] if resource['kind']=='Rules' for r in resource['document']['competitionsRules']}
# All referenced venues already exist in the supplied hill pack (historical Trondheim = HS138).
profiles=next(r for r in bundle['resources'] if r['kind']=='Hills')['document']['profiles']
existing={(h['name'],h['hs']) for h in profiles}
def venue(e,r):
 name=e['place'].rsplit(' (',1)[0].strip();name={'Ruka':'Kuusamo','Wisla':'Wisła','Kulm Bad Mitterndorf/Tauplitz':'Kulm'}.get(name,name)
 if name=='Trondheim':hs=105 if 'Normal' in r['description'] else 138
 elif name=='Lillehammer':hs=98 if 'Normal' in r['description'] else 140
 elif name=='Oberstdorf':hs=235 if 'Flying' in r['description'] else 137
 else:hs={'Courchevel':132,'Szczyrk':104,'Rasnov':97,'Hinzenbach':90,'Klingenthal':140,'Kuusamo':142,'Engelberg':140,'Garmisch-Partenkirchen':142,'Innsbruck':128,'Bischofshofen':142,'Wisła':134,'Zakopane':140,'Kulm':235,'Willingen':147,'Lake Placid':128,'Sapporo':137,'Lahti':130,'Oslo':134,'Vikersund':240,'Planica':240,'Titisee-Neustadt':142}[name]
 assert (name,hs) in existing,(name,hs)
 return {'name':name,'hs-point':hs}
summary={};omitted=[]
for resource in bundle['resources']:
 if resource['kind']!='Calendars':continue
 name=resource['name'];doc=resource['document']['calendar-preset'];old=doc['calendar'];classes=old['classifications'];year=int(name[4:8] if name.startswith('Lato') else name[5:9]);summer=name.startswith('Lato')
 for c in classes:
  c['results']=[];c['scoring-options']['reference-year']=year
  if 'Nations' in c['name'] or c['name']=='SNC':c['classification-type']=1
 def cid(key):
  return next((c['id'] for c in classes if c['name'] in {'world':('SGP','Summer Grand Prix','WC','World Cup'),'nations':('SNC','Summer Nations Cup','Nations Cup'),'flying':('Flying WC',),'fht':('FHT',),'raw':('RA','Raw Air'),'planica':('P7','Planica 7'),'polishInd':('PT (Ind)',),'polishTeam':('PT (Team)',)}[key]),None)
 events=json.loads((facts/(name+'-FIS.json')).read_text())
 if not summer:events+=json.loads((facts/('MS2024-FIS.json' if year==2023 else 'MS2025-FIS.json')).read_text())
 records=[]
 def add(key,date,kind,hill,rule,source,championship=False):
  q=copy.deepcopy(rules[rule]);q['id']=uid(name+'/'+key+'/rules')
  c={'id':uid(name+'/'+key),'scheduled-date':date,'serie-type':kind,'rules':q,'played':False,'cancelled':False,'exceptional-rounds-count':0,'results':{'id':uid(name+'/'+key+'/results'),'results':[]},'rounds-ko-groups':[],'start-list':[],'teams':[],'training-ids':[],'classification-scoring':{},'classifications-ids':[],'start-list-sort-mode':0,'start-list-sort-classification':'','start-list-sort-competition':'','simulation-model-revision':2}
  records.append({'event':c,'hill':hill,'source':source,'championship':championship,'key':key});return c
 for e in events:
  championship=('Kulm' in e['place'] and not summer) or ('Trondheim' in e['place'] and year==2024 and not summer)
  races=[r for r in e['races'] if ('Individual' in r['description'] or 'Team' in r['description']) and not r['cancelled'] and ('Qualification' not in r['description'] or r.get('qualificationResults')) and ('Qualification' in r['description'] or r.get('officialResults'))]
  if not races:continue
  # Training rounds are taken from actual official training result sheets, not guessed from weekends.
  seen=set()
  for r in e['races']:
   training=r.get('training')
   if training and training['date'] and training['url'] not in seen:
    seen.add(training['url']);hill=venue(e,r)
    for n in range(training['rounds']):add('training/'+r['codex']+'/'+str(n),training['date'],3,hill,'Training/Trial',training['url'],championship)
  for r in races:
   hill=venue(e,r);desc=r['description'];qualification='Qualification' in desc;team='Team' in desc
   if qualification:rule='Qualifications'
   elif team:rule='Team Duos' if 'Super Team' in desc else 'Team Competition'
   elif championship:rule='Flying Championships' if hill['name']=='Kulm' else 'WC'
   elif hill['name'] in ('Oberstdorf','Garmisch-Partenkirchen','Innsbruck','Bischofshofen') and hill['hs-point']<200 and r['date'][5:7] in ('12','01') and not summer:rule='Classical KO'
   else:rule='Flying WC' if hill['hs-point']>=200 else 'WC'
   trial=r.get('trial')
   if not qualification and trial and trial['date']:add('trial/'+r['codex'],trial['date'],2,hill,'Team Training/Trial' if team else 'Training/Trial',trial['url'],championship)
   c=add('race/'+r['codex'],r['date'],1 if qualification else 0,hill,rule,r['results'],championship)
   if rule=='Flying Championships':c['rules']['rounds']=c['rules']['rounds'][:3] # Kulm 2024 ended after three rounds.
   if name=='Zima-2023-24' and r['codex']=='3185':
    template=copy.deepcopy(rules['WC']['rounds'][0]);c['rules']['rounds']=[dict(template,count=n,ko=False) for n in (30,20,10)]
   if not qualification and r.get('actualRounds'):c['rules']['rounds']=c['rules']['rounds'][:r['actualRounds']]
   if name=='Zima-2024-25' and r['codex']=='3190':c['rules']['rounds'][0]['count']=30
   # A qualifying round selects the size of the corresponding first round, including flying events.
   c['_time']=r.get('time','')
 records.sort(key=lambda z:(z['event']['scheduled-date'],0 if z['event']['serie-type']==3 else 1 if z['event']['serie-type']==1 else 2 if z['event']['serie-type']==2 else 3,z['event'].get('_time',''),z['key']))
 # Pair each trial with the following main event on that hill/date. Link practices to their next event.
 consumedQ=set()
 for i,item in enumerate(records):
  c=item['event'];h=item['hill'];kind=c['serie-type'];champ=item['championship'];isTeam=c['rules']['competition-type']==1
  if kind==0:
   trials=[z for z in records[:i] if z['event']['serie-type']==2 and z['hill']==h and z['event']['scheduled-date']==c['scheduled-date'] and z['event']['rules']['competition-type']==c['rules']['competition-type']]
   if trials:c['trial-round-id']=trials[-1]['event']['id']
   if not isTeam:
    qs=[z for z in records[:i] if z['event']['serie-type']==1 and z['hill']==h and z['event']['id'] not in consumedQ and ( __import__('datetime').date.fromisoformat(c['scheduled-date'])-__import__('datetime').date.fromisoformat(z['event']['scheduled-date'])).days<=3]
    if qs:
     q=qs[-1]['event'];c['advancement-competition-id']=q['id'];c['start-list-sort-mode']=1;consumedQ.add(q['id']);q['rules']['rounds'][0]['count']=c['rules']['rounds'][0]['count']
  if kind in (0,1):
   practice=[z for z in records[:i] if z['event']['serie-type']==3 and z['hill']==h and (__import__('datetime').date.fromisoformat(c['scheduled-date'])-__import__('datetime').date.fromisoformat(z['event']['scheduled-date'])).days<=3]
   c['training-ids']=[z['event']['id'] for z in practice]
  assigned=[]
  if kind==0 and not champ:
   if not isTeam:assigned.append(cid('world'))
   assigned.append(cid('nations'))
   if h['hs-point']>=200 and not isTeam:assigned.append(cid('flying'))
   if not summer and h['name'] in ('Oberstdorf','Garmisch-Partenkirchen','Innsbruck','Bischofshofen') and h['hs-point']<200 and c['scheduled-date'][5:7] in ('12','01'):assigned.append(cid('fht'))
  if not champ and kind in (0,1):
   if not summer and h['name'] in ('Oslo','Trondheim','Vikersund'):assigned.append(cid('raw'))
   if not summer and h['name']=='Planica':assigned.append(cid('planica'))
   if name=='Zima-2023-24' and h['name'] in ('Wisła','Szczyrk','Zakopane'):
    assigned.append(cid('polishTeam'))
    if not isTeam:assigned.append(cid('polishInd'))
  c['classifications-ids']=list(dict.fromkeys(x for x in assigned if x));c.pop('_time',None)
 # Trials inherit the exact participant list selected for their parent.
 # Runtime already handles that parent linkage; the preset stores it in trial-round-id.
 old['competitions']=[z['event'] for z in records];doc['hills']=[z['hill'] for z in records]
 assert all(a['scheduled-date']<=b['scheduled-date'] for a,b in zip(old['competitions'],old['competitions'][1:]))
 summary[name]={'events':len(records),'individual':sum(z['event']['serie-type']==0 and z['event']['rules']['competition-type']==0 for z in records),'team':sum(z['event']['serie-type']==0 and z['event']['rules']['competition-type']==1 for z in records),'first':records[0]['event']['scheduled-date'],'last':records[-1]['event']['scheduled-date']}
 # Keep an easy-to-read audited timetable beside the bundle.
 lines=['date,hill,HS,type,rules,source']
 import csv,io
 buffer=io.StringIO();writer=csv.writer(buffer);writer.writerow(['date','hill','HS','type','rules','source'])
 for z in records:writer.writerow([z['event']['scheduled-date'],z['hill']['name'],z['hill']['hs-point'],['Konkurs','Kwalifikacje','Próbna','Trening'][z['event']['serie-type']],z['event']['rules']['name'],z['source']])
 (facts/(name+'-terminarz.csv')).write_text(buffer.getvalue())
a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_bytes(magic+json.dumps(bundle,ensure_ascii=False,separators=(',',':')).encode());print(json.dumps(summary,ensure_ascii=False,indent=2))
