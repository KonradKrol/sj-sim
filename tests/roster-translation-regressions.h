#pragma once

static void rosterTranslationRegression() {
    JumpersListEditorWindow source;
    auto athlete=std::make_unique<Jumper>("Cached","Athlete","POL");
    source.setAllJumpers({athlete.get()});source.setSelectedJumpers({athlete.get()});
    QuickRoster roster(&source,nullptr);const auto cached=roster.ordered()->get(0);
    source.setSelectedJumpers({});source.setAllJumpers({});athlete.reset();
    QSignalSpy changed(&roster,&QuickRoster::changed);roster.invalidateText();
    check(changed.count()==1 && roster.ordered()->get(0)==cached,"translation invalidation uses cached values without reading retired athlete storage");
    roster.sync();check(roster.ordered()->rowCount()==0 && roster.library()->rowCount()==0,"explicit data synchronization still replaces retired roster values");
    qInfo()<<"PASS: translation refresh cannot dereference retired roster athletes";
}
