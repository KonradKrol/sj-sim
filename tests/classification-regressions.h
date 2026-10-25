#pragma once
#include "../source/UI/EditorWidgets/ClassificationEditorWidget.h"
#include "../source/UI/Seasons/Classifications/ClassificationResultsTableModel.h"
#include <QJsonArray>

static void qualificationSourceRegressions(){
    SimulationSave save;save.setHills({new Hill("Qualifier","POL",100,110)});save.setCompetitionRules({CompetitionRules()});auto *calendar=new SeasonCalendar("Mixed sources");auto *teams=new Classification("Teams"),*individual=new Classification("Individual");teams->setClassificationType(Classification::Team);calendar->setClassifications({teams,individual});for(int kind:{0,0,1,1}){auto *event=new CompetitionInfo(save.getHillsReference().first());event->getRulesPointer()->setCompetitionType(kind);calendar->getCompetitionsReference().append(event);}Season season;season.setSeasonNumber(2026);season.setCalendars({calendar});season.setActualCalendar(calendar);save.setSeasons({season});save.setActualSeason(&save.getSeasonsReference().first());CalendarEditorTableModel model(calendar,&save.getHillsReference(),&save.getCompetitionRulesReference());CalendarEditorWidget editor(&model,&calendar->getClassificationsReference());editor.setSave(&save);QuickCalendar backend(&editor,&editor);const auto events=calendar->getCompetitions();backend.select(events[1]->getIDStr());check(backend.advancementClassifications()==QStringList{"Brak","Individual"},"individual source picker hides team classifications even when they are first in storage");backend.update({{"advancementClass",1}});check(events[1]->getAdvancementClassification()==individual,"filtered class index resolves to the actual compatible class");backend.select(events[3]->getIDStr());check(backend.advancementClassifications()==QStringList{"Brak","Teams"} && backend.advancementCompetitions().size()==2,"team source pickers expose only compatible prior competition and ranking");backend.update({{"advancement",1}});check(events[3]->getAdvancementCompetition()==events[2],"filtered competition index resolves to the compatible predecessor");events[1]->setAdvancementCompetition(events[2]);calendar->fixAdvancementCompetitions();backend.select(events[1]->getIDStr());check(backend.advancementCompetitions().size()==2 && !backend.advancementCompetitions().join(" ").contains("oczekuje"),"incompatible pending qualification is kept out of the choices");backend.select(events[3]->getIDStr(),Qt::ControlModifier);check(backend.advancementClassifications()==QStringList{"Brak"},"mixed bulk selection cannot assign an incompatible classification");
}

