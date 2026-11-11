#include "FormSession.h"
#include "../simulator/Jumper.h"
#include <QCryptographicHash>
#include <QJsonArray>
#include <QRandomGenerator>
#include <QObject>
#include <QSet>
#include <QLocale>
#include <cmath>
#include <algorithm>
#include <QStringList>
#include <QSettings>

namespace AutoForm {
Session::Session():seed(QRandomGenerator::system()->generate64()),config(defaultConfig().withVariability(QSettings().value("simulation/simple/seasonFormVariability",2).toInt())){}
QJsonObject Session::toJson() const {
    QJsonObject stored;for(auto it=states.cbegin();it!=states.cend();++it)stored[it.key()]=it.value().toJson();
    return {{"version",version},{"enabled",enabled},{"anchorPending",anchorPending},{"seed",QString::number(seed)},{"config",config.toJson()},{"states",stored}};
}
bool Session::fromJson(const QJsonObject &json,Session &out,QString &error){
    if(json["version"].toDouble()!=version || !json["enabled"].isBool() || !json["anchorPending"].isBool() || !json["states"].isObject()){error=QObject::tr("Nieprawidłowe dane automatycznej formy.");return false;}
    Session candidate;bool seedOk=false;candidate.seed=json["seed"].toString().toULongLong(&seedOk);
    if(!seedOk || !Config::fromJson(json["config"].toObject(),candidate.config,error))return false;
    candidate.enabled=json["enabled"].toBool();candidate.anchorPending=json["anchorPending"].toBool();
    const auto states=json["states"].toObject();if(states.size()>10000){error=QObject::tr("Zbyt wiele stanów automatycznej formy.");return false;}
    for(auto it=states.begin();it!=states.end();++it){State state;if(it.key().isEmpty() || !it.value().isObject() || !State::fromJson(it.value().toObject(),state,error))return false;candidate.states.insert(it.key(),state);}
    out=candidate;error.clear();return true;
}
bool Session::setEnabled(bool value,bool dates,QString &error){
    if(value && !dates){error=QObject::tr("Automatyczna forma wymaga dat wydarzeń.");return false;}
    if(value!=enabled){enabled=value;anchorPending=true;}
    error.clear();return true;
}
bool Session::update(QDate date,const QVector<Jumper *> &athletes,bool dates,QString &error){
    if(!enabled || !dates){if(!dates)anchorPending=true;return true;}
    if(!date.isValid()){error=QObject::tr("Ustaw datę wydarzenia przed przeliczeniem formy.");return false;}
    auto candidate=states;QVector<QPair<Jumper *,int>> updates;QSet<QString> active;
    try{
        for(auto *athlete:athletes){
            if(!athlete || !athlete->getSimulationProfile()){error=QObject::tr("Automatyczna forma wymaga nowych profili zawodników.");return false;}
            const auto profile=athlete->getSimulationProfile();const auto key=athlete->getIDStr();active.insert(key);
            if(!candidate.contains(key)){
                const auto bytes=QCryptographicHash::hash(key.toUtf8(),QCryptographicHash::Sha256);quint64 stream=0;for(int i=0;i<8;++i)stream=(stream<<8)|quint8(bytes[i]);
                candidate.insert(key,initialize(profile->form,date,SimulationV2::deriveSeed(seed,stream),config));
            }
            auto &state=candidate[key];
            if(anchorPending){state.date=date;if(state.displayed()!=profile->form)state.form=profile->form;}
            // Switching calendars never rewinds an already processed form timeline.
            if(date>state.date)advance(state,date,profile->perks.size()>3?profile->perks[3]:0,config);
            updates.append(qMakePair(athlete,state.displayed()));
        }
    }catch(const std::exception &exception){error=QString::fromUtf8(exception.what());return false;}
    for(auto it=candidate.begin();it!=candidate.end();)if(!active.contains(it.key()))it=candidate.erase(it);else ++it;
    for(const auto &update:updates){auto profile=*update.first->getSimulationProfile();profile.form=update.second;if(!update.first->setSimulationProfile(profile,error))return false;}
    states=std::move(candidate);anchorPending=false;error.clear();return true;
}
QVariantMap Session::diagnostic(const Jumper &athlete) const {
    const auto it=states.constFind(athlete.getIDStr());
    const auto profile=athlete.getSimulationProfile();const int perk=profile?std::clamp(profile->perks[3],-4,4):0,p=perk+4;
    QVariantList rows;
    const auto number=[](double value,int precision=2){return QLocale().toString(value,'f',precision);};
    const auto add=[&](const QString &group,const QString &label,const QString &value,const QString &help=QString()){rows.append(QVariantMap{{"group",group},{"label",label},{"value",value},{"help",help}});};
    const auto model=QObject::tr("Dynamika"),status=QObject::tr("Stan"),random=QObject::tr("Losowanie");
    add(status,QObject::tr("F w profilu"),profile?QString::number(profile->form):QStringLiteral("—"));
    add(model,QObject::tr("Perk formy"),perk==0?QObject::tr("Brak"):(perk<0?QObject::tr("Równa forma %1"):QObject::tr("Chwiejna forma %1")).arg(QStringList{"I","II","III","IV"}[std::abs(perk)-1]));
    add(model,QObject::tr("Połowiczny powrót F"),number(config.formHalfLife)+QObject::tr(" dni"),QObject::tr("Po tym czasie pozostaje połowa różnicy między F a stałą bazą, bez losowości."));
    add(model,QObject::tr("Połowiczny powrót μ"),number(config.baseHalfLife)+QObject::tr(" dni"),QObject::tr("Tempo powrotu bazy do centrum aktualnego okresu."));
    add(model,QObject::tr("Skala odchylenia F"),number(config.formDeviation*config.noise[p]),QObject::tr("Robocze odchylenie przy stałej bazie, przed ograniczeniem do 1–100."));
    const double a=std::exp2(-1/config.formHalfLife),baseA=std::exp2(-1/config.baseHalfLife);
    add(model,QObject::tr("Losowość F / dzień"),number(config.formDeviation*config.noise[p]*std::sqrt(1-a*a)),QObject::tr("Odchylenie losowego składnika zmiany formy w jednym dniu, przed ograniczeniem zakresu."));
    add(model,QObject::tr("Losowość μ / dzień"),number(config.baseDeviation*std::sqrt(1-baseA*baseA)));
    add(model,QObject::tr("Mnożnik wahań"),number(config.noise[p])+"×");
    add(model,QObject::tr("Waga wejścia w kryzys"),number(config.crisis[p])+"×",QObject::tr("Mnożnik wagi przed normalizacją szans przejścia."));
    add(model,QObject::tr("Waga wejścia na szczyt"),number(config.peak[p])+"×",QObject::tr("Mnożnik wagi przed normalizacją szans przejścia."));
    add(model,QObject::tr("Zachowanie starej bazy"),number(config.entryInertia*100,1)+" %");
    add(random,QObject::tr("Seed rozgrywki"),QString::number(seed));
    if(it==states.cend())return {{"automatic",enabled},{"pending",true},{"rows",rows}};
    const auto &state=it.value();
    add(status,QObject::tr("Dokładne F procesu"),number(state.form),QObject::tr("Ostatni przeliczony stan. Po ręcznej zmianie przy wyłączonej autoformie może różnić się od profilu."));
    add(status,QObject::tr("Baza μ"),number(state.base));
    add(status,QObject::tr("F − μ"),number(state.form-state.base));
    add(status,QObject::tr("Okres dyspozycji"),regimeName(state.regime));
    add(status,QObject::tr("Centrum okresu"),number(config.centers[state.regime]));
    add(status,QObject::tr("Ostatnie przeliczenie"),state.date.toString("dd.MM.yyyy"));
    add(status,QObject::tr("Do zmiany okresu"),number(state.remainingDays)+QObject::tr(" dni"),QObject::tr("Pozostały wylosowany czas do następnej zmiany okresu. Sama forma nie przeskakuje wtedy do jego centrum."));
    add(status,QObject::tr("Średni czas okresu"),number(config.meanDays[state.regime])+QObject::tr(" dni"));
    add(status,QObject::tr("Zmiany okresu"),QString::number(state.transitions));
    QStringList visited;for(int r=0;r<regimeCount;++r)if(state.visited&(1<<r))visited.append(regimeName(r));
    add(status,QObject::tr("Odwiedzone okresy"),QString::number(visited.size()),visited.join(", "));
    add(model,QObject::tr("Powrót F / dzień"),number((state.base-state.form)*(1-a)),QObject::tr("Przewidywany składnik powrotu do bazy w jednym dniu, przy niezmienionej bazie i bez losowości."));
    add(model,QObject::tr("Powrót μ / dzień"),number((config.centers[state.regime]-state.base)*(1-baseA)));
    add(model,QObject::tr("Rozrzut nowej bazy"),number(config.entryDeviation[state.regime]));
    const auto probabilities=transitionProbabilities(state.regime,perk,config);
    for(int r=0;r<regimeCount;++r)if(r!=state.regime)add(QObject::tr("Następna zmiana okresu"),regimeName(r),number(probabilities[r]*100,2)+" %",QObject::tr("Szansa przy najbliższej zmianie okresu, nie w każdym dniu."));
    add(random,QObject::tr("Stan RNG"),QString::number(state.randomState));
    return {{"automatic",enabled},{"pending",false},{"form",state.form},{"base",state.base},{"regime",regimeName(state.regime)},{"date",state.date.toString("dd.MM.yyyy")},{"transitions",state.transitions},{"rows",rows}};
}
Checkpoint::Checkpoint(Session &session,const QVector<Jumper *> &entries):live(session),before(session),athletes(entries){for(auto *athlete:athletes)if(athlete && athlete->getSimulationProfile())forms.insert(athlete->getIDStr(),athlete->getSimulationProfile()->form);}
Checkpoint::~Checkpoint(){restore();}
void Checkpoint::restore(){
    if(finished)return;finished=true;live=before;
    for(auto *athlete:athletes)if(athlete && athlete->getSimulationProfile() && forms.contains(athlete->getIDStr())){
        auto profile=*athlete->getSimulationProfile();profile.form=forms.value(athlete->getIDStr());QString error;athlete->setSimulationProfile(profile,error);
    }
}
}
