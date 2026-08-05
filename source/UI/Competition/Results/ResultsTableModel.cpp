#include "ResultsTableModel.h"

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
    lastJumper(nullptr)
{
}

QVariant ResultsTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

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
                        QString text = QString::number(results->getResultsReference().at(index.row()).getJumps().at(jump).getDistance());
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
    else if(role == Qt::BackgroundRole){
        if(lastJumper != nullptr)
        {
            if(results->getResultByIndex(index.row())->getJumper() == lastJumper)
                return QColor(qRgb(232, 243, 255));
        }
        if(startListStatuses != nullptr){
            switch(type){
            case CompetitionRules::Individual:{
                results->sortInDescendingOrder();
                Jumper * jumper = results->getResultsReference().at(index.row()).getJumper();
                if(manager->getActualRound() == manager->getCompetitionRules()->getRoundsReference().count()){
                    if(results->getResultOfIndividualJumper(jumper)->getPosition() + StartListCompetitorStatus::remainingJumpers(*startListStatuses) == 3)
                        return QColor(qRgb(255, 232, 215));
                    else if(results->getResultOfIndividualJumper(jumper)->getPosition() + StartListCompetitorStatus::remainingJumpers(*startListStatuses) == 2)
                        return QColor(qRgb(232, 232, 232));
                    else if(results->getResultOfIndividualJumper(jumper)->getPosition() + StartListCompetitorStatus::remainingJumpers(*startListStatuses) == 1)
                        return QColor(qRgb(255, 247, 205));
                }

                if(StartListCompetitorStatus::getStatusOfJumper(results->getResultsReference().at(index.row()).getJumper(), *startListStatuses) == nullptr){
                    return QColor(qRgb(255, 255, 255));
                }
                switch(StartListCompetitorStatus::getStatusOfJumper(results->getResultsReference().at(index.row()).getJumper(), *startListStatuses)->getAdvanceStatus())
                {
                case StartListCompetitorStatus::Waiting:
                    return QColor(qRgb(253, 253, 249));
                case StartListCompetitorStatus::SureAdvanced:
                    if(StartListCompetitorStatus::getStatusOfJumper(results->getResultsReference().at(index.row()).getJumper(), *startListStatuses)->getQualifiedBy95HSRule())
                        return QColor(qRgb(233, 245, 236));
                    else{
                        return QColor(qRgb(242, 255, 246));
                    }
                case StartListCompetitorStatus::SureDroppedOut:
                    return QColor(qRgb(252, 237, 237));
                }
                break;
            }
            }
        }
        else
        {
            if(index.row() == 2)
                return QColor(qRgb(255, 232, 215));
            else if(index.row() == 1)
                return QColor(qRgb(232, 232, 232));
            else if(index.row() == 0)
                return QColor(qRgb(255, 247, 205));
            return QColor(qRgb(253, 253, 249));
        }
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