static void classificationScoringRegressions()
{
    qualificationSourceRegressions();
    const auto oldLocale=QLocale();for(const auto &locale:{QLocale(QLocale::Polish,QLocale::Poland),QLocale(QLocale::English,QLocale::UnitedStates)}){QLocale::setDefault(locale);auto text=classificationPointsText(1234.10000000001);text.remove(locale.groupSeparator());check(text=="1234"+QString(locale.decimalPoint())+"1" && classificationPointsText(-.00000001)=="0"+QString(locale.decimalPoint())+"0","classification formatting localizes decimals and removes floating point noise");}QLocale::setDefault(oldLocale);

    Jumper a("A","Junior","POL"),b("B","Senior","POL"),c("C","Veteran","POL"),d("D","Unknown","NOR");
    a.setBirthDate(QDate(2008,12,31));b.setBirthYear(2004);c.setBirthYear(1990);
    CompetitionInfo first,second,third;
    auto add=[](CompetitionInfo &event,Jumper *jumper,int place,double points){
        CompetitionSingleResult result(&event,jumper);result.setPosition(place);result.setPointsSum(points);
        event.getResultsReference().getResultsReference().append(result);
    };
    for(auto *event:{&first,&second,&third}){
        add(*event,&a,1,300);add(*event,&b,2,290);add(*event,&c,31,200);add(*event,&d,40,100);
        event->setPlayed(true);
    }
    Classification classification("Variable scoring");classification.setPointsForPlaces({{1,100},{2,80}});
    ClassificationScoringMethod top40;top40.name="Top 40";top40.points={{1,50},{2,40},{31,8},{40,1}};
    ClassificationScoringMethod winner20;winner20.name="20 for winner";winner20.points={{1,20},{2,10}};
    auto options=classification.getOptions();options.methods={top40,winner20};classification.setOptions(options);
    ClassificationCompetitionSettings selection;selection.methodId=top40.id;second.setScoringFor(&classification,selection);
    selection.methodId=winner20.id;third.setScoringFor(&classification,selection);
    ClassificationSingleResult ar(&classification,&a),br(&classification,&b),cr(&classification,&c),dr(&classification,&d);
    for(auto *row:{&ar,&br,&cr,&dr}){row->setCompetitionsResults({&first.getResultsReference(),&second.getResultsReference(),&third.getResultsReference()});row->updateSingleResults();row->updatePointsSum();}
    check(ar.getPointsSum()==170 && br.getPointsSum()==130 && cr.getPointsSum()==8 && dr.getPointsSum()==1,"each competition chooses an independent classification scoring method");
    classification.setResults({&ar,&br,&cr,&dr});classification.sortInDescendingOrder();
    options.contextParameters=true;classification.setOptions(options);
    selection.parameters={{"multiplier",2.5},{"points-limit",1}};third.setScoringFor(&classification,selection);
    ar.updatePointsSum();br.updatePointsSum();check(ar.getPointsSum()==200 && br.getPointsSum()==120,"optional context multiplier and scoring-place limit");
    options.contextParameters=false;classification.setOptions(options);ar.updatePointsSum();check(ar.getPointsSum()==170,"classification can disable competition parameter overrides");
    options.ageFilter=true;options.minimumAge=18;options.maximumAge=22;options.referenceYear=2026;classification.setOptions(options);
    for(auto *row:{&ar,&br,&cr,&dr})row->updatePointsSum();classification.sortInDescendingOrder();
    check(classification.getRankedResults().size()==2 && ar.getPointsSum()==170 && br.getPointsSum()==130 && cr.getPointsSum()==0 && dr.getPointsSum()==0,"inclusive annual age filter excludes older and unknown-age athletes");
    ClassificationResultsTableModel table(&classification);check(table.rowCount()==2,"age-ineligible athletes do not appear in the classification table");
    CompetitionInfo qualification;RoundInfo qualifyingRound;qualifyingRound.setCount(1);qualification.getRulesPointer()->setRounds({qualifyingRound});QVector<Jumper *> entrants{&a,&b,&c,&d};
    check(IndividualCompetitionManager::getFilteredJumpersByClassification(&qualification,&classification,entrants)==QVector<Jumper *>{&a},"age-excluded position-zero rows never qualify through a classification");
    options.includeUnknownAge=true;classification.setOptions(options);dr.updatePointsSum();classification.sortInDescendingOrder();table.setClassification(&classification);check(table.rowCount()==3,"unknown ages can be explicitly included");
    options.ageFilter=false;options.countrySelection=1;options.countryCount=2;classification.setOptions(options);
    for(auto *row:{&ar,&br,&cr,&dr})row->updatePointsSum();classification.sortInDescendingOrder();
    table.setClassification(&classification);check(table.rowCount()==4 && classification.getRankedResults().contains(&cr),"individual classification ignores legacy country selection");
    qualification.getRulesPointer()->getRoundsReference().first().setCount(10);
    check(IndividualCompetitionManager::getFilteredJumpersByClassification(&qualification,&classification,entrants).contains(&c),"country selection does not block individual qualification");
    options.countrySelection=2;options.countryCount=1;classification.setOptions(options);classification.sortInDescendingOrder();
    table.setClassification(&classification);check(table.rowCount()==4 && classification.getRankedResults().contains(&cr) && classification.getRankedResults().contains(&ar),"legacy bottom-N option does not filter an individual ranking");
    options.countrySelection=1;options.countryCount=2;classification.setOptions(options);
    ar.setPointsSum(10);br.setPointsSum(10);cr.setPointsSum(10);classification.sortInDescendingOrder();
    const auto tied=classification.getRankedResults();check(tied.size()==4 && tied.contains(&ar) && tied.contains(&br) && tied.contains(&cr),"individual ties are unaffected by a legacy country quota");
    cr.setPointsSum(20);classification.sortInDescendingOrder();check(classification.getRankedResults().contains(&cr),"a previously hidden athlete can return to the ranking");

    Classification nations("Nations");nations.setClassificationType(Classification::Team);nations.setPointsForPlaces({{1,400}});nations.setAltPointsForPlaces({{1,50},{2,40},{31,30},{40,20}});
    auto country=nations.getOptions();country.countrySelection=1;country.countryCount=2;nations.setOptions(country);
    ClassificationSingleResult pol(&nations,QString("POL"));pol.setCompetitionsResults({&first.getResultsReference()});pol.updateSingleResults();pol.updatePointsSum();check(pol.getPointsSum()==90,"nations classification sums only N best individual results");
    country.countryExclude=true;nations.setOptions(country);pol.updatePointsSum();check(pol.getPointsSum()==30,"excluding TOP N leaves only the other country results in that event");
    country.countrySelection=2;nations.setOptions(country);pol.updatePointsSum();check(pol.getPointsSum()==50,"excluding LAST N retains the country's best remaining result");
    country.countryCount=100;nations.setOptions(country);pol.updatePointsSum();check(pol.getPointsSum()==0,"excluding more athletes than the country entered yields no contribution");country.countryExclude=false;nations.setOptions(country);pol.updatePointsSum();check(pol.getPointsSum()==120,"including a quota larger than the field includes all country athletes");country.countryCount=2;
    country.countrySelection=2;nations.setOptions(country);pol.updatePointsSum();check(pol.getPointsSum()==70,"nations classification can sum N worst individual results");
    first.getResultsReference().getResultsReference()[2].setPosition(2);pol.updatePointsSum();check(pol.getPointsSum()==80,"equal places still contribute exactly N individual results");
    first.getResultsReference().getResultsReference()[2].setPosition(31);
    CompetitionInfo teamEvent;CompetitionRules teamRules;teamRules.setCompetitionType(CompetitionRules::Team);teamEvent.setRules(teamRules);
    Team team("POL");team.setJumpers({&a,&b});CompetitionSingleResult teamResult(&teamEvent,&team,CompetitionSingleResult::TeamResult);teamResult.setPosition(1);teamResult.setPointsSum(600);teamEvent.getResultsReference().getResultsReference().append(teamResult);
    country.countryExclude=true;country.countryCount=100;nations.setOptions(country);pol.setCompetitionsResults({&teamEvent.getResultsReference()});pol.setSingleResults({});pol.updatePointsSum();check(pol.getPointsSum()==400,"country exclusion cannot suppress team-event points");country.countryExclude=false;country.countryCount=2;nations.setOptions(country);
    pol.setCompetitionsResults({&first.getResultsReference(),&teamEvent.getResultsReference()});pol.updateSingleResults();pol.updatePointsSum();check(pol.getPointsSum()==470,"team-event points and selected individual-event points are added together");
    country.ageFilter=true;country.minimumAge=18;country.maximumAge=22;country.referenceYear=2026;nations.setOptions(country);
    pol.updatePointsSum();check(pol.getPointsSum()==490,"age filter is applied before the country selection");
    team.setJumpers({&a,&c});pol.updatePointsSum();check(pol.getPointsSum()==90,"a team containing an ineligible athlete cannot receive team-event points");
    pol.setCompetitionsResults({&teamEvent.getResultsReference()});pol.setSingleResults({});pol.updatePointsSum();nations.setResults({&pol});nations.sortInDescendingOrder();QVector<Team> teamEntrants{team};
    check(TeamCompetitionManager::getFilteredTeamsByClassification(&qualification,&nations,teamEntrants).isEmpty(),"age-excluded teams do not qualify via their hidden position-zero rows");
    classification.setAltPointsForPlaces({{1,12}});classification.setPunctationType(Classification::PointsForPlaces);
    options.ageFilter=false;options.countrySelection=0;classification.setOptions(options);team.setJumpers({&a,&b});
    ClassificationSingleResult teamMember(&classification,&a);teamMember.setCompetitionsResults({&teamEvent.getResultsReference()});teamMember.updateSingleResults();teamMember.updatePointsSum();check(teamMember.getPointsSum()==12,"individual classification uses athlete points for the place of their team");

    Hill ageHill;Jumper young("Fixed","Age","POL"),future("Future","Birth","POL");young.setBirthYear(2008);future.setBirthYear(2030);Classification ageClass("Year-stable age");ageClass.setPointsForPlaces({{1,100}});auto fixed=ageClass.getOptions();fixed.ageFilter=true;fixed.minimumAge=18;fixed.maximumAge=18;fixed.referenceYear=1999;fixed.contextParameters=true;fixed.includeUnknownAge=true;ageClass.setOptions(fixed);
    CompetitionInfo december(&ageHill),january(&ageHill);for(auto *event:{&december,&january}){add(*event,&young,1,100);add(*event,&future,2,50);event->setPlayed(true);event->setClassifications({&ageClass});}december.setScheduledDate(QDate(2026,12,31));january.setScheduledDate(QDate(2027,1,1));ClassificationCompetitionSettings overrideYear;overrideYear.parameters={{"reference-year",2099}};january.setScoringFor(&ageClass,overrideYear);
    SeasonCalendar ageCalendar;ageCalendar.setCompetitions({&december,&january});ageCalendar.setClassifications({&ageClass});Season fixedSeason;fixedSeason.setSeasonNumber(2026);fixedSeason.setCalendars({&ageCalendar});fixedSeason.setActualCalendar(&ageCalendar);check(ageClass.getOptions().referenceYear==2026 && ageClass.contributions(&december).size()==1 && ageClass.contributions(&january).size()==1,"age eligibility remains fixed across New Year and cannot be overridden per competition");
    ClassificationSingleResult ageResult(&ageClass,&young);ageResult.setCompetitionsResults({&december.getResultsReference(),&january.getResultsReference()});ageResult.updateSingleResults();ageResult.updatePointsSum();ageClass.setResults({&ageResult});ageClass.sortInDescendingOrder();check(ageResult.getPointsSum()==200,"cross-year participation retains all classification points");
    IdentifiableObjectsStorage yearStorage;yearStorage.add(&ageHill);yearStorage.add(&young);yearStorage.add(&future);auto loadedSeason=Season::getFromJson(Season::getJsonObject(fixedSeason),&yearStorage);auto *loadedAge=loadedSeason.getCalendars().first()->getClassifications().first();check(loadedAge->getOptions().referenceYear==2026 && loadedAge->getResults().size()==1 && loadedAge->getResults().first()->getPointsSum()==200 && loadedAge->contributions(loadedSeason.getCalendars().first()->getCompetitions().last()).size()==1,"season reload binds its year before recomputing classification contributions");for(auto *calendar:loadedSeason.getCalendars()){qDeleteAll(calendar->getCompetitionsReference());for(auto *entry:calendar->getClassificationsReference())qDeleteAll(entry->getResultsReference());qDeleteAll(calendar->getClassificationsReference());delete calendar;}
    fixedSeason.setSeasonNumber(2027);check(ageClass.contributions(&january).isEmpty(),"a new season re-evaluates the athlete's annual age");

    Classification definition;definition.copyConfiguration(classification);definition.setResults({});
    IdentifiableObjectsStorage storage;std::unique_ptr<Classification> restored(Classification::getFromJson(Classification::getJsonObject(&definition),&storage));
    check(restored->getOptions().toJson()==definition.getOptions().toJson() && restored->getAltPointsForPlaces()==definition.getAltPointsForPlaces(),"all scoring methods and filters survive classification serialization");
    auto legacy=Classification::getJsonObject(&definition);legacy.remove("scoring-options");std::unique_ptr<Classification> old(Classification::getFromJson(legacy,&storage));
    check(old->getOptions().methods.isEmpty() && !old->getOptions().ageFilter && old->getOptions().countrySelection==0,"legacy classification defaults preserve existing scoring");

    SeasonCalendar calendar("Scoring calendar");calendar.setCompetitions({&first,&second,&third});calendar.setClassifications({&classification});
    for(auto *event:calendar.getCompetitionsReference())event->setClassifications({&classification});
    options.countrySelection=0;classification.setOptions(options);
    const auto archive=classification.constructJumpersArchiveResults(nullptr,&calendar);check(archive.value(&c).value(&second)==3,"classification history uses the selected competition method");
    SeasonCalendarPreset preset(&calendar);
    auto *presetClass=preset.getCalendarReference().getClassificationsReference().first();
    auto *presetEvent=preset.getCalendarReference().getCompetitionsReference()[1];
    check(presetClass!=&classification && presetClass->getResultsReference().isEmpty() && classification.getResultsReference().size()==4,"calendar preset contains independent classification definitions without live standings");
    check(presetEvent->scoringFor(presetClass).methodId==top40.id && !presetEvent->getPlayed() && first.getPlayed(),"preset remaps method assignments and removes history without modifying the source calendar");
    IdentifiableObjectsStorage presetStorage;auto loaded=SeasonCalendarPreset::getFromJson(SeasonCalendarPreset::getJsonObject(preset),&presetStorage);
    auto *loadedClass=loaded.getCalendarReference().getClassificationsReference().first();
    auto *loadedEvent=loaded.getCalendarReference().getCompetitionsReference()[1];
    check(loadedClass->getOptions().toJson()==classification.getOptions().toJson() && loadedEvent->getClassificationsReference().contains(loadedClass) && loadedEvent->scoringFor(loadedClass).methodId==top40.id,"calendar preset round trip includes classification methods, filters and per-event assignment");
    qDeleteAll(preset.getCalendarReference().getCompetitionsReference());qDeleteAll(preset.getCalendarReference().getClassificationsReference());
    qDeleteAll(loaded.getCalendarReference().getCompetitionsReference());qDeleteAll(loaded.getCalendarReference().getClassificationsReference());
    qInfo()<<"PASS: variable classification scoring, context parameters, annual age, exact top/bottom country limits and calendar presets";
}

