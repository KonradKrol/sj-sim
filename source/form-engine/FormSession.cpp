#include "FormSession.h"
#include "../simulator/Jumper.h"
#include <QCryptographicHash>
#include <QJsonArray>
#include <QRandomGenerator>
#include <QObject>
#include <QSet>

namespace AutoForm {
Session::Session():seed(QRandomGenerator::system()->generate64()),config(defaultConfig()){}
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
    if(it==states.cend())return {{"automatic",enabled},{"pending",true}};
    const auto &state=it.value();
    return {{"automatic",enabled},{"pending",false},{"form",state.form},{"base",state.base},{"regime",regimeName(state.regime)},{"date",state.date.toString("dd.MM.yyyy")},{"transitions",state.transitions}};
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
