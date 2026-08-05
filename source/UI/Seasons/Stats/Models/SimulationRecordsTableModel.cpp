#include "SimulationRecordsTableModel.h"
#include "../../../../utilities/functions.h"
#include "../../../../global/CountryFlagsManager.h"
#include <QIcon>

SimulationRecordsTableModel::SimulationRecordsTableModel(SimulationSave * save, QObject *parent)
    : QAbstractTableModel(parent), save(save)
{
}

QVariant SimulationRecordsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        if(section == 0)
            return tr("Lp");
        else if(section == 1)
            return tr("Zawodnik");
        else if(section == 2)
            return tr("Wartość");
        else if(section == 3)
            return tr("Zawody/Skok");
    }
    return QVariant();
}

int SimulationRecordsTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return records.count();
}

int SimulationRecordsTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 4;
}

QVariant SimulationRecordsTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole)
    {
        if(index.column() == 0)
            return index.row() + 1;
        else if(index.column() == 1)
            return records[index.row()].first->getJumper()->getNameAndSurname();
        else if(index.column() == 2)
            return records[index.row()].second;
        else if(index.column() == 3)
        {
            JumpData * jump = records[index.row()].first;
            CompetitionInfo * comp = jump->getCompetition();
            Season * season = nullptr;
            SeasonCalendar * c = nullptr;
            for(auto & s : save->getSeasonsReference())
                for(auto & cal : s.getCalendarsReference())
                {
                    if(cal->getCompetitionsReference().contains(comp))
                    {
                        season = &s;
                        c = cal;
                    }

                }


            QString string = QString::number(season->getSeasonNumber()) + "/" + QString::number(c->getCompetitionsReference().indexOf(comp) + 1)
                             + " " + comp->getHill()->getName() + " HS" + QString::number(comp->getHill()->getHSPoint()) + tr(" (Runda ")
                             + QString::number(MyFunctions::getIndexOfItemInVector(jump->getSingleResult()->getJumpsReference(), jump) + 1) + ") ";
            switch(comp->getSerieType())
            {
            case CompetitionInfo::Competition:
                string += tr("(Konk.)");
                break;
            case CompetitionInfo::Qualifications:
                string += tr("(Kwal.)");
                break;
            case CompetitionInfo::TrialRound:
                string += tr("(Prób.)");
                    break;
            case CompetitionInfo::Training:
                string += tr("(Tren.)");
                    break;
            }

            return string;
        }
    }
    else if(role == Qt::FontRole)
    {
        QFont font("Quicksand", 9);
        if(index.column() == 0)
            font.setBold(true);
        else if(index.column() == 2)
            font.setBold(true);
        else if(index.column() == 3)
            font.setItalic(true);
    }
    else if(role == Qt::TextAlignmentRole)
        return Qt::AlignHCenter;
    else if(role == Qt::DecorationRole)
    {
        JumpData * jump = records[index.row()].first;
        if(index.column() == 1) //zawodnik
            return QIcon(CountryFlagsManager::getFlagPixmap(jump->getJumper()->getCountryCode().toLower()));
        else if(index.column() == 3) //skocznia
            return QIcon(CountryFlagsManager::getFlagPixmap(jump->getCompetition()->getHill()->getCountryCode().toLower()));
    }
    else if(role == Qt::BackgroundRole)
    {
        if(index.row() == 0)
            return QColor(qRgb(250, 239, 205));
        else if(index.row() == 1)
            return QColor(qRgb(222, 222, 222));
        else if(index.row() == 2)
            return QColor(qRgb(255, 227, 199));
        else
            return QColor(qRgb(253, 253, 253));
    }

    // FIXME: Implement me!
    return QVariant();
}

QVector<QPair<JumpData *, double> > SimulationRecordsTableModel::getRecords() const
{
    return records;
}

QVector<QPair<JumpData *, double> > &SimulationRecordsTableModel::getRecordsReference()
{
    return records;
}

void SimulationRecordsTableModel::setRecords(const QVector<QPair<JumpData *, double> > &newRecords)
{
    records = newRecords;
}

SimulationSave *SimulationRecordsTableModel::getSave() const
{
    return save;
}

void SimulationRecordsTableModel::setSave(SimulationSave *newSave)
{
    save = newSave;
}