static void classificationTablePerformanceRegression()
{
    const int count=600;QVector<Jumper> jumpers;QVector<ClassificationSingleResult> results;jumpers.reserve(count);results.reserve(count);
    Classification classification("Large filtered ranking");auto options=classification.getOptions();options.countrySelection=1;options.countryCount=count;classification.setOptions(options);
    for(int i=0;i<count;++i){jumpers.append(Jumper(QString::number(i),"Athlete","POL"));results.append(ClassificationSingleResult(&classification,&jumpers.last()));results.last().setPointsSum(count-i);}
    for(auto &row:results)classification.getResultsReference().append(&row);classification.sortInDescendingOrder();
    ClassificationResultsTableModel model(&classification);QAbstractItemModelTester contract(&model,QAbstractItemModelTester::FailureReportingMode::Fatal);
    QElapsedTimer timer;timer.start();int cells=0;
    const int roles[]={Qt::DisplayRole,Qt::DecorationRole,Qt::FontRole,Qt::BackgroundRole,Qt::ForegroundRole,Qt::TextAlignmentRole};
    for(int row=0;row<model.rowCount();++row)for(int column=0;column<model.columnCount();++column){for(int role:roles)model.data(model.index(row,column),role);++cells;}
    check(cells==count*3 && timer.elapsed()<5000,"a filtered classification with hundreds of athletes renders without repeated ranking work");
    options.countryCount=1;classification.setOptions(options);classification.sortInDescendingOrder();model.setClassification(&classification);check(model.rowCount()==count && model.data(model.index(0,1),Qt::DisplayRole).toString()==jumpers.first().getNameAndSurname(),"individual ranking retains every athlete after an obsolete country-limit edit");
    model.setClassification(nullptr);check(model.rowCount()==0 && !model.data(QModelIndex()).isValid(),"clearing the classification clears cached rows");
    qInfo()<<"PASS: filtered classification table benchmark"<<cells<<"cells in"<<timer.elapsed()<<"ms including refresh";
}

