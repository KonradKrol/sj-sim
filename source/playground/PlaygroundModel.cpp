#include "PlaygroundModel.h"
#include <QObject>
#include <type_traits>
#include <cmath>
#include <stdexcept>

namespace Playground {
namespace {
template<class F> void visitEngine(SimulationV2::EngineParameters &m,F f) {
    f("quality.overallWeight",QObject::tr("Udział OVR"),"quality",m.quality.overallWeight,0,1,0.01,"1");
    f("quality.negativeInfluence",QObject::tr("Udział ujemnego wpływu losowości"),"quality",m.quality.negativeInfluence,0,1,0.01,"1");
    f("quality.noise",QObject::tr("Rozkład: 0 normalny, 1 Laplace’a"),"quality",m.quality.noise,0,1,1,"1");
    f("quality.hillPreferenceStrength",QObject::tr("Wpływ preferencji skoczni"),"quality",m.quality.hillPreferenceStrength,0,10,0.05,"1");
    f("flight.referenceQuality",QObject::tr("Jakość odniesienia"),"flight",m.flight.referenceQuality,1,100,1,"1");
    f("flight.referenceDistanceAtK",QObject::tr("Odległość odniesienia / K"),"flight",m.flight.referenceDistanceAtK,0,2,0.01,"1");
    f("flight.qualityMetresAtK120",QObject::tr("Metry za punkt jakości na K120"),"flight",m.flight.qualityMetresAtK120,0,10,0.01,"m");
    f("flight.referenceGate",QObject::tr("Belka odniesienia"),"flight",m.flight.referenceGate,0,200,1,"1");
    f("flight.gateMetresAtK120",QObject::tr("Metry za belkę na K120"),"flight",m.flight.gateMetresAtK120,0,20,0.1,"m");
    f("flight.gateTailwindInteraction",QObject::tr("Dodatkowy wpływ belki w plecy"),"flight",m.flight.gateTailwindInteraction,0,1,0.01,"1");
    f("flight.precipitationLossPerLevel",QObject::tr("Strata zapasu za poziom śniegu"),"flight",m.flight.precipitationLossPerLevel,0,0.1,0.001,"1");
    f("flight.rainLossPerLevel",QObject::tr("Strata zapasu za poziom deszczu"),"flight",m.flight.rainLossPerLevel,0,0.1,0.001,"1");
    f("flight.rainLossThreshold",QObject::tr("Próg wpływu deszczu"),"flight",m.flight.rainLossThreshold,0,10,0.1,"1");
    f("flight.rainHeavyThreshold",QObject::tr("Próg ulewy"),"flight",m.flight.rainHeavyThreshold,0,9.9,0.1,"1");
    f("flight.rainHeavyLossAtMaximum",QObject::tr("Strata zapasu w maksymalnej ulewie"),"flight",m.flight.rainHeavyLossAtMaximum,0,0.5,0.01,"1");
    f("flight.rainHeavyExponent",QObject::tr("Wykładnik wpływu ulewy"),"flight",m.flight.rainHeavyExponent,1,6,0.1,"1");
    f("flight.stepMetres",QObject::tr("Krok całkowania"),"flight",m.flight.stepMetres,0.05,5,0.05,"m");
    f("flight.secondsPerK",QObject::tr("Czas lotu do K"),"flight",m.flight.secondsPerK,0.1,30,0.1,"s");
    f("flight.windReservePerMetre",QObject::tr("Wpływ wiatru na zapas lotu"),"flight",m.flight.windReservePerMetre,0,1,0.001,"1");
    f("flight.crosswindDrag",QObject::tr("Opór wiatru bocznego"),"flight",m.flight.crosswindDrag,0,1,0.001,"1");
    f("flight.windResponseMetres",QObject::tr("Dystans narastania reakcji na wiatr"),"flight",m.flight.windResponseMetres,0,100,0.5,"m");
    f("flight.earlyWindSensitivity",QObject::tr("Wrażliwość na wiatr na początku"),"flight",m.flight.earlyWindSensitivity,0,10,0.05,"1");
    f("flight.lateWindSensitivity",QObject::tr("Wrażliwość na wiatr w końcu lotu"),"flight",m.flight.lateWindSensitivity,0,10,0.05,"1");
    f("flight.maximumWindAssistance",QObject::tr("Maksymalna pomoc wiatru"),"flight",m.flight.maximumWindAssistance,0,0.99,0.01,"1");
    f("flight.geometryStartRealHs",QObject::tr("Początek oporu zeskoku / real HS"),"flight",m.flight.geometryStartRealHs,0.5,1,0.01,"1");
    f("flight.usableLandingBeyondRealHsAtK",QObject::tr("Przedłużenie zeskoku / K"),"flight",m.flight.usableLandingBeyondRealHsAtK,0.01,0.5,0.01,"1");
    f("flight.geometryResistance",QObject::tr("Opór wypłaszczonego zeskoku"),"flight",m.flight.geometryResistance,0.01,100,0.1,"1");
    f("flight.exposureSections",QObject::tr("Liczba odcinków zapisu lotu"),"flight",m.flight.exposureSections,1,32,1,"1");
    f("landing.fallBase",QObject::tr("Upadek: ryzyko bazowe"),"landing",m.landing.fallBase,0,1,0.001,"1");
    f("landing.fallIncrease",QObject::tr("Upadek: wzrost ryzyka"),"landing",m.landing.fallIncrease,0,1,0.01,"1");
    f("landing.fallCentre",QObject::tr("Upadek: środek krzywej / real HS"),"landing",m.landing.fallCentre,0.5,2,0.005,"1");
    f("landing.fallWidth",QObject::tr("Upadek: szerokość krzywej"),"landing",m.landing.fallWidth,0.001,1,0.001,"1");
    f("landing.supportBase",QObject::tr("Podpórka: ryzyko bazowe"),"landing",m.landing.supportBase,0,1,0.001,"1");
    f("landing.supportIncrease",QObject::tr("Podpórka: wzrost ryzyka"),"landing",m.landing.supportIncrease,0,1,0.01,"1");
    f("landing.supportCentre",QObject::tr("Podpórka: środek krzywej / real HS"),"landing",m.landing.supportCentre,0.5,2,0.005,"1");
    f("landing.supportWidth",QObject::tr("Podpórka: szerokość krzywej"),"landing",m.landing.supportWidth,0.001,1,0.001,"1");
    f("landing.twoFeetBase",QObject::tr("Dwie nogi: ryzyko bazowe"),"landing",m.landing.twoFeetBase,0,1,0.001,"1");
    f("landing.twoFeetIncrease",QObject::tr("Dwie nogi: wzrost ryzyka"),"landing",m.landing.twoFeetIncrease,0,1,0.01,"1");
    f("landing.twoFeetCentre",QObject::tr("Dwie nogi: środek krzywej / real HS"),"landing",m.landing.twoFeetCentre,0.5,2,0.005,"1");
    f("landing.twoFeetWidth",QObject::tr("Dwie nogi: szerokość krzywej"),"landing",m.landing.twoFeetWidth,0.001,1,0.001,"1");
    f("landing.crosswindFallPerMetrePerSecond",QObject::tr("Wiatr boczny: wzrost ryzyka upadku"),"landing",m.landing.crosswindFallPerMetrePerSecond,0,1,0.001,"1");
    f("landing.crosswindSupportPerMetrePerSecond",QObject::tr("Wiatr boczny: wzrost ryzyka podpórki"),"landing",m.landing.crosswindSupportPerMetrePerSecond,0,1,0.001,"1");
    f("landing.crosswindTwoFeetPerMetrePerSecond",QObject::tr("Wiatr boczny: wzrost ryzyka dwóch nóg"),"landing",m.landing.crosswindTwoFeetPerMetrePerSecond,0,1,0.001,"1");
    f("landing.sharedStyleDeviation",QObject::tr("Wspólne odchylenie not"),"landing",m.landing.sharedStyleDeviation,0,10,0.05,"1");
    f("landing.judgeDeviation",QObject::tr("Odchylenie pojedynczego sędziego"),"landing",m.landing.judgeDeviation,0,10,0.05,"1");
    f("landing.difficultLandingStylePenalty",QObject::tr("Kara za trudne lądowanie"),"landing",m.landing.difficultLandingStylePenalty,0,20,0.1,"1");
    for(std::size_t i=0;i<m.quality.eventDeviation.size();++i) f(QString("quality.eventDeviation.%1").arg(i),QObject::tr("Odchylenie serii")+QString(" [%1]").arg(i),"quality",m.quality.eventDeviation[i],0,100,.01,"1");
    for(std::size_t i=0;i<m.quality.specialistBias.size();++i) f(QString("quality.specialistBias.%1").arg(i),QObject::tr("Siła specjalizacji")+QString(" [%1]").arg(i),"quality",m.quality.specialistBias[i],0,100,.01,"1");
    for(std::size_t i=0;i<m.quality.trainingBonus.size();++i) f(QString("quality.trainingBonus.%1").arg(i),QObject::tr("Bonus treningowy")+QString(" [%1]").arg(i),"quality",m.quality.trainingBonus[i],0,100,.01,"1");
    for(std::size_t i=0;i<m.quality.competitionPenalty.size();++i) f(QString("quality.competitionPenalty.%1").arg(i),QObject::tr("Kara konkursowa")+QString(" [%1]").arg(i),"quality",m.quality.competitionPenalty[i],0,100,.01,"1");
    for(std::size_t i=0;i<m.quality.conservedEffortPenalty.size();++i) f(QString("quality.conservedEffortPenalty.%1").arg(i),QObject::tr("Oszczędzanie: kara")+QString(" [%1]").arg(i),"quality",m.quality.conservedEffortPenalty[i],0,100,.01,"1");
    for(std::size_t i=0;i<m.quality.conservedEffortDeviation.size();++i) f(QString("quality.conservedEffortDeviation.%1").arg(i),QObject::tr("Oszczędzanie: odchylenie")+QString(" [%1]").arg(i),"quality",m.quality.conservedEffortDeviation[i],0,100,.01,"1");
    for(std::size_t i=0;i<m.quality.regularNegative.size();++i) f(QString("quality.regularNegative.%1").arg(i),QObject::tr("Regularny: straty")+QString(" [%1]").arg(i),"quality",m.quality.regularNegative[i],0,100,.01,"1");
    for(std::size_t i=0;i<m.quality.regularPositive.size();++i) f(QString("quality.regularPositive.%1").arg(i),QObject::tr("Regularny: bonusy")+QString(" [%1]").arg(i),"quality",m.quality.regularPositive[i],0,100,.01,"1");
    for(std::size_t i=0;i<m.quality.unpredictableNegative.size();++i) f(QString("quality.unpredictableNegative.%1").arg(i),QObject::tr("Nieprzewidywalny: straty")+QString(" [%1]").arg(i),"quality",m.quality.unpredictableNegative[i],0,100,.01,"1");
    for(std::size_t i=0;i<m.quality.unpredictablePositive.size();++i) f(QString("quality.unpredictablePositive.%1").arg(i),QObject::tr("Nieprzewidywalny: bonusy")+QString(" [%1]").arg(i),"quality",m.quality.unpredictablePositive[i],0,100,.01,"1");
    for(std::size_t i=0;i<m.landing.styleMean.size();++i) f(QString("landing.styleMean.%1").arg(i),QObject::tr("Średnia nota klasy lądowania")+QString(" [%1]").arg(i),"landing",m.landing.styleMean[i],0,20,.01,"1");
    for(std::size_t i=0;i<m.landing.landingPenalty.size();++i) f(QString("landing.landingPenalty.%1").arg(i),QObject::tr("Kara za rodzaj lądowania")+QString(" [%1]").arg(i),"landing",m.landing.landingPenalty[i],0,20,.01,"1");
    f("curve.extension",QObject::tr("Poza krzywą: 0 stała, 1 liniowa"),"quality",m.curveExtension,0,1,1,"1");
    for(std::size_t i=0;i<m.skillAnchors.size();++i) {
        f(QString("curve.k.%1").arg(i),QObject::tr("Punkt krzywej K")+QString(" [%1]").arg(i),"quality",m.skillAnchors[i].k,1,10000,1,"m");
        f(QString("curve.takeoff.%1").arg(i),QObject::tr("Udział wybicia")+QString(" [%1]").arg(i),"quality",m.skillAnchors[i].takeoff,0,1,.01,"1");
    }
}
template<class F> void visitWind(SimulationV2::WindParameters &m,F f) {
    f("wind.temporalAmplitude",QObject::tr("Amplituda zmian w czasie"),"wind",m.temporalAmplitude,0,100,0.1,"m/s");
    f("wind.spatialAmplitude",QObject::tr("Amplituda różnic wzdłuż zeskoku"),"wind",m.spatialAmplitude,0,100,0.1,"m/s");
    f("wind.correlationSeconds",QObject::tr("Skala korelacji w czasie"),"wind",m.correlationSeconds,0.1,100000,1,"s");
    f("wind.correlationMetres",QObject::tr("Skala korelacji wzdłuż zeskoku"),"wind",m.correlationMetres,0.1,100000,1,"m");
    f("wind.gustAmplitude",QObject::tr("Amplituda podmuchów"),"wind",m.gustAmplitude,0,100,0.1,"m/s");
    f("wind.gustProbability",QObject::tr("Prawdopodobieństwo podmuchu"),"wind",m.gustProbability,0,1,0.01,"1");
    f("wind.gustPeriodSeconds",QObject::tr("Okres podmuchów"),"wind",m.gustPeriodSeconds,0.1,100000,1,"s");
    f("wind.gustWidthMetres",QObject::tr("Szerokość podmuchu"),"wind",m.gustWidthMetres,0.1,100000,1,"m");
    f("wind.gustTravelMetresPerSecond",QObject::tr("Prędkość przemieszczania podmuchu"),"wind",m.gustTravelMetresPerSecond,0,100,0.1,"m");
    f("wind.maximumStrength",QObject::tr("Limit siły wiatru"),"wind",m.maximumStrength,0.01,100,0.1,"m/s");
    f("wind.trendAmplitude",QObject::tr("Amplituda zmian tendencji"),"wind",m.trendAmplitude,0,100,0.1,"m/s");
    f("wind.trendPeriodSeconds",QObject::tr("Okres tendencji"),"wind",m.trendPeriodSeconds,1,100000,1,"s");
    f("wind.trendTransitionSeconds",QObject::tr("Czas zmiany tendencji"),"wind",m.trendTransitionSeconds,0.1,30000,0.1,"s");
    f("wind.activityVariation",QObject::tr("Zmienność aktywności"),"wind",m.activityVariation,0,1,0.01,"1");
    f("wind.activityCorrelationSeconds",QObject::tr("Czas epizodu aktywności"),"wind",m.activityCorrelationSeconds,0.1,100000,1,"s");
    f("wind.fastCorrelationSeconds",QObject::tr("Skala szybkich zmian"),"wind",m.fastCorrelationSeconds,0.1,100000,0.1,"s");
}
template<class F> void visitPrecipitation(SimulationV2::PrecipitationParameters &m,F f) {
    f("precipitation.scenario",QObject::tr("Przebieg: brak / stały / zmienny / silny / narastający"),"precipitation",m.scenario,0,4,1,"1");
    f("precipitation.intensity",QObject::tr("Natężenie opadów"),"precipitation",m.intensity,0,10,0.1,"1");
    f("precipitation.variation",QObject::tr("Zmienność opadów"),"precipitation",m.variation,0,10,0.1,"1");
    f("precipitation.correlationSeconds",QObject::tr("Skala korelacji w czasie"),"precipitation",m.correlationSeconds,0.1,100000,1,"s");
    f("precipitation.increasePerMinute",QObject::tr("Przyrost opadów na minutę"),"precipitation",m.increasePerMinute,0,10,0.01,"1");
    f("precipitation.kind",QObject::tr("Rodzaj: 0 śnieg, 1 deszcz"),"precipitation",m.kind,0,1,1,"1");
}
template<class F> void visitMeasurement(SimulationV2::FieldWindMeasurementParameters &m,F f) {
    f("measurement.temporalSamples",QObject::tr("Liczba próbek pomiaru"),"measurement",m.temporalSamples,1,257,1,"1");
    f("measurement.extentK",QObject::tr("Zasięg pomiaru / K"),"measurement",m.extentK,0.5,2,0.05,"1");
    f("measurement.denseSpacingMetres",QObject::tr("Rozstaw gęstej sieci"),"measurement",m.denseSpacingMetres,1,50,1,"m");
    f("measurement.weightPeak",QObject::tr("Położenie szczytu wag / zasięg"),"measurement",m.weightPeak,0.001,0.999,0.01,"1");
    f("measurement.weightStrength",QObject::tr("Siła ważenia pomiaru"),"measurement",m.weightStrength,0,10,0.1,"1");
}
QString effectDescription(const QString &key,const QString &group) {
    if(key=="quality.overallWeight")return QObject::tr("Udział OVR w jakości bazowej. Pozostała część pochodzi z formy; 0,70 oznacza 70% OVR i 30% formy.");
    if(key=="quality.negativeInfluence")return QObject::tr("Udział siły losowych strat w łącznym wpływie losowości, nie odsetek słabych skoków.");
    if(key.startsWith("curve."))return QObject::tr("Punkty K muszą rosnąć, a udział wybicia nie może rosnąć. Krzywa dotyczy preferencji skoczni i specjalizacji, nie ukrytego skilla.");
    if(group=="quality")return QObject::tr("Parametr jakości lub perka. Poziomy perków: 0 brak, 1–3 siła. Serie: 0 trening, 1 próba, 2 kwalifikacje, 3 konkurs.");
    if(group=="wind")return QObject::tr("Amplitudy zmieniają siłę wahań; skale czasu i odległości ich tempo oraz podobieństwo. Korekty punktów dodają się do wspólnego wiatru. Nie zmienia to metody pomiaru.");
    if(group=="measurement")return QObject::tr("Zmienia odczyt wiatru do punktacji, nie fizyczny lot. Wagi przybliżenia FIS są parametrami gry.");
    if(group=="flight")return QObject::tr("Zmienia zapas lotu, jego zużycie lub reakcję na warunki. Opór geometrii działa w trakcie lotu; nie skraca gotowego wyniku.");
    if(group=="landing")return QObject::tr("Środki krzywych odnoszą się do odległości / real HS, szerokość steruje łagodnością przejścia. Klasa lądowania zmienia noty, nie prawdopodobieństwa lądowania.");
    if(group=="precipitation")return QObject::tr("Steruje opadami w czasie. Fizyczny wpływ śniegu i deszczu ma osobne współczynniki w grupie Lot i geometria.");
    if(group=="athlete" || group=="field")return QObject::tr("Dodatnie perki: błyszczy na treningach, lotnik, regularny. Ujemne: oszczędza siły, małe skocznie, nieprzewidywalny. Klasa lądowania 0–4 wpływa tylko na noty.");
    if(group=="hill")return QObject::tr("Geometria, preferencja i punktacja skoczni są niezależne. Sam punkt K nie podmienia ręcznie ustawionej punktacji.");
    if(group=="analysis")return QObject::tr("Zmienia zakres lub dokładność analizy. Gęstsze próbkowanie zwiększa koszt, ale nie zmienia pola pogody ani silnika.");
    if(group=="rules")return QObject::tr("Włącza składnik punktacji lub regułę eksperymentu. Dane normalnej gry pozostają bez zmian.");
    return QObject::tr("W trybie kontrolowanym wszystkie starty mają czas 0. W przebiegu konkursu wspólny zegar przesuwa się między skokami. Belka 0 dobiera wspólną belkę z bazy.");
}
QJsonObject definition(const QString &key,const QString &label,const QString &group,double value,double lo,double hi,double step,const QString &unit,bool integer) {
    return {{"key",key},{"label",label},{"group",group},{"default",value},{"minimum",lo},{"maximum",hi},{"step",step},{"unit",unit},{"type",integer?"integer":"number"},{"help",effectDescription(key,group)+"\n"+QObject::tr("%1 · jednostka: %2 · domyślnie: %3").arg(key,unit).arg(value,0,'g',9)}};
}
}
QJsonArray parameters() {
    QJsonArray rows;
    const auto add=[&](const QString &key,const QString &label,const QString &group,auto &value,double lo,double hi,double step,const QString &unit){rows.append(definition(key,label,group,double(value),lo,hi,step,unit,!std::is_floating_point<typename std::decay<decltype(value)>::type>::value));};
    SimulationV2::EngineParameters engine;visitEngine(engine,add);
    SimulationV2::WindParameters wind;visitWind(wind,add);
    SimulationV2::PrecipitationParameters precipitation;visitPrecipitation(precipitation,add);
    SimulationV2::FieldWindMeasurementParameters measurement;visitMeasurement(measurement,add);
    const auto input=[&](const char *key,const QString &label,const char *group,double value,double lo,double hi,double step,const char *unit="1"){rows.append(definition(key,label,group,value,lo,hi,step,unit,step==1));};
    input("gate",QObject::tr("Belka (0: automatyczna)"),"conditions",0,0,200,1,"1");
    input("event",QObject::tr("Seria: trening / próba / kwalifikacje / konkurs"),"conditions",3,0,3,1,"1");
    input("clock.mode",QObject::tr("Warunki: 0 kontrolowane, 1 przebieg konkursu"),"conditions",1,0,1,1,"1");
    input("clock.interval",QObject::tr("Czas między skokami"),"conditions",45,1,600,1,"s");
    input("dsq",QObject::tr("Ryzyko DSQ"),"conditions",0,0,1,0.001,"1");
    input("field.skill",QObject::tr("OVR stawki"),"field",70,1,100,1,"1");
    input("field.spread",QObject::tr("Rozpiętość OVR stawki"),"field",30,0,99,1,"1");
    input("field.form",QObject::tr("Forma stawki"),"field",70,1,100,1,"1");
    input("field.landing",QObject::tr("Klasa lądowania stawki"),"field",2,0,4,1,"1");
    input("athlete.overall",QObject::tr("OVR skoczka"),"athlete",70,1,100,1,"1");
    input("athlete.form",QObject::tr("Forma skoczka"),"athlete",70,1,100,1,"1");
    input("athlete.landing",QObject::tr("Klasa lądowania skoczka"),"athlete",2,0,4,1,"1");
    input("hill.k",QObject::tr("Punkt K"),"hill",120,1,400,0.5,"m");
    input("hill.hs",QObject::tr("HS"),"hill",134,1,500,0.5,"m");
    input("hill.realHs",QObject::tr("Początek wypłaszczenia"),"hill",134,0.5,600,0.5,"m");
    input("hill.takeoffBias",QObject::tr("Preferencja: lot (−1), wybicie (+1)"),"hill",0,-1,1,0.05,"1");
    input("hill.pointsAtK",QObject::tr("Punkty za K"),"hill",60,0,200,0.1,"pkt");
    input("hill.pointsPerMetre",QObject::tr("Punkty za metr"),"hill",1.8,0,20,0.1,"pkt/m");
    input("hill.gatePoints",QObject::tr("Punkty za belkę"),"hill",5,0,100,0.1,"pkt");
    input("hill.headwindPoints",QObject::tr("Rekompensata pod narty"),"hill",8,0,100,0.1,"pkt/(m/s)");
    input("hill.tailwindPoints",QObject::tr("Rekompensata w plecy"),"hill",12,0,100,0.1,"pkt/(m/s)");
    input("wind.mean",QObject::tr("Wiatr przedni (+) / tylny (−)"),"wind",0,-100,100,0.1,"m/s");
    input("wind.cross",QObject::tr("Wiatr boczny"),"wind",0,-100,100,0.1,"m/s");
    input("rules.measurement",QObject::tr("Metoda pomiaru wiatru (0–5)"),"measurement",3,0,5,1,"1");
    input("rules.windCompensation",QObject::tr("Rekompensata za wiatr"),"rules",1,0,1,1,"1");
    input("rules.gateCompensation",QObject::tr("Rekompensata za belkę"),"rules",1,0,1,1,"1");
    input("rules.judges",QObject::tr("Noty za styl"),"rules",1,0,1,1,"1");
    input("rules.95hs",QObject::tr("Reguła 95% HS"),"rules",1,0,1,1,"1");
    input("athlete.perk.0",QObject::tr("Podejście (−3…+3)"),"athlete",0,-3,3,1,"1");
    input("athlete.perk.1",QObject::tr("Specjalizacja (−3…+3)"),"athlete",0,-3,3,1,"1");
    input("athlete.perk.2",QObject::tr("Regularność (−3…+3)"),"athlete",0,-3,3,1,"1");
    input("field.perk.0",QObject::tr("Podejście (−3…+3)"),"field",0,-3,3,1,"1");
    input("field.perk.1",QObject::tr("Specjalizacja (−3…+3)"),"field",0,-3,3,1,"1");
    input("field.perk.2",QObject::tr("Regularność (−3…+3)"),"field",0,-3,3,1,"1");
    input("wind.reference.0.head",QObject::tr("Punkt 1: przód / tył"),"wind",0,-100,100,0.1,"m/s");
    input("wind.reference.0.cross",QObject::tr("Punkt 1: bok"),"wind",0,-100,100,0.1,"m/s");
    input("wind.reference.1.head",QObject::tr("Punkt 2: przód / tył"),"wind",0,-100,100,0.1,"m/s");
    input("wind.reference.1.cross",QObject::tr("Punkt 2: bok"),"wind",0,-100,100,0.1,"m/s");
    input("wind.reference.2.head",QObject::tr("Punkt 3: przód / tył"),"wind",0,-100,100,0.1,"m/s");
    input("wind.reference.2.cross",QObject::tr("Punkt 3: bok"),"wind",0,-100,100,0.1,"m/s");
    input("wind.reference.3.head",QObject::tr("Punkt 4: przód / tył"),"wind",0,-100,100,0.1,"m/s");
    input("wind.reference.3.cross",QObject::tr("Punkt 4: bok"),"wind",0,-100,100,0.1,"m/s");
    input("wind.reference.4.head",QObject::tr("Punkt 5: przód / tył"),"wind",0,-100,100,0.1,"m/s");
    input("wind.reference.4.cross",QObject::tr("Punkt 5: bok"),"wind",0,-100,100,0.1,"m/s");
    input("wind.reference.5.head",QObject::tr("Punkt 6: przód / tył"),"wind",0,-100,100,0.1,"m/s");
    input("wind.reference.5.cross",QObject::tr("Punkt 6: bok"),"wind",0,-100,100,0.1,"m/s");
    input("analysis.duration",QObject::tr("Horyzont analizy wiatru"),"analysis",3600,60,86400,60,"s");
    input("analysis.time",QObject::tr("Chwila przekroju zeskoku"),"analysis",0,0,86400,1,"s");
    input("analysis.distance",QObject::tr("Miejsce odczytu w czasie"),"analysis",78,0,600,.5,"m");
    for(int i=0;i<6;++i)input(QString("wind.reference.%1.positionK").arg(i).toUtf8().constData(),QObject::tr("Pozycja punktu / K")+QString(" [%1]").arg(i+1),"wind",1.2*i/5,0,5,.01,"K");
    input("analysis.scale",QObject::tr("Skala mapy wiatru ±"),"analysis",3,.1,100,.1,"m/s");
    input("analysis.timeSamples",QObject::tr("Próbki czasu analizy"),"analysis",241,2,2001,1,"1");
    input("analysis.spaceSamples",QObject::tr("Próbki odległości analizy"),"analysis",49,2,201,1,"1");
    for(int i=0;i<rows.size();++i){auto d=rows[i].toObject();auto k=d["key"].toString();if(k=="clock.mode")d["options"]=QJsonArray{QObject::tr("Kontrolowane warunki"),QObject::tr("Przebieg konkursu")};if(k=="event")d["options"]=QJsonArray{QObject::tr("Trening"),QObject::tr("Seria próbna"),QObject::tr("Kwalifikacje"),QObject::tr("Konkurs")};if(k=="quality.noise")d["options"]=QJsonArray{QObject::tr("Normalny"),QObject::tr("Laplace’a")};if(k=="rules.measurement")d["options"]=QJsonArray{QObject::tr("Dotychczasowy Sj.Sim"),QObject::tr("Odczyt przed skokiem"),QObject::tr("Pomiar przez czas lotu"),QObject::tr("FIS — przybliżenie"),QObject::tr("Gęsta sieć czujników"),QObject::tr("Wiatr na trasie lotu")};if(k=="precipitation.kind")d["options"]=QJsonArray{QObject::tr("Śnieg"),QObject::tr("Deszcz")};if(k=="precipitation.scenario")d["options"]=QJsonArray{QObject::tr("Bez opadów"),QObject::tr("Stałe"),QObject::tr("Zmienne"),QObject::tr("Silne"),QObject::tr("Narastające")};rows[i]=d;}
    input("analysis.jumpSamples",QObject::tr("Skoki w próbie analizy wiatru"),"analysis",512,16,10000,1,"1");
    input("rules.dsq",QObject::tr("Dyskwalifikacje"),"rules",1,0,1,1,"1");
    return rows;
}
SimulationV2::EngineParameters engineParameters(const QJsonObject &p) {
    SimulationV2::EngineParameters result;
    visitEngine(result,[&](const QString &key,const QString &,const QString &,auto &field,double,double,double,const QString &){if(p.contains(key))field=static_cast<typename std::decay<decltype(field)>::type>(p[key].toDouble());});
    SimulationV2::Engine verified(result);
    return result;
}
SimulationV2::WindParameters windParameters(const QJsonObject &p) {
    SimulationV2::WindParameters result;
    visitWind(result,[&](const QString &key,const QString &,const QString &,auto &field,double,double,double,const QString &){if(p.contains(key))field=static_cast<typename std::decay<decltype(field)>::type>(p[key].toDouble());});
    result.references.clear();for(int i=0;i<6;++i)result.references.push_back({p["hill.k"].toDouble()*p[QString("wind.reference.%1.positionK").arg(i)].toDouble(),{p["wind.mean"].toDouble()+p[QString("wind.reference.%1.head").arg(i)].toDouble(),p["wind.cross"].toDouble()+p[QString("wind.reference.%1.cross").arg(i)].toDouble()}});
    result.validate();
    return result;
}
SimulationV2::PrecipitationParameters precipitationParameters(const QJsonObject &p) {
    SimulationV2::PrecipitationParameters result;
    visitPrecipitation(result,[&](const QString &key,const QString &,const QString &,auto &field,double,double,double,const QString &){if(p.contains(key))field=static_cast<typename std::decay<decltype(field)>::type>(p[key].toDouble());});
    result.validate();
    return result;
}
SimulationV2::FieldWindMeasurementParameters measurementParameters(const QJsonObject &p) {
    SimulationV2::FieldWindMeasurementParameters result;
    visitMeasurement(result,[&](const QString &key,const QString &,const QString &,auto &field,double,double,double,const QString &){if(p.contains(key))field=static_cast<typename std::decay<decltype(field)>::type>(p[key].toDouble());});
    result.validate();
    return result;
}
SimulationV2::Hill hillInput(const QJsonObject &p) {
    SimulationV2::Hill h;
    h.k=p["hill.k"].toDouble();
    h.hs=p["hill.hs"].toDouble();
    h.realHs=p["hill.realHs"].toDouble();
    h.takeoffBias=p["hill.takeoffBias"].toDouble();
    h.pointsAtK=p["hill.pointsAtK"].toDouble();
    h.pointsPerMetre=p["hill.pointsPerMetre"].toDouble();
    h.gatePoints=p["hill.gatePoints"].toDouble();
    h.headwindPoints=p["hill.headwindPoints"].toDouble();
    h.tailwindPoints=p["hill.tailwindPoints"].toDouble();
    h.validate();return h;
}
SimulationV2::Athlete athleteInput(const QJsonObject &p,const QString &prefix) {
    SimulationV2::Athlete a;a.overall=p[prefix+".overall"].toInt(70);a.form=p[prefix+".form"].toInt(70);a.landing=p[prefix+".landing"].toInt(2);
    for(int i=0;i<3;++i)a.perks[i]=p[prefix+QString(".perk.%1").arg(i)].toInt();a.validate();return a;
}
}
