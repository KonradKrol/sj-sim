#include "ResultsHistoryWidget.h"
#include "../Competition/JumperCompetitionResultsWidget.h"
#include "../../competitions/CompetitionInfo.h"
#include "../../competitions/CompetitionAdvancement.h"
#include "../../global/CountryFlagsManager.h"
#include <QComboBox>
#include <QDir>
#include "../../global/CountryCodes.h"
#include <QTreeView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSignalBlocker>
#include <QApplication>
#include <QAbstractButton>
#include <QAbstractSpinBox>
#include <algorithm>

CompetitionResults ResultsHistoryWidget::afterRound(const CompetitionResults &source,int round,bool individual,CompetitionInfo *owner) {
    CompetitionResults out=source;out.getResultsReference().clear();
    auto *competition=owner?owner:source.getCompetition();if(!competition && !source.getResults().isEmpty())competition=source.getResults().first().getCompetition();out.setCompetition(competition);
    const int size=competition?qMax(1,competition->getRules().getJumpersInTeamCount()):1;
    QHash<Jumper *,int> athletes;
    for(auto result:source.getResults()) {
        if(competition)result.setCompetition(competition);
        const bool team=result.getType()==CompetitionSingleResult::TeamResult;
        auto &jumps=result.getJumpsReference();
        if(round>0 && jumps.size()>round*(team?size:1))jumps.resize(round*(team?size:1));
        if(jumps.isEmpty())continue;
        if(individual && team) {
            for(const auto &jump:jumps) {
                auto *jumper=jump.getJumper();if(!jumper)continue;
                if(!athletes.contains(jumper)){athletes.insert(jumper,out.getResultsReference().size());out.getResultsReference().append(CompetitionSingleResult(competition,jumper));}
                out.getResultsReference()[athletes.value(jumper)].getJumpsReference().append(jump);
            }
        }else {
            result.updatePointsSum(round>0?qMin(round,result.getScoringRound()):result.getScoringRound());
            if(team)result.updateTeamJumpersResults();
            out.getResultsReference().append(result);
        }
    }
    if(individual) {
        for(auto &result:out.getResultsReference()){double sum=0;for(const auto &jump:result.getJumpsReference())sum+=jump.getPoints();result.setPointsSum(sum);}
        std::stable_sort(out.getResultsReference().begin(),out.getResultsReference().end(),[](const CompetitionSingleResult &a,const CompetitionSingleResult &b){return qRound64(a.getPointsSum()*10)>qRound64(b.getPointsSum()*10);});
        int place=0; qint64 previous=0;
        for(int i=0;i<out.getResultsReference().size();++i){auto &result=out.getResultsReference()[i];const auto points=qRound64(result.getPointsSum()*10);if(i==0 || points!=previous)place=i+1;result.setPosition(place);previous=points;}
    }else out.sortInDescendingOrder();
    for(auto &result:out.getResultsReference()){
        for(auto &jump:result.getJumpsReference()){jump.setCompetition(competition);jump.setSingleResult(&result);}
        for(auto &member:result.getTeamJumpersResultsReference())for(auto &jump:member.getJumpsReference()){jump.setCompetition(competition);jump.setSingleResult(&member);}
    }
    return out;
}
ResultsHistoryWidget::ResultsHistoryWidget(CompetitionInfo *competition,bool live,QWidget *parent)
    :QWidget(parent),competition(competition),live(live),owner(parent) {
    setObjectName("resultsHistory");
    auto *layout=new QVBoxLayout(this);layout->setContentsMargins(0,0,0,0);
    auto *controls=new QHBoxLayout;layout->addLayout(controls);
    rounds=new QComboBox(this);rounds->setObjectName("comboBox_resultHistoryRound");rounds->setAccessibleName(tr("Wyniki po serii"));controls->addWidget(rounds);
    mode=new QComboBox(this);mode->setObjectName("comboBox_resultHistoryMode");mode->addItems({tr("Wyniki drużynowe"),tr("Wyniki indywidualne")});mode->setVisible(competition->getRules().getCompetitionType()==CompetitionRules::Team);controls->addWidget(mode);
    model=new QStandardItemModel(this);table=new QTreeView(this);table->setObjectName("historyResultsView");table->setModel(model);table->setSelectionMode(QAbstractItemView::SingleSelection);table->setSelectionBehavior(QAbstractItemView::SelectRows);layout->addWidget(table,1);
    preview=new JumperCompetitionResultsWidget(this);preview->hide();layout->addWidget(preview);
    connect(table,&QTreeView::clicked,this,&ResultsHistoryWidget::showJump);
    connect(table,&QTreeView::doubleClicked,this,&ResultsHistoryWidget::showJump);
    const auto update=[this]{signature.clear();preview->hide();applyPause();if(!active())preview->setJumperResult(nullptr);refresh(completedLimit);emit changed();};
    connect(rounds,qOverload<int>(&QComboBox::currentIndexChanged),this,update);connect(mode,qOverload<int>(&QComboBox::currentIndexChanged),this,update);
    refresh();
}
bool ResultsHistoryWidget::individualRanking() const {return mode->isVisibleTo(const_cast<ResultsHistoryWidget *>(this)) && mode->currentIndex()==1;}
bool ResultsHistoryWidget::paused() const {return live && (rounds->currentData().toInt()>0 || individualRanking());}
bool ResultsHistoryWidget::active() const {return !live || paused() || individualRanking();}
void ResultsHistoryWidget::refresh(int completedRounds) {
    completedLimit=completedRounds;
    int count=0;const int size=competition->getRules().getCompetitionType()==CompetitionRules::Team?qMax(1,competition->getRules().getJumpersInTeamCount()):1;
    for(const auto &result:competition->getResultsReference().getResultsReference())count=qMax(count,int((result.getJumps().size()+size-1)/size));
    const int recorded=count;
    if(completedRounds>=0)count=qMin(count,completedRounds);
    if(live && count>0 && recorded==count)--count;
    if(rounds->count()!=count+1){const QSignalBlocker block(rounds);const int selected=rounds->currentData().toInt();rounds->clear();rounds->addItem(live?tr("Wyniki na żywo"):tr("Wszystkie serie"),0);for(int r=1;r<=count;++r)rounds->addItem(tr("Wyniki z %1 serii").arg(r),r);rounds->setCurrentIndex(qBound(0,selected,count));}
    available=count;table->setVisible(active());if(!active())preview->hide();
    applyPause();
    const QString key=competition->getResultsReference().getLatestJumpId()+QString("/%1/%2/%3/%4").arg(rounds->currentData().toInt()).arg(individualRanking()).arg(count).arg(competition->getResultsReference().getResultsReference().size());
    if(active() && key!=signature){rebuild();signature=key;}
}
void ResultsHistoryWidget::rebuild() {
    preview->hide();preview->setJumperResult(nullptr);
    snapshot=afterRound(competition->getResultsReference(),rounds->currentData().toInt(),individualRanking(),competition);
    auto *previousModel=model;
    auto *nextModel=new QStandardItemModel(this);
    const bool team=competition->getRules().getCompetitionType()==CompetitionRules::Team && !individualRanking();
    table->setProperty("historyTeam",team);
    int n=rounds->currentData().toInt();if(n==0){n=1;for(const auto &result:snapshot.getResultsReference()){const int size=result.getType()==CompetitionSingleResult::TeamResult?qMax(1,competition->getRules().getJumpersInTeamCount()):1;n=qMax(n,int((result.getJumps().size()+size-1)/size));}}
    QStringList headers=team?QStringList{tr("Poz."),tr("Drużyna"),tr("Zawodnik"),tr("Suma"),QString()}:QStringList{tr("Poz."),tr("Zawodnik"),tr("Suma")};
    for(int r=1;r<=n;++r){if(!team)headers.append(tr("%1 · m").arg(r));headers.append(tr("%1 · pkt").arg(r));}nextModel->setHorizontalHeaderLabels(headers);
    const auto row=[&](CompetitionSingleResult &result,int resultIndex,bool child){
        QStringList values=team?QStringList{child?QString():result.isTeamDisqualified()?QStringLiteral("DSQ"):QString::number(result.getPosition()),child?QString():result.getTeam()->getCountryCode(),child?result.getJumper()->getNameAndSurname():QString(),result.isTeamDisqualified()?QString::fromUtf8("—"):QString::number(result.getPointsSum(),'f',1),QString()}:QStringList{QString::number(result.getPosition()),result.getJumper()->getNameAndSurname(),QString::number(result.getPointsSum(),'f',1)};
        const int perRound=team&&!child?qMax(1,competition->getRules().getJumpersInTeamCount()):1;
        const auto &jumps=result.getJumpsReference();for(int r=0;r<n;++r){double points=0;bool has=false;for(int i=r*perRound;i<qMin((r+1)*perRound,int(jumps.size()));++i){points+=jumps[i].getPoints();has=true;}if(!team)values.append(has?(jumps[r].getDNS()?QStringLiteral("DNS"):jumps[r].getDSQ()?QStringLiteral("DSQ"):QString::number(jumps[r].getDistance(),'f',1)):QString());values.append(has?(team && child && jumps[r].getDNS()?QStringLiteral("DNS"):team && child && jumps[r].getDSQ()?QStringLiteral("DSQ"):QString::number(points,'f',1)):QString());}
        QList<QStandardItem *> items;for(const auto &value:values){auto *item=new QStandardItem(value);item->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);items.append(item);}items[0]->setData(resultIndex,Qt::UserRole);items[0]->setData(snapshot.containsLatestJump(result),CompetitionAdvancement::LatestJumpRole);const int column=team&&child?2:1;const auto country=team&&!child?result.getTeam()->getCountryCode():result.getJumper()->getCountryCode();items[column]->setIcon(QIcon(QDir::current().absoluteFilePath("flags/"+CountryCodes::toIso(country)+".svg")));return items;
    };
    for(int i=0;i<snapshot.getResultsReference().size();++i){auto &result=snapshot.getResultsReference()[i];auto items=row(result,i,false);nextModel->appendRow(items);if(team)for(auto &member:result.getTeamJumpersResultsReference())items[0]->appendRow(row(member,i,true));}
    // Expose only the finished model; row-by-row signals used to reset Quick hundreds of times.
    auto *previousSelection=table->selectionModel();
    {const QSignalBlocker blocker(table);table->setModel(nextModel);model=nextModel;table->expandAll();table->resizeColumnToContents(0);}
    if(previousSelection && previousSelection!=table->selectionModel())previousSelection->deleteLater();
    int selected=0;if(live && rounds->currentData().toInt()==0)for(int i=0;i<snapshot.getResultsReference().size();++i)if(snapshot.containsLatestJump(snapshot.getResultsReference()[i])){selected=i;break;}
    if(model->rowCount())showJump(model->index(selected,0));emit changed();previousModel->deleteLater();
}
void ResultsHistoryWidget::showJump(const QModelIndex &index) {
    const int requested=table->property("historyRoundRequested").toInt();table->setProperty("historyRoundRequested",0);
    if(!index.isValid() || index.model()!=model)return;const auto parent=index.parent();const int resultIndex=(parent.isValid()?parent:index.sibling(index.row(),0)).data(Qt::UserRole).toInt();
    if(resultIndex<0 || resultIndex>=snapshot.getResultsReference().size())return;
    auto *result=&snapshot.getResultsReference()[resultIndex];if(parent.isValid()){if(index.row()>=result->getTeamJumpersResultsReference().size())return;result=&result->getTeamJumpersResultsReference()[index.row()];}
    if(result->getType()==CompetitionSingleResult::TeamResult){if(result->getTeamJumpersResultsReference().isEmpty())return;result=result->getTeamJumperResult(result->getJumpsReference().last().getJumper());if(!result)return;}
    if(requested>result->getJumpsReference().size())return;
    preview->setJumperResult(result);preview->fillWidget();if(requested>0)preview->selectRound(requested-1);preview->show();emit changed();
}

void ResultsHistoryWidget::applyPause(){
    if(!live || !owner)return;owner->setProperty("resultHistoryPaused",paused());
    if(wasPaused==paused())return;wasPaused=paused();
    if(wasPaused){for(auto *widget:owner->findChildren<QWidget *>())if(!isAncestorOf(widget) && (qobject_cast<QAbstractButton *>(widget) || qobject_cast<QAbstractSpinBox *>(widget) || qobject_cast<QComboBox *>(widget))){disabledControls.append({widget,!widget->testAttribute(Qt::WA_ForceDisabled)});widget->setEnabled(false);}}
    else {for(const auto &entry:disabledControls)if(entry.first)entry.first->setEnabled(entry.second);disabledControls.clear();}
}