static void classificationContributionUiRegressions(MainWindow &main)
{
    Hill hill;Jumper a("Anna","One","POL"),b("Beata","Two","POL");Classification classification("Country contributions");classification.setClassificationType(Classification::Team);classification.setPointsForPlaces({{1,400}});classification.setAltPointsForPlaces({{1,100},{2,50}});auto settings=classification.getOptions();settings.countrySelection=1;settings.countryCount=1;classification.setOptions(settings);
    CompetitionInfo first(&hill),second(&hill),teamEvent(&hill);Team team("POL");team.setJumpers({&a,&b});CompetitionRules teamRules;teamRules.setCompetitionType(CompetitionRules::Team);teamEvent.setRules(teamRules);
    for(auto *event:{&first,&second})for(int i=0;i<2;++i){auto *jumper=event==&first?(i?&b:&a):(i?&a:&b);CompetitionSingleResult result(event,jumper);result.setPosition(i+1);result.setPointsSum(200-i*10);event->getResultsReference().getResultsReference().append(result);}
    CompetitionSingleResult teamResult(&teamEvent,&team,CompetitionSingleResult::TeamResult);teamResult.setPosition(1);teamResult.setPointsSum(500);teamEvent.getResultsReference().getResultsReference().append(teamResult);
    for(auto *event:{&first,&second,&teamEvent}){event->setPlayed(true);event->setClassifications({&classification});}
    ClassificationSingleResult country(&classification,QString("POL"));country.setCompetitionsResults({&first.getResultsReference()});country.updateSingleResults();country.updatePointsSum();check(country.getPointsSum()==100,"first competition awards its own best country athlete");country.setCompetitionsResults({&first.getResultsReference(),&second.getResultsReference(),&teamEvent.getResultsReference()});country.updateSingleResults();country.updatePointsSum();check(country.getPointsSum()==600,"a different best athlete in the next event cannot remove previously earned points");classification.setResults({&country});classification.sortInDescendingOrder();
    SeasonCalendar calendar;calendar.setCompetitions({&first,&second,&teamEvent});calendar.setClassifications({&classification});QDialog holder(&main);auto *layout=new QVBoxLayout(&holder);auto *view=new ClassificationResultsTableView(false,&classification,&holder);layout->addWidget(view);view->setHistoryCalendar(&calendar);view->fillTable();holder.resize(800,600);holder.show();QTest::qWait(100);
    auto *rankingBridge=holder.findChild<QuickBridge *>();auto *rankingNode=qobject_cast<QuickNode *>(rankingBridge->find("tableView"));check(rankingNode,"classification ranking table is bridged");auto *rankingAdapter=qobject_cast<QuickTableModel *>(rankingNode->values().value("table").value<QObject *>());rankingAdapter->select(0,1,Qt::ControlModifier);rankingAdapter->selectAll();QTest::qWait(40);auto *ranking=item(scene(&holder)->rootObject(),"dataTable-"+rankingNode->values().value("id").toString());check(view->getTableView()->selectionModel()->selectedIndexes().isEmpty() && !rankingAdapter->multiple() && ranking && !ranking->property("searchOpen").toBool() && !ranking->property("sortable").toBool(),"classification standings have no selection actions or sorting and start with collapsed search");
    for(int checkpoint:{0,1}){
        view->findChild<QComboBox *>("comboBox_classificationHistory")->setCurrentIndex(checkpoint);QTest::qWait(60);bool opened=false;int ticks=0;QTimer watcher;
        QObject::connect(&watcher,&QTimer::timeout,[&]{check(++ticks<250,"country breakdown opens without getting stuck");for(auto *widget:QApplication::allWidgets())if(auto *dialog=qobject_cast<QDialog *>(widget);dialog && dialog->objectName()=="classificationContributionDetails" && dialog->isVisible()){
            if(!scene(dialog) || scene(dialog)->status()!=QQuickWidget::Ready)continue;watcher.stop();opened=true;auto *table=dialog->findChild<QTableView *>("classificationContributionTable");check(table && table->model()->rowCount()==(checkpoint?1:3),"country breakdown separates individual athletes from direct team-event points");auto *quickTable=item(scene(dialog)->rootObject(),"classificationContributionResults");auto *flag=item(scene(dialog)->rootObject(),"classificationContributionFlag");check(quickTable && !quickTable->property("searchEnabled").toBool() && flag && flag->isVisible() && !flag->property("source").toUrl().isEmpty(),"contribution page has a visible country flag and no search controls");double total=0;for(int r=0;r<table->model()->rowCount();++r)total+=table->model()->index(r,2).data(Qt::UserRole).toDouble();check(total==(checkpoint?100:600),"country contribution breakdown exactly reconciles the selected historical total");dialog->reject();return;
        }});watcher.start(20);QMetaObject::invokeMethod(view->getTableView(),"doubleClicked",Q_ARG(QModelIndex,view->getTableView()->model()->index(0,1)));watcher.stop();check(opened,"double-clicking a country shows the Quick contribution breakdown even in overview mode");
    }
    holder.hide();qInfo()<<"PASS: per-competition country quota preserves past points and country double-click reconciles all/history contribution totals";
}

