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
#include <QtConcurrent>
#include <QFutureWatcher>
#include <QEventLoop>
#include <QTimer>
#include <QThreadPool>
#include <QThread>
#include <optional>
#include <vector>
#include "../global/GlobalSimulationSettings.h"

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

QFuture<QVector<JumpData>> SingleJumpsManager::simulateAsync(int count,int threads,std::shared_ptr<BatchControl> control) const {
    count=qMax(0,count);
    const auto athleteSnapshot=jumper;const auto hillSnapshot=hill;const auto rulesSnapshot=rules;
    const auto windSettings=windsGeneratorSettings;const bool varying=changeableWind;
    const auto settings=GlobalSimulationSettings::snapshot();const int batchGate=gate,dsq=DSQProbability;const double priority=importance;
    if(!control)control=std::make_shared<BatchControl>();
    const int workers=qBound(1,threads>0?threads:qMax(1,QThread::idealThreadCount()),qMin(4,qMax(1,count/512)));
    return QtConcurrent::run([=] {
        std::vector<std::optional<JumpData>> results(static_cast<size_t>(count));
        std::atomic_int next{0};
        const auto work=[&] {
            GlobalSimulationSettings::ScopedSnapshot isolated(settings);
            auto athlete=athleteSnapshot;auto venue=hillSnapshot;auto competitionRules=rulesSnapshot;
            JumpManipulator manipulation;JumpSimulator simulator(&athlete,{},&venue);
            simulator.setCompetitionRules(&competitionRules);simulator.setManipulator(&manipulation);simulator.setGate(batchGate);simulator.setCompetitionStartGate(batchGate);simulator.setDSQBaseProbability(dsq);simulator.setJumpsImportance(priority);
            WindsGenerator generator(windSettings);QVector<Wind> fixed;
            if(!varying){for(const auto &value:windSettings){Wind wind;wind.setDirection(value.getBaseDirection());wind.setStrength(value.getBaseWindStrength());fixed.append(wind);}simulator.setWinds(fixed);}
            while(!control->canceled.load(std::memory_order_relaxed)) {
                const int first=next.fetch_add(32,std::memory_order_relaxed);if(first>=count)break;
                for(int index=first;index<qMin(count,first+32);++index) {
                    if(control->canceled.load(std::memory_order_relaxed))break;
                    if(varying)simulator.setWinds(generator.generateWinds());simulator.simulateJump();
                    auto jump=simulator.getJumpData();jump.setJumper(nullptr);jump.setHill(nullptr);jump.setSimulator(nullptr);jump.setInSingleJumps(true);
                    results[static_cast<size_t>(index)]=std::move(jump);control->completed.fetch_add(1,std::memory_order_relaxed);
                }
            }
        };
        if(workers==1)work();
        else {
            QThreadPool pool;pool.setMaxThreadCount(workers);std::vector<QFuture<void>> tasks;
            for(int worker=0;worker<workers;++worker)tasks.push_back(QtConcurrent::run(&pool,work));
            for(auto &task:tasks)task.waitForFinished();
        }
        QVector<JumpData> completed;completed.reserve(control->completed.load(std::memory_order_relaxed));
        for(auto &jump:results)if(jump)completed.append(std::move(*jump));return completed;
    });
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
    if(jumpsCount>=4096) {
        auto control=std::make_shared<BatchControl>();
        auto future=simulateAsync(jumpsCount,0,control);
        QFutureWatcher<QVector<JumpData>> watcher;QEventLoop wait;QTimer pulse;
        QObject::connect(&watcher,&QFutureWatcher<QVector<JumpData>>::finished,&wait,&QEventLoop::quit);
        QObject::connect(&dialog,&QProgressDialog::canceled,&wait,[control]{control->canceled.store(true,std::memory_order_relaxed);});
        QObject::connect(&pulse,&QTimer::timeout,&wait,[&]{updateProgress(control->completed.load(std::memory_order_relaxed));if(dialog.wasCanceled())control->canceled.store(true,std::memory_order_relaxed);});
        watcher.setFuture(future);pulse.start(33);wait.exec();pulse.stop();
        auto completed=future.result();
        for(auto &jump:completed){jump.setJumper(&jumper);jump.setHill(&hill);jump.setSimulator(&jumpSimulator);jumps.append(std::move(jump));}
    } else for(int i=0; i<jumpsCount; i++){
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
