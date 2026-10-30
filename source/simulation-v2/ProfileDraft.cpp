#include "ProfileDraft.h"
#include "../global/GlobalDatabase.h"
#include "../global/CountryCodes.h"
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QJsonArray>
#include <QSet>
#include <QUrl>
#include <QTimer>

namespace {
QString flag(const QString &country) {return QUrl::fromLocalFile(QDir("flags").absoluteFilePath(CountryCodes::toIso(country)+".svg")).toString();}
}
SimulationProfileDraft::SimulationProfileDraft(DataPack::Kind kind,QObject *parent):QObject(parent),kind(kind) {}
void SimulationProfileDraft::load(const Jumper &value,const QString &root) {athlete=originalAthlete=value;directory=root;loaded=kind==DataPack::Jumpers && bool(value.getSimulationProfile());error.clear();notifyChanged();}
void SimulationProfileDraft::load(const Hill &value) {hill=originalHill=value;loaded=kind==DataPack::Hills && bool(value.getSimulationProfile());error.clear();notifyChanged();}
void SimulationProfileDraft::clear() {loaded=false;directory.clear();error.clear();notifyChanged();}
bool SimulationProfileDraft::valid() const {return loaded;}
double SimulationProfileDraft::realHs() const {return loaded && kind==DataPack::Hills?hill.getSimulationProfile()->realHs():0;}
QVariantMap SimulationProfileDraft::profile() const {
    if(!loaded)return {};
    auto values=kind==DataPack::Jumpers?athlete.getSimulationProfile()->toJson().toVariantMap():hill.getSimulationProfile()->toJson().toVariantMap();
    values["flag"]=flag(values["country"].toString());
    if(kind==DataPack::Jumpers) {
        const auto path=athlete.getImageFilePath();values["photo"]=path.isEmpty()?QString():QUrl::fromLocalFile(path).toString();values["photoAvailable"]=!path.isEmpty();
        if(values["photoPath"].toString().isEmpty())values["photoPath"]=values["portrait"];
    }
    return values;
}
QVariantList SimulationProfileDraft::selectedPerks() const {return profile().value("perks").toList();}
QVariantList SimulationProfileDraft::countries() const {
    QVariantList values;QSet<QString> seen;
    for(const auto &country:GlobalDatabase::get()->getCountries()) {
        const auto code=CountryCodes::toFis(country.getAlpha3());if(code.size()!=3 || seen.contains(code))continue;seen.insert(code);
        values.append(QVariantMap{{"code",code},{"name",country.getName()+" · "+code},{"flag",flag(code)}});
    }
    return values;
}
bool SimulationProfileDraft::apply(const QJsonObject &json) {
    bool accepted=false;
    if(kind==DataPack::Jumpers) {AthleteProfile checked;accepted=AthleteProfile::fromJson(json,checked,error) && athlete.setSimulationProfile(checked,error);}
    else {HillProfile checked;accepted=HillProfile::fromJson(json,checked,error) && hill.setSimulationProfile(checked,error);}
    notifyChanged();return accepted;
}
void SimulationProfileDraft::setValue(const QString &key,const QVariant &value) {
    if(!loaded)return;
    const QSet<QString> athleteKeys{"name","surname","country","overall","form","landing","birthYear","personalBest","photoPath"};
    const QSet<QString> hillKeys{"name","country","k","hs","record","pointsAtK","pointsPerMetre","gatePoints","headwindPoints","tailwindPoints"};
    if(!(kind==DataPack::Jumpers?athleteKeys:hillKeys).contains(key))return;
    auto json=kind==DataPack::Jumpers?athlete.getSimulationProfile()->toJson():hill.getSimulationProfile()->toJson();
    if(json.value(key)==QJsonValue::fromVariant(value) && (key!="photoPath" || json["portrait"].toString().isEmpty())) {
        if(!error.isEmpty()){error.clear();notifyChanged();}
        return;
    }
    json[key]=QJsonValue::fromVariant(value);if(key=="photoPath")json["portrait"]="";
    apply(json);
}
void SimulationProfileDraft::setPerk(int pair,int side,int level,bool toggle) {
    if(!loaded || pair<0 || pair>=(kind==DataPack::Jumpers?3:4) || (side!=1 && side!=-1) || level<1 || level>4)return;
    auto json=kind==DataPack::Jumpers?athlete.getSimulationProfile()->toJson():hill.getSimulationProfile()->toJson();
    auto values=json["perks"].toArray();values[pair]=toggle && values[pair].toInt()*side>0?0:side*level;json["perks"]=values;apply(json);
}
void SimulationProfileDraft::reset() {if(!loaded)return;athlete=originalAthlete;hill=originalHill;error.clear();notifyChanged();}
void SimulationProfileDraft::choosePhoto() {
    if(!loaded || kind!=DataPack::Jumpers || directory.isEmpty())return;
    const auto path=QFileDialog::getOpenFileName(nullptr,tr("Zdjęcie w folderze paczki"),directory,tr("Obrazy (*.png *.jpg *.jpeg *.webp)"));
    if(!path.isEmpty())setValue("photoPath",QDir(directory).relativeFilePath(path));
}
void SimulationProfileDraft::clearPhoto() {setValue("photoPath",QString());}
void SimulationProfileDraft::openPackFolder() {if(!directory.isEmpty())QDesktopServices::openUrl(QUrl::fromLocalFile(directory));}

void SimulationProfileDraft::notifyChanged()
{
    // Synchronize domain observers now; refresh QML after its input callback.
    if(!m_changePending) {
        m_changePending=true;
        QTimer::singleShot(0,this,[this]{m_changePending=false;emit uiChanged();});
    }
    emit changed();
}
