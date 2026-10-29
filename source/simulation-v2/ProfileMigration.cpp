#include "ProfileMigration.h"
#include "../global/CountryCodes.h"
#include <QFileInfo>
#include <algorithm>
#include <cmath>
#include <utility>

namespace SimulationV2 {

bool approximateLegacyPack(const DataPack &source,double maximumSkills,PackMigration &result,QString &error)
{
    const auto fail=[&](const QString &message){error=message;return false;};
    if(source.engineVersion!=1 || source.kind==DataPack::Rules)
        return fail(QObject::tr("Wybierz starą paczkę skoczków lub skoczni. Nowe profile nie wymagają migracji."));
    if(!std::isfinite(maximumSkills) || maximumSkills<=0)
        return fail(QObject::tr("Nieprawidłowa skala dawnych umiejętności."));
    PackMigration migration;migration.pack.kind=source.kind;migration.pack.engineVersion=2;
    migration.pack.name=source.name.left(75)+QObject::tr(" — ze starej wersji");
    migration.pack.ageMode=source.ageMode;migration.pack.referenceYear=source.referenceYear;
    migration.pack.approximateMigrationSource=source.id;
    migration.warning=QObject::tr("To niedokładna, przybliżona migracja. OVR i forma są przeliczone, lądowanie przybliżone, a wszystkie perki ustawione od zera. Oryginalna paczka pozostaje bez zmian.");
    for(const auto &old:source.jumpers) {
        if(old.getSimulationProfile())return fail(QObject::tr("Paczka zawiera już nowe profile. Nie przeliczono ich ponownie."));
        auto profile=AthleteProfile::create();const auto skills=old.getJumperSkills();
        for(double value:{skills.getTakeoffTechnique(),skills.getFlightTechnique(),skills.getForm(),skills.getLandingStyle(),old.getPersonalBest()})
            if(!std::isfinite(value))return fail(QObject::tr("Skoczek %1 ma nieprawidłowe parametry.").arg(old.getNameAndSurname()));
        const auto scaled=[&](double value) {return int(std::round(std::clamp(value/maximumSkills*100,1.0,100.0)));};
        profile.name=old.getName();profile.surname=old.getSurname();profile.country=CountryCodes::toFis(old.getCountryCode());
        profile.overall=scaled(skills.getTakeoffTechnique()*.5+skills.getFlightTechnique()*.5);profile.form=scaled(skills.getForm());
        profile.landing=int(std::round(std::clamp(skills.getLandingStyle()/5,0.0,4.0)));
        profile.birthYear=old.getBirthYear();profile.personalBest=std::clamp(old.getPersonalBest(),0.0,1000.0);profile.sourceId=old.getIDStr();
        QString image=old.getImageName();image.replace('\\','/');
        if(!image.isEmpty())profile.photoPath="photos/"+profile.id+"/"+QFileInfo(image).fileName();
        ::Jumper jumper;if(!jumper.setSimulationProfile(profile,error))return false;
        migration.pack.jumpers.append(jumper);
        if(!image.isEmpty())migration.photos.append({image,profile.photoPath});
    }
    for(const auto &old:source.hills) {
        if(old.getSimulationProfile())return fail(QObject::tr("Paczka zawiera już nowe profile. Nie przeliczono ich ponownie."));
        auto profile=HillProfile::create();profile.name=old.getName();profile.country=CountryCodes::toFis(old.getCountryCode());
        profile.k=old.getKPoint();profile.hs=old.getHSPoint();profile.record=old.getRecord();profile.sourceId=old.getIDStr();
        profile.pointsAtK=old.getPointsForKPoint();profile.pointsPerMetre=old.getPointsForMeter();profile.gatePoints=old.getPointsForGate();
        profile.headwindPoints=old.getPointsForFrontWind();profile.tailwindPoints=old.getPointsForBackWind();
        ::Hill hill;if(!hill.setSimulationProfile(profile,error))return false;migration.pack.hills.append(hill);
    }
    result=std::move(migration);error.clear();return true;
}

}
