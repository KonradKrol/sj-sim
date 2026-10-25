#include "ResultsTableModel.h"
#include "../../../competitions/CompetitionAdvancement.h"

#include "../../../competitions/CompetitionResults.h"
#include "../../../competitions/CompetitionManagers/IndividualCompetitionManager.h"
#include "../../../global/CountryFlagsManager.h"
#include "../../../utilities/functions.h"
#include <QFont>
#include <QPixmap>
#include <algorithm>

// Musimy dać możliwość ustalenia własnej liczby kwalifikujących się zawodników. Tzn tak: Jeżeli konkurs jest jednoseryjny to to działa. Robimy to w updateAdvanceStatuses.


ResultsTableModel::ResultsTableModel(int type, CompetitionResults *results, AbstractCompetitionManager *manager, QObject *parent)
    : QAbstractTableModel(parent),
    type(type),
    results(results),
    manager(manager),
    startListStatuses(nullptr),
    qualifiersLimit(0),
    lastJumper(nullptr)
{
}

QVariant ResultsTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !results || index.row()<0 || index.row()>=results->getResultsReference().size() || index.column()<0 || index.column()>=columnCount())
        return QVariant();

    if(role==CompetitionAdvancement::LatestJumpRole)return results->containsLatestJump(results->getResultsReference().at(index.row()));
    auto *jumper=results->getResultsReference().at(index.row()).getJumper();
    const auto *status=startListStatuses?StartListCompetitorStatus::getStatusOfJumper(jumper,*startListStatuses):nullptr;
    const int roundStatus=manager && !status && results->getResultsReference().at(index.row()).getScoringRound()<manager->getActualRound()?StartListCompetitorStatus::SureDroppedOut
        :manager && manager->getActualRound()<manager->effectiveRoundsCount() && status?status->getAdvanceStatus():-1;
    const int qualificationStatus=manager?manager->qualificationStatus(jumper):-1;
    if(role==CompetitionAdvancement::LuckyLoserRole){auto *individual=dynamic_cast<IndividualCompetitionManager *>(manager);return individual && individual->isLuckyLoser(jumper);}
    if(role==CompetitionAdvancement::RoundStatusRole)return roundStatus;
    if(role==CompetitionAdvancement::QualificationStatusRole)return qualificationStatus;
    if(role==CompetitionAdvancement::QualificationTargetRole) {
        auto *target=manager?manager->nearestQualificationTarget():nullptr;
        return target && target->getHill()?target->getHill()->getName():QString();
    }
    if(role == Qt::DisplayRole){
        switch(type){
        case CompetitionRules::Individual:{
            switch(index.column()){
            case 0:
                return results->getResultsReference().at(index.row()).getPosition();
                //return indResults->getEditableJumpersResults().at(index.row()).getPosition();
                break;
            case 1:
                //qDebug()<<index.column()<<", "<<indResults->getEditableJumpersResults().at(index.row()).getJumper()->getNameAndSurname().toUpper();
                return results->getResultsReference().at(index.row()).getJumper()->getNameAndSurname();
                break;
            case 2:
                return QString::number(results->getResultsReference().at(index.row()).getPointsSum(), 'f', 1);
                break;
            }
            if(index.column() > 2){
                int col = index.column() - 3;
                int m = 1;
                int jump = 0;
                while(true){
                    if(col < (2 * m)){
                        jump = m - 1;
                        break;
                    }
                    m++;
                }
                int i = col % 2;
                if(results->getResultsReference().at(index.row()).getJumps().count() > jump){
                    switch(i){
                    case 0:{
                        if(results->getResultsReference().at(index.row()).getJumps().at(jump).getDNS())return QStringLiteral("DNS");
                        if(results->getResultsReference().at(index.row()).getJumps().at(jump).getDSQ())return QStringLiteral("DSQ");
                        QString text = QString::number(results->getResultsReference().at(index.row()).getJumps().at(jump).getDistance(), 'f', 1);
                        if(results->getResultsReference().at(index.row()).getJumps().at(jump).getLanding().getType() == Landing::Fall)
                            text += "*";
                        return text;
                    }
                    case 1: return results->getResultsReference().at(index.row()).getJumps().at(jump).getPoints();
                    }
                }
                break;
            }
        }
        }
    }
    else if(role == Qt::ToolTipRole){
        return data(index, Qt::DisplayRole);
    }
    else if(role == Qt::BackgroundRole){
        if(!manager || manager->checkCompetitionEnd()) {
            const int place=results->getResultsReference().at(index.row()).getPosition();
            if(place==1)return QColor(255,247,205);
            if(place==2)return QColor(232,232,232);
            if(place==3)return QColor(255,232,215);
        }
        if(roundStatus==StartListCompetitorStatus::SureDroppedOut || qualificationStatus==StartListCompetitorStatus::SureDroppedOut)return QColor(238,240,242);
        if(roundStatus==StartListCompetitorStatus::SureAdvanced)return QColor(245,255,248);
        if(!results->getLatestJumpId().isEmpty() || lastJumper != nullptr)
        {
            if(results->getLatestJumpId().isEmpty()?results->getResultByIndex(index.row())->getJumper()==lastJumper:results->containsLatestJump(*results->getResultByIndex(index.row())))
                return QColor(qRgb(232, 243, 255));
        }
        return QColor(253,253,249);
    }
    else if(role == Qt::DecorationRole){
        switch(index.column()){
        case 1:
            switch(type){
            case CompetitionRules::Individual:{
                return CountryFlagsManager::getFlagPixmap(results->getResultByIndex(index.row())->getJumper()->getCountryCode().toLower()).scaled(40, 28.5);
                break;
            }
            case CompetitionRules::Team:{
                return CountryFlagsManager::getFlagPixmap(results->getResultByIndex(index.row())->getTeam()->getCountryCode().toLower()).scaled(40, 28.5);
                break;
            }
            }
        }
    }
    else if(role == Qt::ForegroundRole){
        if(roundStatus==StartListCompetitorStatus::SureDroppedOut || qualificationStatus==StartListCompetitorStatus::SureDroppedOut)return QColor(108,116,128);
        if(index.column() == 0){
            return QColor(qRgb(0, 0, 0));
        }
        else if(index.column() == 1){
            return QColor(qRgb(23, 17, 92));
        }
        else return QColor(qRgb(50, 50, 50));
    }
    else if(role == Qt::FontRole){
        QFont font("Quicksand Medium", 11, 500);
        if(index.column() == 0){
            font.setBold(true);
            return font;
        }
        else if(index.column() == 1)
            return font;
        else if(index.column() == 2){
            font.setItalic(true);
            font.setBold(true);
            return font;
        }
        else{
            font.setItalic(true);
            return font;
        }
    }
    else if(role == Qt::TextAlignmentRole){
        return Qt::AlignCenter;
    }

    // FIXME: Implement me!
    return QVariant();
}