static void classificationUiRegressions(MainWindow &main)
{
    SimulationSave save;save.setName("Classification test");
    {NewSeasonConfiguratorWindow setup;save.setJumpers(setup.getJumpersReference());save.setHills(setup.getHillsReference());save.setCompetitionRules(setup.getCompetitionsRulesReference());}
    auto *calendar=new SeasonCalendar("Classification calendar");auto *event=new CompetitionInfo(save.getHillsReference().first());event->setRules(save.getCompetitionRulesReference().first());calendar->setCompetitions({event});
    auto *classification=new Classification("Nations");classification->setClassificationType(Classification::Team);classification->setPointsForPlaces({{1,400},{2,350}});classification->setAltPointsForPlaces({{1,100},{2,80}});calendar->setClassifications({classification});event->setClassifications({classification});
    Season season;season.setSeasonNumber(2026);season.setCalendars({calendar});season.setActualCalendar(calendar);save.setSeasons({season});save.setActualSeason(&save.getSeasonsReference().first());save.fixJumpersFormInstabilities();save.updateNextCompetitionIndex();
    SimulationSaveManagerWindow manager(&save,&main);manager.fillNextCompetitionInformations();render(&manager,"classification-manager");
    auto *bridge=manager.findChild<QuickBridge *>();auto *backend=bridge->findChild<QuickCalendar *>();check(backend,"calendar backend exists");
    bridge->edit(bridge->root()->values().value("id").toInt(),4);QTest::qWait(120);
    auto *workspace=item(scene(&manager)->rootObject(),"calendarWorkspace");check(workspace,"calendar workspace is visible");
    workspace->setProperty("section",1);QTest::qWait(100);
    auto *classes=item(scene(&manager)->rootObject(),"calendarClassesWorkspace");check(classes,"classification workspace exists");classes->setProperty("editing",true);
    backend->selectClass(classification->getIDStr());auto *editor=backend->classificationEditor();
    auto *custom=item(scene(&manager)->rootObject(),"classificationWorkspace");check(custom && !custom->property("multipleMethods").toBool(),"basic classification editor hides named scoring methods");
    clickControl(scene(&manager),"classificationMultipleMethods");QTest::qWait(40);check(custom->property("multipleMethods").toBool(),"named scoring controls are revealed only on request");
    clickControl(scene(&manager),"classificationMultipleMethods");QTest::qWait(40);
    clickControl(scene(&manager),"classificationSource-1");QTest::qWait(40);
    auto *visiblePoints=item(scene(&manager)->rootObject(),"classificationPointsRows");check(visiblePoints && visiblePoints->property("model").value<QObject *>(),"one visible points table serves both scoring sources");
    auto *alternativeAdapter=qobject_cast<QuickTableModel *>(visiblePoints->property("model").value<QObject *>());check(alternativeAdapter && alternativeAdapter->rowCount()==2,"alternate scoring source exposes its point table");
    alternativeAdapter->edit(0,1,125.5);QTest::qWait(100);check(classification->getAltPointsForPlaces().value(1)==125.5 && classification->getPointsForPlaces().value(1)==400,"editing the displayed alternate source persists without changing primary points");
    clickControl(scene(&manager),"classificationSource-0");QTest::qWait(40);
    const auto originalPoints=classification->getPointsForPlaces();auto *presetChoice=editor->findChild<QComboBox *>("comboBox_presets");if(presetChoice->count()>1){clickControl(scene(&manager),"classificationPointsPreset");QTest::keyClick(scene(&manager),Qt::Key_End);QTest::keyClick(scene(&manager),Qt::Key_Return);QTest::qWait(100);const int selected=presetChoice->currentIndex();auto *presetControl=item(scene(&manager)->rootObject(),"classificationPointsPreset");check(selected>0 && presetControl->property("displayText").toString()==presetChoice->currentText(),"selected scoring preset is shown by name");const auto selectedTable=classification->getPointsForPlaces();QMetaObject::invokeMethod(presetControl,"activated",Q_ARG(int,0));QTest::qWait(60);check(presetChoice->currentIndex()==selected && classification->getPointsForPlaces()==selectedTable,"custom preset state cannot be selected to erase a real preset");auto *adapter=qobject_cast<QuickTableModel *>(visiblePoints->property("model").value<QObject *>());adapter->edit(0,1,selectedTable.value(1)+.123456789);QTest::qWait(100);check(presetChoice->currentIndex()==0 && presetControl->property("displayText").toString()==QString::fromUtf8("Własne"),"editing preset points switches the displayed preset to custom");qobject_cast<PointsForPlacesTableModel *>(editor->findChild<QTableView *>("tableView_pointsForPlaces")->model())->replacePoints(originalPoints);QTest::qWait(100);}
    auto reveal=[&](const char *name){auto *target=item(scene(&manager)->rootObject(),name);auto *scroll=item(scene(&manager)->rootObject(),"calendarClassEditorScroll");auto *flick=qobject_cast<QQuickItem *>(scroll->property("contentItem").value<QObject *>());check(target&&flick,"classification filter navigation exists");const qreal y=target->mapToItem(flick,QPointF()).y()+flick->property("contentY").toReal();flick->setProperty("contentY",qBound(0.,y-20.,qMax(0.,flick->property("contentHeight").toReal()-flick->height())));QTest::qWait(60);};
    reveal("classificationAdvancedToggle");clickControl(scene(&manager),"classificationAdvancedToggle");QTest::qWait(60);
    reveal("classificationCountryLimitToggle");clickControl(scene(&manager),"classificationCountryLimitToggle");QTest::qWait(100);check(classification->getOptions().countrySelection==1 && item(scene(&manager)->rootObject(),"classificationCountryLimit")->isVisible(),"country toggle enables the limit and immediately saves its mode");
    clickControl(scene(&manager),"classificationCountryAction");QTest::keyClick(scene(&manager),Qt::Key_End);QTest::keyClick(scene(&manager),Qt::Key_Return);QTest::qWait(100);check(classification->getOptions().countryExclude,"country exclusion mode persists through its visible selector");
    clickControl(scene(&manager),"classificationCountryLimitToggle");QTest::qWait(100);check(classification->getOptions().countrySelection==0 && !item(scene(&manager)->rootObject(),"classificationCountryLimit")->isVisible(),"country toggle restores all athletes and hides irrelevant count");
    clickControl(scene(&manager),"choice-comboBox_type");QTest::keyClick(scene(&manager),Qt::Key_Home);QTest::keyClick(scene(&manager),Qt::Key_Return);QTest::qWait(100);check(classification->getClassificationType()==Classification::Individual && !item(scene(&manager)->rootObject(),"classificationCountryLimitToggle")->isVisible(),"country filtering is not offered for an individual classification");clickControl(scene(&manager),"choice-comboBox_type");QTest::keyClick(scene(&manager),Qt::Key_End);QTest::keyClick(scene(&manager),Qt::Key_Return);QTest::qWait(100);
    reveal("classificationAgeToggle");clickControl(scene(&manager),"classificationAgeToggle");QTest::qWait(100);check(classification->getOptions().ageFilter,"age filter toggle persists through the custom UI");clickControl(scene(&manager),"classificationAgeToggle");QTest::qWait(100);
    for(auto *action:editor->actions())check(action->shortcut().isEmpty(),"legacy classification editing shortcuts are removed");
    auto *pointsModel=editor->findChild<QTableView *>("tableView_pointsForPlaces")->model();auto *gridAdapter=qobject_cast<QuickTableModel *>(visiblePoints->property("model").value<QObject *>());QAbstractItemModelTester gridContract(gridAdapter,QAbstractItemModelTester::FailureReportingMode::Fatal);QSignalSpy gridResets(gridAdapter,&QAbstractItemModel::modelReset);pointsModel->insertRows(2,80);QTest::qWait(120);visiblePoints=item(scene(&manager)->rootObject(),"classificationPointsRows");check(visiblePoints->property("columns").toInt()>1,"scoring uses multiple compact columns instead of a full-width row per place");const auto addAt=controlPoint(scene(&manager),"classificationPointAdd");visiblePoints->setProperty("contentY",64.);auto *classScroll=item(scene(&manager)->rootObject(),"calendarClassEditorScroll");auto *outer=classScroll->property("contentItem").value<QObject *>();const qreal outerY=outer->property("contentY").toReal(),gridY=visiblePoints->property("contentY").toReal();QTest::mouseClick(scene(&manager),Qt::LeftButton,Qt::NoModifier,addAt);QTest::qWait(150);check(gridResets.count()==0 && pointsModel->rowCount()==83 && qAbs(visiblePoints->property("contentY").toReal()-gridY)<2 && qAbs(outer->property("contentY").toReal()-outerY)<2,QString("adding a scored place preserves inner and outer scroll positions: count %1, grid %2 -> %3, outer %4 -> %5").arg(pointsModel->rowCount()).arg(gridY).arg(visiblePoints->property("contentY").toReal()).arg(outerY).arg(outer->property("contentY").toReal()));pointsModel->removeRows(2,81);QTest::qWait(100);
    check(editor->findChild<QLabel *>("label_altPreset")->text().contains(QString::fromUtf8("indywidualnych")),"team classification explains its individual-result scoring");
    auto *help=editor->findChild<QPushButton *>("classificationAlternativeHelp");check(help && help->text()=="?" && help->toolTip().contains(QString::fromUtf8("Konkurs indywidualny")) && help->toolTip().contains(QString::fromUtf8("Pucharze Narodów")),"question-mark help explains the selected source using the team-classification mechanism");
    reveal("classificationMultipleMethods");clickControl(scene(&manager),"classificationMultipleMethods");QTest::qWait(40);clickControl(scene(&manager),"classificationNewMethod");QTest::qWait(80);
    check(classification->getOptions().methods.size()==1,"new scoring method is saved by the calendar editor");
    clickControl(scene(&manager),"classificationMethod-1");QTest::keyClick(scene(&manager),Qt::Key_A,Qt::ControlModifier);QTest::keyClicks(scene(&manager),"Winner 20");QTest::keyClick(scene(&manager),Qt::Key_Tab);QTest::qWait(100);check(classification->getOptions().methods.first().name=="Winner 20","selected scoring method is renamed inline without a separate field");
    auto *points=editor->findChild<QTableView *>("tableView_pointsForPlaces");check(points->model()->setData(points->model()->index(0,1),20.),"edit method point table");
    editor->findChild<QCheckBox *>("classificationAdvanced")->setChecked(true);editor->findChild<QCheckBox *>("classificationContextParameters")->setChecked(true);
    editor->findChild<QCheckBox *>("classificationAgeFilter")->setChecked(true);editor->findChild<QSpinBox *>("classificationMinimumAge")->setValue(18);editor->findChild<QSpinBox *>("classificationMaximumAge")->setValue(30);
    editor->findChild<QComboBox *>("classificationCountrySelection")->setCurrentIndex(1);QMetaObject::invokeMethod(editor->findChild<QComboBox *>("classificationCountrySelection"),"activated",Q_ARG(int,1));editor->findChild<QSpinBox *>("classificationCountryCount")->setValue(2);QTest::qWait(100);
    const auto options=classification->getOptions();check(options.contextParameters && options.ageFilter && options.minimumAge==18 && options.maximumAge==30 && options.countrySelection==1 && options.countryCount==2 && options.methods.first().points.value(1)==20,"filters and custom table edits persist without an extra submit");
    backend->select(event->getIDStr());backend->setClassificationScoring(0,{{"method",options.methods.first().id},{"multiplier",0.5}});
    check(event->scoringFor(classification).methodId==options.methods.first().id && event->scoringFor(classification).parameters.value("multiplier").toDouble()==0.5,"competition chooses the method and optional context parameters");
    backend->undo();check(event->scoringFor(classification).methodId.isEmpty(),"undo restores the previous competition scoring settings");backend->redo();check(event->scoringFor(classification).methodId==options.methods.first().id,"redo restores the scoring method");
    main.resize(1440,1000);workspace->setProperty("section",0);workspace->setProperty("editing",true);QTest::qWait(100);
    auto *inspector=item(scene(&manager)->rootObject(),"calendarInspector");check(inspector,"event inspector exists");inspector->setProperty("section",2);QTest::qWait(100);
    check(!item(scene(&manager)->rootObject(),"calendar-parameter-reference-year-0") && !item(scene(&manager)->rootObject(),"calendar-parameter-points-limit-0"),"competition settings expose the multiplier without obsolete context inputs");
    clickControl(scene(&manager),"calendar-scoring-method-0");QTest::keyClick(scene(&manager),Qt::Key_Home);QTest::keyClick(scene(&manager),Qt::Key_Return);QTest::qWait(100);
    check(event->scoringFor(classification).methodId.isEmpty(),"visible competition method selector chooses default");
    clickControl(scene(&manager),"calendar-scoring-method-0");QTest::keyClick(scene(&manager),Qt::Key_End);QTest::keyClick(scene(&manager),Qt::Key_Return);QTest::qWait(100);
    check(event->scoringFor(classification).methodId==options.methods.first().id,"visible competition selector chooses the named method");
    clickControl(scene(&manager),"input-classification-parameter-multiplier-0");QTest::keyClick(scene(&manager),Qt::Key_A,Qt::ControlModifier);
    QTest::keyClicks(scene(&manager),QLocale().toString(.25,'f',2));QTest::keyClick(scene(&manager),Qt::Key_Tab);QTest::qWait(100);
    check(event->scoringFor(classification).parameters.value("multiplier").toDouble()==0.25,"visible context field commits the edited multiplier");
    backend->setClassificationScoring(0,{{"multiplier",0.5}});workspace->setProperty("section",1);QTest::qWait(80);classes->setProperty("editing",true);
    for(const auto size:{QSize(1440,900),QSize(1024,720),QSize(520,850),QSize(360,800)}){main.resize(size);QTest::qWait(150);presentation(&manager)->grab().save(output+QString("/classification-editor-%1.png").arg(size.width()));}
    // The real preset buttons must preserve the same definition and method IDs.
    const auto oldPresets=GlobalDatabase::get()->getGlobalCalendarPresets();
    QTimer::singleShot(200,[]{for(auto *w:QApplication::topLevelWidgets())if(auto *input=qobject_cast<QInputDialog *>(w);input && input->isVisible()){input->setTextValue("Classification preset test");input->accept();return;}check(false,"preset name prompt");});
    backend->preset(true);QTest::qWait(100);
    auto &presets=GlobalDatabase::get()->getEditableGlobalCalendarPresets();check(presets.size()==oldPresets.size()+1,"saving through the UI creates a calendar preset");
    auto &preset=presets.last();auto *pc=preset.getCalendarReference().getClassificationsReference().first();check(pc->getOptions().methods.first().points.value(1)==20 && preset.getCalendarReference().getCompetitionsReference().first()->scoringFor(pc).methodId==options.methods.first().id,"UI preset includes classification definitions and selected scoring method");
    QTimer::singleShot(200,[]{for(auto *w:QApplication::topLevelWidgets())if(auto *input=qobject_cast<QInputDialog *>(w);input && input->isVisible()){auto *combo=input->findChild<QComboBox *>();check(combo,"preset choice");combo->setCurrentIndex(combo->count()-1);input->accept();return;}check(false,"preset selection prompt");});
    backend->preset(false);QTest::qWait(150);
    auto *lc=calendar->getClassificationsReference().first();auto *le=calendar->getCompetitionsReference().first();check(lc->getOptions().toJson()==options.toJson() && le->getClassificationsReference().contains(lc) && le->scoringFor(lc).methodId==options.methods.first().id && le->scoringFor(lc).parameters.value("multiplier").toDouble()==0.5,"UI calendar preset loading restores classification options and context after reassigning IDs");
    GlobalDatabase::get()->setGlobalCalendarPresets(oldPresets);manager.hide();main.resize(1024,640);
    classificationContributionUiRegressions(main);
    qInfo()<<"PASS: classification editor, dynamic alternative labels, per-event methods, parameter undo and real calendar preset save/load";
}
