#pragma once
#include "../source/UI/EditorWidgets/ConfigSelections.h"

static void configDefaultsRegressions(MainWindow &main) {
    auto *database=GlobalDatabase::get();
    check(!database->getEditableGlobalJumpers().isEmpty() && !database->getEditableGlobalHills().isEmpty() && !database->getEditableCompetitionRules().isEmpty(),"required selection fixtures exist");
    SingleJumpsConfigWindow single;
    auto *jumper=single.findChild<QComboBox *>("comboBox_existingJumper"),*hill=single.findChild<QComboBox *>("comboBox_existingHill");
    check(jumper->count()==database->getEditableGlobalJumpers().size() && hill->count()==database->getEditableGlobalHills().size(),"single jumps offer only real jumper and hill entries");
    check(jumper->currentIndex()==0 && hill->currentIndex()==0,"single jumps default to first available jumper and hill");
    check(single.getJumperEditor()->getJumperFromWidgetInput().getNameAndSurname()==database->getEditableGlobalJumpers().first().getNameAndSurname(),"default jumper is applied to inputs");
    check(single.getHillEditor()->getHillFromWidgetInput().getName()==database->getEditableGlobalHills().first().getName() && single.getWindsGeneratorSettingsEditor()->getKPoint()==database->getEditableGlobalHills().first().getKPoint(),"default hill and wind generator are synchronized");
    jumper->setCurrentIndex(jumper->count()-1);hill->setCurrentIndex(hill->count()-1);
    check(single.getJumperEditor()->getJumperFromWidgetInput().getNameAndSurname()==database->getEditableGlobalJumpers().last().getNameAndSurname() && single.getHillEditor()->getHillFromWidgetInput().getName()==database->getEditableGlobalHills().last().getName(),"last entries map correctly without a placeholder offset");
    jumper->setCurrentIndex(-1);hill->setCurrentIndex(-1);check(jumper->currentIndex()==0 && hill->currentIndex()==0,"populated required selectors cannot remain empty");
    render(&single,"config-default-single-jumps",true);single.hide();
    CompetitionConfigWindow competition(CompetitionConfigWindow::SingleCompetition,&main);
    hill=competition.findChild<QComboBox *>("comboBox_existingHill");auto *rules=competition.findChild<QComboBox *>("comboBox_existingCompetitionRules");
    check(hill && rules && hill->currentIndex()==0 && rules->currentIndex()==0 && hill->count()==database->getEditableGlobalHills().size() && rules->count()==database->getEditableCompetitionRules().size(),"competition defaults to real hill and rules without NONE entries");
    check(competition.getHillEditor()->getHillFromWidgetInput().getName()==database->getEditableGlobalHills().first().getName() && competition.getCompetitionRulesEditor()->getCompetitionRulesFromWidgetInputs().getName()==database->getEditableCompetitionRules().first().getName(),"competition applies default hill and rules to the editor");
    hill->setCurrentIndex(hill->count()-1);check(competition.getHillEditor()->getHillFromWidgetInput().getName()==database->getEditableGlobalHills().last().getName(),"competition's last hill maps correctly");hill->setCurrentIndex(-1);rules->setCurrentIndex(-1);check(hill->currentIndex()==0 && rules->currentIndex()==0,"competition required selectors cannot remain empty");
    render(&competition,"config-default-competition",true);competition.hide();
    QComboBox presets;QDoubleSpinBox importance;importance.setRange(0,100);importance.setDecimals(3);
    JumpsImportancePreset first,second;first.setName("First");first.setJumpsImportance(3.5);second.setName("Second");second.setJumpsImportance(6.75);
    ConfigSelections::importancePresets(&presets,&importance,{first,second});check(presets.currentIndex()==0 && importance.value()==3.5,"first importance preset is selected and applied by default");
    presets.setCurrentIndex(1);check(importance.value()==6.75,"importance preset data remains correctly indexed");importance.setValue(4.125);check(presets.currentIndex()==2 && !presets.currentData().isValid(),"manual importance uses explicit custom-settings entry");presets.setCurrentIndex(-1);check(presets.currentIndex()==0 && importance.value()==3.5,"importance cannot remain unselected");
    QComboBox empty;QDoubleSpinBox custom;custom.setValue(5);ConfigSelections::importancePresets(&empty,&custom,{});check(empty.count()==1 && empty.currentIndex()==0 && custom.value()==5,"empty preset library retains valid manual settings");
    qInfo()<<"PASS: required configuration defaults, first and last mappings, nonempty selectors and explicit custom settings";
}