Jumper *ResultsTableModel::getLastJumper() const
{
    return lastJumper;
}

void ResultsTableModel::setLastJumper(Jumper *newLastJumper)
{
    lastJumper = newLastJumper;
}

int ResultsTableModel::getQualifiersLimit() const
{
    return qualifiersLimit;
}

void ResultsTableModel::setQualifiersLimit(int newQualifiersLimit)
{
    qualifiersLimit = newQualifiersLimit;
}

QVector<StartListCompetitorStatus> *ResultsTableModel::getStartListStatuses() const
{
    return startListStatuses;
}

void ResultsTableModel::setStartListStatuses(QVector<StartListCompetitorStatus> *newStartListStatuses)
{
    startListStatuses = newStartListStatuses;
}

AbstractCompetitionManager *ResultsTableModel::getManager() const
{
    return manager;
}

void ResultsTableModel::setManager(AbstractCompetitionManager *newManager)
{
    manager = newManager;
}

QVariant ResultsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole){
        if(type == CompetitionRules::Individual){
            //IndividualCompetitionResults * indResults = dynamic_cast<IndividualCompetitionResults *>(results);
            switch(orientation){
            case Qt::Horizontal:
                switch(section){
                case 0:
                    return tr("Poz.");
                case 1:
                return tr("Imię i nazwisko");
                    case 2:
                    return tr("Suma punktów");
                }
                if(section > 2){
                    int col = section - 3;
                    int m = 1;
                    int jump = 0;
                    while(true){
                        if(col < (2 * m)){
                            jump = m - 1;
                            break;
                        }
                        m++;
                    }
                    int i = col % 2;
                    switch(i){
                    case 0: return tr("Odległość (seria ") + QString::number(jump + 1) + ")";
                    case 1: return tr("Punkty (seria ") + QString::number(jump + 1) + ")";
                    }
                }
                break;
            }
        }
    }
    else if(role == Qt::FontRole){
        switch(orientation){
        case Qt::Horizontal:{
            QFont f("Ubuntu", 8, 600);
            f.setBold(true);
            return f;
        }
        case Qt::Vertical:
            return QFont("Ubuntu", 6, 450);
        }
    }
    else if(role == Qt::ForegroundRole){
        return QColor(qRgb(40, 40, 40));
    }
    else if(role == Qt::TextAlignmentRole){
        return Qt::AlignCenter;
    }
    return QVariant();
}

int ResultsTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    switch(type){
    case CompetitionRules::Individual:
        return results->getResultsReference().count();
    case CompetitionRules::Team:
        return 0;
    }

    return 0;
}

int ResultsTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    switch(type){
    case CompetitionRules::Individual:{
        int max = 0;
        for(auto & res : results->getResultsReference()){
            if(res.getJumpsReference().count() > max)
                max = res.getJumpsReference().count();
        }
        if(max == 0)
            return 0;
        else
            return (max * 2) + 3;
    }
    case CompetitionRules::Team:
        return 0;
    }

    return 0;
}

CompetitionResults *ResultsTableModel::getResults() const
{
    return results;
}

void ResultsTableModel::setResults(CompetitionResults *newResults)
{
    results = newResults;
}

int ResultsTableModel::getType() const
{
    return type;
}

void ResultsTableModel::setType(int newType)
{
    type = newType;
}
