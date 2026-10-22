#include "SingleJumpsManager.h"

#include <QDebug>
#include <QFile>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>
#include <QJsonArray>
#include <QMessageBox>
#include <QProgressDialog>
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QObject>
#include <QDir>

#include "../simulator/JumpManipulator.h"

SingleJumpsManager::SingleJumpsManager(int gate, int jumpsCount, const QString &resultsFileName, bool changeableWind, short resultsFormat) :
    jumpsCount(jumpsCount),
    changeableWind(changeableWind),
    resultsFormat(resultsFormat),
    resultsFileName(resultsFileName),
    gate(gate),
    importance(5.)
{
    windAverageCalculatingType = windCompensationDistanceEffect = DSQProbability = 0;
}

void SingleJumpsManager::simulate()
{
    jumpSimulator.setJumper(&jumper);
    jumpSimulator.setHill(&hill);
    jumpSimulator.setGate(gate);
    jumpSimulator.setCompetitionStartGate(gate);
    jumpSimulator.setDSQBaseProbability(getDSQProbability());
    jumpSimulator.setCompetitionRules(&rules);
    JumpManipulator manipulator;
    jumpSimulator.setManipulator(&manipulator);
    jumpSimulator.setJumpsImportance(importance);

    jumps.reserve(jumps.size() + qMax(0,jumpsCount));
    windsGenerator.setGenerationSettings(windsGeneratorSettings);
    QVector<Wind> winds;
    winds.reserve(windsGeneratorSettings.size());
    if(changeableWind == false)
    {
        for(const auto & setting : windsGeneratorSettings)
        {
            Wind wind;
            wind.setDirection(setting.getBaseDirection());
            wind.setStrength(setting.getBaseWindStrength());
            winds.push_back(wind);
        }
    }
    if(!changeableWind)jumpSimulator.setWinds(winds);
    QProgressDialog dialog;
    dialog.setStyleSheet("QProgressDialog{background-color: white; color: black;}");
    dialog.setMinimum(0);
    dialog.setMaximum(jumpsCount);
    dialog.setWindowTitle(QObject::tr("Symulacja pojedynczych skoków"));
    dialog.setLabelText(QString(QObject::tr("Przesymulowano %1 z %2 skoków")).arg(dialog.value()).arg(dialog.maximum()));
    dialog.setCancelButtonText(QObject::tr("Przerwij"));
    dialog.setMinimumDuration(0);
    dialog.setModal(true);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.show();

    const int initialCount = jumps.size();
    const auto updateProgress = [&](int count) {
        dialog.setLabelText(QString(QObject::tr("Przesymulowano %1 z %2 skoków")).arg(count).arg(jumpsCount));
        dialog.setValue(count);
    };
    updateProgress(0);
    QCoreApplication::processEvents();
    QElapsedTimer progressPulse;
    progressPulse.start();
    for(int i=0; i<jumpsCount; i++){
        if((i % 32)==0 && progressPulse.elapsed()>=33) {
            updateProgress(i);
            progressPulse.restart();
        }
        if(dialog.wasCanceled())
            break;

        if(getChangeableWind() == true){
            jumpSimulator.setWinds(windsGenerator.generateWinds());
        }
        jumpSimulator.simulateJump();
        jumps.push_back(jumpSimulator.getJumpData());
        jumps.last().setInSingleJumps(true);

    }
    if(!dialog.wasCanceled())updateProgress(jumps.size()-initialCount);

    jumpsCount = jumps.count();
    jumpSimulator.setManipulator(nullptr);
}

void SingleJumpsManager::saveResultsCsv(QString fileName)
{
    QDir dir(QDir::currentPath());
    dir.mkpath("results/single-jumps");
    QFile file("results/single-jumps/" + fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.resize(0);
    QTextStream stream(&file);
    stream << QObject::tr("Lp.") << ";" << QObject::tr("Odleglosc") << ";" << QObject::tr("Belka")  << ";" << QObject::tr("Usredniony wiatr") << ";" << QObject::tr("Punkty za wiatr") << ";" << QObject::tr("Punkty za styl") << ";" << QObject::tr("Nota laczna") << "\n";
    int i = 0;
    for(auto & jump : jumps)
    {
        QString string = QString::number(i+1)+";";
        string += QString::number(jump.getDistance())+";";
        string += QString::number(jump.getGate())+";";
        string += QString::number(jump.getAveragedWind())+";";
        string += QString::number(jump.getTotalCompensation())+";";
        string += QString::number(jump.getJudgesPoints())+";";
        string += QString::number(jump.getPoints())+";";
        stream << string.replace(".", ",") << "\n";
        i++;
    }
    file.close();
}

double SingleJumpsManager::getImportance() const
{
    return importance;
}

void SingleJumpsManager::setImportance(double newImportance)
{
    importance = newImportance;
}

CompetitionRules& SingleJumpsManager::getRules()
{
    return rules;
}

void SingleJumpsManager::setRules(const CompetitionRules &newRules)
{
    rules = newRules;
}

int SingleJumpsManager::getDSQProbability() const
{
    return DSQProbability;
}

void SingleJumpsManager::setDSQProbability(int newDSQProbability)
{
    DSQProbability = newDSQProbability;
}

Jumper SingleJumpsManager::getJumper() const
{
    return jumper;
}

void SingleJumpsManager::setJumper(const Jumper &newJumper)
{
    jumper = newJumper;
}

Hill SingleJumpsManager::getHill() const
{
    return hill;
}

void SingleJumpsManager::setHill(const Hill &newHill)
{
    hill = newHill;
}

int SingleJumpsManager::getJumpsCount() const
{
    return jumpsCount;
}

void SingleJumpsManager::setJumpsCount(int newJumpsCount)
{
    jumpsCount = newJumpsCount;
}

QVector<JumpData> SingleJumpsManager::getJumps() const
{
    return jumps;
}

QVector<JumpData> &SingleJumpsManager::getJumpsReference()
{
    return jumps;
}

void SingleJumpsManager::setJumps(const QVector<JumpData> &newJumps)
{
    jumps = newJumps;
}

QVector<WindGenerationSettings> SingleJumpsManager::getWindsGeneratorSettings() const
{
    return windsGeneratorSettings;
}

void SingleJumpsManager::setWindsGeneratorSettings(const QVector<WindGenerationSettings> &newWindsGeneratorSettings)
{
    windsGeneratorSettings = newWindsGeneratorSettings;
}

bool SingleJumpsManager::getChangeableWind() const
{
    return changeableWind;
}

void SingleJumpsManager::setChangeableWind(bool newChangeableWind)
{
    changeableWind = newChangeableWind;
}

QString SingleJumpsManager::getResultsFileName() const
{
    return resultsFileName;
}

void SingleJumpsManager::setResultsFileName(const QString &newResultsFileName)
{
    resultsFileName = newResultsFileName;
}

int SingleJumpsManager::getGate() const
{
    return gate;
}

void SingleJumpsManager::setGate(int newGate)
{
    gate = newGate;
}

short SingleJumpsManager::getWindAverageCalculatingType() const
{
    return windAverageCalculatingType;
}

void SingleJumpsManager::setWindAverageCalculatingType(short newWindAverageCalculatingType)
{
    windAverageCalculatingType = newWindAverageCalculatingType;
}

short SingleJumpsManager::getWindCompensationDistanceEffect() const
{
    return windCompensationDistanceEffect;
}

void SingleJumpsManager::setWindCompensationDistanceEffect(short newWindCompensationDistanceEffect)
{
    windCompensationDistanceEffect = newWindCompensationDistanceEffect;
}
