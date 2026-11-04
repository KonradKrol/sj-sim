#pragma once
static void teamWeekendRegressions() {
    CompetitionRules rules;rules.setCompetitionType(CompetitionRules::Team);rules.setJumpersInTeamCount(2);rules.setRounds({RoundInfo(10,true)});
    CompetitionInfo main,qualifier,trial,training,other;
    for(auto *event:{&main,&qualifier,&trial,&training,&other})event->setRules(rules);
    qualifier.setSerieType(CompetitionInfo::Qualifications);trial.setSerieType(CompetitionInfo::TrialRound);training.setSerieType(CompetitionInfo::Training);
    main.setAdvancementCompetition(&qualifier);main.setTrialRound(&trial);main.setTrainings({&training});
    Jumper first("A","One","POL"),second("B","Two","POL");Team squad("POL");squad.setJumpers({&second,&first});
    SeasonCalendar calendar;calendar.setCompetitions({&training,&qualifier,&trial,&main,&other});
    training.setTeams({squad});training.setPlayed(true);
    check(!calendar.teamSquadLock(&main) && calendar.previousTeamTraining(&main)==&training,"team trainings suggest squads without locking competition entries");
    qualifier.setTeams({squad});qualifier.setPlayed(true);
    check(calendar.teamSquadLock(&main)==&qualifier && calendar.teamSquadLock(&trial)==&qualifier,"team qualifications lock the entire linked competition and trial");
    check(!calendar.teamSquadLock(&training) && !calendar.teamSquadLock(&other),"training and unrelated events remain editable");
    qualifier.setCancelled(true);check(!calendar.teamSquadLock(&main),"cancelled qualifiers do not establish a squad lock");
    qualifier.setPlayed(false);qualifier.setCancelled(false);trial.setTeams({squad});trial.setPlayed(true);
    check(calendar.teamSquadLock(&main)==&trial && trial.getTeams().first().getJumpers()==QVector<Jumper *>({&second,&first}),"the first played team trial locks the exact member order");
    {
        GlobalDatabase::get()->ensurePacks();
        SimulationSave fixture;fixture.setEngineVersion(1);fixture.setHills({new Hill("Planning","POL",120,134)});fixture.setCompetitionRules({rules});
        auto *plan=new SeasonCalendar("Undated planning");Season season;season.setSeasonNumber(2026);season.setCalendars({plan});season.setActualCalendar(plan);fixture.setSeasons({season});fixture.setActualSeason(&fixture.getSeasonsReference().first());
        CalendarEditorTableModel model(plan,&fixture.getHillsReference(),&fixture.getCompetitionRulesReference());CalendarEditorWidget editor(&model,&plan->getClassificationsReference());editor.setSave(&fixture);QuickCalendar planner(&editor,&editor);
        check(!planner.datesEnabled(),"undated season fixture");planner.previewWeekend("",0,0,false,false);
        check(planner.weekendPreview().size()==3 && planner.applyWeekend(),"competition packages can be planned without dates");
        int competitions=0,qualifications=0;
        for(auto *event:plan->getCompetitions()) {
            check(!event->getScheduledDate().isValid(),"undated planning never stores its internal date anchor");
            if(event->getSerieType()==CompetitionInfo::Competition){++competitions;check(event->getAdvancementCompetition() && event->getAdvancementCompetition()->getRules().getCompetitionType()==CompetitionRules::Team,"team competitions receive team qualification links");}
            if(event->getSerieType()==CompetitionInfo::Qualifications)++qualifications;
            if(event->getSerieType()==CompetitionInfo::TrialRound)check(event->getRules().getCompetitionType()==CompetitionRules::Team && event->getRules().getJumpersInTeamCount()==2,"planned team trials use the main event team size");
        }
        check(competitions==2 && qualifications==2,"undated package contains both competitions and their qualifiers");
    }
    qInfo()<<"PASS: team event families, fixed squads, training suggestions and undated planning";
}
