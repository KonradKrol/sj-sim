#include "PlaygroundExperiment.h"
#include <QSet>
#include <QUuid>
#include "../competitions/CompetitionRules.h"
#include <QThread>
#include <cmath>

namespace Playground {
QJsonArray parameters() {
    QJsonArray result;
    const auto add=[&](const char *key,const char *label,const char *group,double value,double minimum,double maximum,double step,const char *help){result.append(QJsonObject{{"key",key},{"label",QString::fromUtf8(label)},{"group",group},{"default",value},{"minimum",minimum},{"maximum",maximum},{"step",step},{"help",QString::fromUtf8(help)}});};
    add("wind.mean","Średni wiatr","conditions",0,-8,8,.1,"Dodatni: pod narty. Ujemny: w plecy. m/s.");
    add("wind.deviation","Zmienność wiatru","conditions",.5,0,4,.1,"Odchylenie standardowe w m/s. Wiatr jest losowany osobno dla skoku.");
    add("wind.segmentDeviation","Zmienność wzdłuż zeskoku","conditions",.2,0,4,.1,"Odchylenie wiatru pomiędzy pięcioma sektorami. Pozwala badać uśrednianie i rekompensatę przy nierównym wietrze.");
    add("snow","Śnieg na rozbiegu","conditions",0,0,10,.1,"Stały poziom śniegu w eksperymencie.");
    add("gate","Belka","conditions",0,0,100,1,"0 dobiera wspólną belkę z wariantu bazowego. Pozostałe wartości ustalają ją ręcznie.");
    add("importance","Znaczenie skoku","conditions",5,0,10,.1,"Parametr znaczenia używany przez rzeczywisty silnik skoku.");
    add("engine.maxSkills","Zakres umiejętności","engine",100,1,200,1,"Górny limit umiejętności używany przez silnik w tym eksperymencie.");
    add("engine.autoHillEffects","Automatyczne parametry skoczni","engine",0,0,1,1,"Automatyczne dopasowanie parametrów skoczni zgodne z ustawieniem silnika.");
    add("engine.randomness","Losowość silnika","engine",1,0,5,.05,"Mnożnik losowej części symulacji.");
    add("engine.takeoffForm","Wpływ formy na wybicie","engine",.1,0,2,.01,"Współczynnik formy działający tylko w tym eksperymencie.");
    add("engine.flightForm","Wpływ formy na lot","engine",.1,0,2,.01,"Współczynnik formy działający tylko w tym eksperymencie.");
    add("engine.hillEffects","Wpływ skoczni","engine",1,0,5,.05,"Mnożnik charakterystyki skoczni.");
    add("engine.dsq","Parametr DSQ","engine",0,0,10000,1,"0 wyłącza DSQ. Bez dodatkowych cech: 1 = 100%, 100 ≈ 1%, 200 ≈ 0,5%. Większa wartość oznacza mniejsze ryzyko.");
    add("athlete.windBias","Różnica wiatru dla skoczka","athlete",0,-6,6,.1,"Dodatkowy wiatr tylko dla wybranego skoczka. Pozwala porównać korzyść z warunków względem reszty stawki.");
    add("athlete.flightStyle","Styl lotu skoczka","athlete",1,0,3,1,"Styl lotu wybranego skoczka.");
    add("field.flightStyle","Styl lotu stawki","field",1,0,3,1,"Styl lotu pozostałych zawodników.");
    add("athlete.takeoff","Wybicie skoczka","athlete",70,0,200,.5,"Umiejętność wybranego skoczka; reszta stawki pozostaje bez zmian.");
    add("athlete.flight","Lot skoczka","athlete",70,0,200,.5,"Umiejętność wybranego skoczka.");
    add("athlete.form","Forma skoczka","athlete",70,0,200,.5,"Forma wybranego skoczka.");
    add("athlete.landing","Lądowanie skoczka","athlete",10,0,20,.1,"Umiejętność lądowania wybranego skoczka.");
    add("athlete.equality","Równość skoczka","athlete",0,-5,5,.1,"Wpływ równości zgodny z aktualnym silnikiem.");
    add("field.skill","Poziom stawki","field",70,0,200,.5,"Środkowy poziom wybicia i lotu syntetycznej stawki.");
    add("field.spread","Rozpiętość stawki","field",30,0,200,1,"Różnica umiejętności między skrajnymi syntetycznymi zawodnikami.");
    add("field.form","Forma stawki","field",70,0,200,.5,"Forma wszystkich zawodników, z wyjątkiem jawnego ustawienia wybranego skoczka.");
    add("field.landing","Lądowanie stawki","field",10,0,20,.1,"Umiejętność lądowania całej stawki.");
    add("field.equality","Równość stawki","field",0,-5,5,.1,"Równość całej stawki.");
    add("hill.k","Punkt K","hill",120,10,300,.5,"Zmiana K przelicza punkty za metry. HS musi pozostać co najmniej równy K.");
    add("hill.hs","Rozmiar HS","hill",134,10,400,.5,"Rozmiar skoczni; wspólna belka jest wyznaczana z wariantu bazowego.");
    add("hill.balance","Balans wybicie–lot","hill",1,.05,5,.05,"Oryginalny parametr balance skoczni; sprawdź jego efekt porównaniem wariantów.");
    add("hill.distance","Mnożnik odległości skoczni","hill",1,.2,3,.01,"Mnożnik odległości dostępny w danych skoczni.");
    add("hill.headwindPoints","Rekompensata pod narty","hill",8,0,40,.1,"Punkty rekompensaty za 1 m/s.");
    add("hill.tailwindPoints","Rekompensata w plecy","hill",12,0,60,.1,"Punkty rekompensaty za 1 m/s.");
    add("rules.windAverage","Uśrednianie wiatru","rules",0,0,2,1,"Oryginalne metody uśredniania wiatru z silnika gry.");
    add("rules.windDistance","Wiatr a długość skoku","rules",1,0,2,1,"Zasięg sektorów wiatru branych do rekompensaty w zależności od długości skoku.");
    add("rules.windCompensation","Rekompensata za wiatr","rules",1,0,1,1,"Włącza lub wyłącza naliczanie punktów za wiatr.");
    add("rules.gateCompensation","Rekompensata za belkę","rules",1,0,1,1,"Naliczanie punktów za zmianę belki.");
    add("rules.judges","Noty sędziowskie","rules",1,0,1,1,"Uwzględnianie not sędziów w punktach.");
    add("rules.95hs","Reguła 95% HS","rules",1,0,1,1,"Awans po upadku na odległość co najmniej 95% HS, zgodnie z regułami gry.");
    add("experimental.takeoffScale","Mnożnik metrów z wybicia","experimental",1,0,3,.05,"Eksperymentalny mnożnik wkładu wybicia w odległość. 1 zachowuje silnik gry; działa tylko w playgroundzie.");
    add("experimental.flightScale","Mnożnik metrów z lotu","experimental",1,0,3,.05,"Eksperymentalny mnożnik wkładu lotu w odległość.");
    add("experimental.windScale","Mnożnik wpływu wiatru na metry","experimental",1,0,3,.05,"Zmienia fizyczny wpływ wiatru na odległość; rekompensata punktowa pozostaje osobnym mechanizmem.");
    add("rules.dsq","Dyskwalifikacje","rules",1,0,1,1,"Dopuszcza DSQ zgodnie z regułami; prawdopodobieństwo ustala parametr silnika DSQ.");
    add("experimental.takeoffBias","Korekta oceny wybicia","experimental",0,-30,30,.1,"Eksperymentalny bonus z mechanizmu JumpManipulator. Nie zmienia ustawień zwykłej gry.");
    add("experimental.flightBias","Korekta oceny lotu","experimental",0,-30,30,.1,"Eksperymentalny bonus z mechanizmu JumpManipulator.");
    add("experimental.distanceBias","Korekta odległości","experimental",0,-30,30,.1,"Dodatkowe metry z mechanizmu JumpManipulator; wpływają także na lądowanie.");
    for(int i=0;i<result.size();++i){auto p=result[i].toObject();const auto key=p["key"].toString();if(p["maximum"].toInt()==1 && p["minimum"].toInt()==0)p["type"]="boolean";else if(key=="gate" || key=="engine.dsq" || key=="engine.maxSkills" || key.endsWith("flightStyle") || key=="rules.windAverage" || key=="rules.windDistance")p["type"]="integer";if(key.endsWith("flightStyle"))p["options"]=QJsonArray{"V","Nowoczesne V","Szerokie V","H"};if(key=="rules.windAverage")p["options"]=QJsonArray{"Proste","Proste ulepszone","Oryginalne"};if(key=="rules.windDistance")p["options"]=QJsonArray{"Wyłączony","Oryginalny","Pełny"};result[i]=p;}
    return result;
}
QJsonObject example() {
    QJsonObject values;
    for(const auto &entry:parameters()){const auto p=entry.toObject();values.insert(p["key"].toString(),p["default"]);}
    values.remove("hill.k");values.remove("hill.hs");
    return {{"schema",1},{"name",QString::fromUtf8("Wpływ wiatru")},{"seed","20261025"},{"competitions",1000},{"threads",qMax(1,qMin(16,QThread::idealThreadCount()-1))},{"athletes",50},{"focus",24},{"sampleLimit",256},{"hill",QJsonObject{{"name","HS134"},{"k",120},{"hs",134}}},{"rounds",QJsonArray{50,30}},{"parameters",values},{"variants",QJsonArray{QJsonObject{{"name",QString::fromUtf8("Bez wiatru")},{"changes",QJsonObject{{"wind.mean",0}}}},QJsonObject{{"name",QString::fromUtf8("Pod narty")},{"changes",QJsonObject{{"wind.mean",2}}}},QJsonObject{{"name",QString::fromUtf8("W plecy")},{"changes",QJsonObject{{"wind.mean",-2}}}}}}};
}
bool validate(const QJsonObject &config,QString &error) {
    const auto fail=[&](const QString &message){error=message;return false;};
    if(config["schema"].toInt()!=1)return fail(QStringLiteral("Nieobsługiwana wersja konfiguracji playgroundu."));
    const auto integer=[&](const char *key,int minimum,int maximum){const auto value=config[key];return value.isDouble() && std::isfinite(value.toDouble()) && value.toDouble()==std::floor(value.toDouble()) && value.toDouble()>=minimum && value.toDouble()<=maximum;};
    if(!integer("competitions",1,1000000) || !integer("athletes",2,512) || !integer("focus",0,config["athletes"].toInt()-1) || !integer("threads",1,64) || !integer("sampleLimit",0,4096))return fail(QStringLiteral("Sprawdź liczbę konkursów (1–1000000), skoczków (2–512), wybranego skoczka, wątków (1–64) i próbkę (0–4096)."));
    bool seedOk=false;config["seed"].toString().toULongLong(&seedOk);if(!seedOk)return fail(QStringLiteral("Ziarno musi być nieujemną liczbą całkowitą zapisaną jako tekst."));
    QHash<QString,QJsonObject> definitions;for(const auto &entry:parameters()){const auto p=entry.toObject();definitions.insert(p["key"].toString(),p);}
    const auto checkParameters=[&](const QJsonObject &values){for(auto it=values.begin();it!=values.end();++it){
        if(!definitions.contains(it.key())){error=QStringLiteral("Nieznany parametr: ")+it.key();return false;}
        const auto definition=definitions[it.key()];const auto label=definition["label"].toString();
        if(!it.value().isDouble() || !std::isfinite(it.value().toDouble())){error=label+QStringLiteral(": wymagana jest skończona liczba.");return false;}
        if((definition["type"].toString()=="integer" || definition["type"].toString()=="boolean") && it.value().toDouble()!=std::floor(it.value().toDouble())){error=label+QStringLiteral(": wymagana jest liczba całkowita.");return false;}
        if(it.value().toDouble()<definition["minimum"].toDouble() || it.value().toDouble()>definition["maximum"].toDouble()){error=QStringLiteral("%1: zakres %2–%3.").arg(label).arg(definition["minimum"].toDouble()).arg(definition["maximum"].toDouble());return false;}
    }return true;};
    if(!config["parameters"].isObject())return fail(QStringLiteral("Parametry muszą być obiektem JSON."));
    if(!checkParameters(config["parameters"].toObject()))return false;
    const auto variants=config["variants"].toArray();if(variants.isEmpty() || variants.size()>32)return fail(QStringLiteral("Wybierz od 1 do 32 wariantów."));
    QSet<QString> names;for(const auto &entry:variants){if(!entry.isObject())return fail(QStringLiteral("Wariant musi być obiektem."));const auto variant=entry.toObject();const auto name=variant["name"].toString().trimmed();if(name.isEmpty() || names.contains(name) || !variant["changes"].isObject())return fail(QStringLiteral("Warianty wymagają różnych nazw i poprawnych zmian parametrów."));if(!checkParameters(variant["changes"].toObject())){error=name+": "+error;return false;}if(config["roster"].isArray() && (variant["changes"].toObject().contains("field.skill") || variant["changes"].toObject().contains("field.spread")))return fail(name+QStringLiteral(": poziom i rozpiętość dotyczą stawki syntetycznej. Wybierz inny parametr skanowania lub stawkę syntetyczną."));names.insert(name);}
    const auto rounds=config["rounds"].toArray();if(rounds.isEmpty() || rounds.size()>8)return fail(QStringLiteral("Wybierz od 1 do 8 serii."));int previous=config["athletes"].toInt();for(const auto &value:rounds){if(!value.isDouble() || value.toDouble()!=std::floor(value.toDouble()) || value.toInt()<1 || value.toInt()>previous)return fail(QStringLiteral("Limity serii muszą być dodatnie i nie mogą rosnąć."));previous=value.toInt();}
    const auto hill=config["hill"].toObject();const double k=hill["k"].toDouble(),hs=hill["hs"].toDouble();if(!std::isfinite(k) || !std::isfinite(hs) || k<10 || k>300 || hs<k || hs>400)return fail(QStringLiteral("Skocznia wymaga K 10–300 i HS od K do 400."));
    if(config.contains("roster") && (!config["roster"].isArray() || config["roster"].toArray().size()!=config["athletes"].toInt()))return fail(QStringLiteral("Liczba zawodników musi odpowiadać zaimportowanej stawce."));
    if(config.contains("rulesData")){
        if(!config["rulesData"].isObject() || QUuid(config["rulesData"].toObject()["id"].toString()).isNull())return fail(QStringLiteral("Nieprawidłowy preset zasad lub jego identyfikator."));
        const auto rules=CompetitionRules::getFromJson(config["rulesData"].toObject());
        if(rules.getCompetitionType()!=CompetitionRules::Individual && rules.getCompetitionType()!=CompetitionRules::Team)return fail(QStringLiteral("Nieprawidłowy rodzaj konkursu."));
        if(rules.getRounds().isEmpty() || rules.getRounds().size()>8 || (rules.getCompetitionType()==CompetitionRules::Team && (rules.getJumpersInTeamCount()<1 || rules.getJumpersInTeamCount()>8)))return fail(QStringLiteral("Nieprawidłowe serie lub skład drużyny w presecie."));
        for(const auto &round:rules.getRounds())if(round.getCount()<0 || round.getCount()>512 || (round.getKO() && (rules.getCompetitionType()==CompetitionRules::Team || round.getCountInKOGroup()<1 || round.getCountInKOGroup()>512 || round.getAdvancingFromKOGroup()<1 || round.getAdvancingFromKOGroup()>round.getCountInKOGroup() || round.getKoGroupsSelectionType()<0 || round.getKoGroupsSelectionType()>4)))return fail(QStringLiteral("Nieprawidłowe limity lub grupy KO w presecie."));
    }
    for(const auto &variant:variants){auto combined=config["parameters"].toObject();const auto changes=variant.toObject()["changes"].toObject();for(auto it=changes.begin();it!=changes.end();++it)combined[it.key()]=it.value();const double variantK=combined["hill.k"].toDouble(k),variantHS=combined["hill.hs"].toDouble(hs);if(variantHS<variantK)return fail(QStringLiteral("W każdym wariancie HS musi być co najmniej równe K."));}
    const auto validObject=[](const QJsonObject &object){if(QUuid(object["id"].toString()).isNull())return false;for(auto it=object.begin();it!=object.end();++it)if(it.value().isDouble() && !std::isfinite(it.value().toDouble()))return false;for(const auto &entry:object["characteristics"].toArray()){const auto level=entry.toObject()["level"];if(!level.isDouble() || !std::isfinite(level.toDouble()) || std::abs(level.toDouble())>1000000)return false;}return true;};
    if(hill.contains("source") && (!hill["source"].isObject() || !validObject(hill["source"].toObject())))return fail(QStringLiteral("Nieprawidłowe źródło danych skoczni."));
    for(const auto &entry:config["roster"].toArray())if(!entry.isObject() || !validObject(entry.toObject()))return fail(QStringLiteral("Nieprawidłowe dane zaimportowanego skoczka."));
    error.clear();return true;
}
}
