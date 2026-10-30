#include "PlaygroundModel.h"
#include "../simulation-v2/WeatherCalibration.h"
#include <QObject>
#include <QSet>
#include <type_traits>
#include <cmath>
#include <stdexcept>

namespace Playground {
void validateGenerator(const QJsonObject &p) {
    for(const auto &r:SimulationV2::weatherGeneratorRanges()){const auto key=QString::fromStdString(r.key);if(p.value(key+".min").toDouble(r.low)>p.value(key+".max").toDouble(r.high))throw std::invalid_argument(("Reversed generator interval: "+key).toStdString());}
    QHash<QString,double> defaults;for(const auto &r:SimulationV2::weatherGeneratorScalars())defaults[QString::fromStdString(r.key)]=r.value;
    for(auto prefix:{"characterNeutral","characterStable","characterLottery","precipitationPattern"}){double total=0;for(int i=0;i<(QString(prefix)=="precipitationPattern"?4:3);++i){const auto k="generator.auto."+QString(prefix)+"."+QString::number(i);total+=p.value(k).toDouble(defaults.value(k));}if(total<=0)throw std::invalid_argument("Weather character weights cannot all be zero");}
    for(auto prefix:{"strength","heavyPrecipitation","precipitation","precipitationVariation"}){const auto k="generator.auto."+QString(prefix);if(p.value(k+".min").toDouble(defaults.value(k+".min"))>p.value(k+".max").toDouble(defaults.value(k+".max")))throw std::invalid_argument("Reversed automatic weather generator interval");}
}
SimulationV2::WeatherProposal weatherInput(const QJsonObject &config,const QJsonObject &p,SimulationV2::Seed seed) {
    if(config["weatherSource"].toInt()==0)return {windParameters(p),precipitationParameters(p)};
    SimulationV2::WeatherGeneratorParameters tuning;for(auto i=p.begin();i!=p.end();++i)if(i.key().startsWith("generator."))tuning.values[i.key().toStdString()]=i.value().toDouble();
    SimulationV2::SeededRandom rng(SimulationV2::deriveSeed(seed,0x505245534554ULL));
    SimulationV2::WeatherProposalContext context;context.k=p["hill.k"].toDouble();context.country=config["hill"].toObject()["source"].toObject()["country-code"].toString().toStdString();context.variabilityPerk=p["hill.perk.2"].toInt();context.directionPerk=p["hill.perk.3"].toInt();context.month=p["weather.month"].toInt();context.surface=p["weather.surface"].toInt()==1?SimulationV2::Surface::Mat:SimulationV2::Surface::Snow;
    if(config["weatherSource"].toInt()==1)return SimulationV2::proposeWeather(context,rng,tuning);
    return SimulationV2::sampleWeatherPreset(SimulationV2::WeatherPreset(config["weatherPreset"].toInt()),context.k,rng,tuning);
}
namespace {
template<class F> void visitEngine(SimulationV2::EngineParameters &m,F f) {
    f("quality.overallWeight",QObject::tr("Udział OVR"),"quality",m.quality.overallWeight,0,1,0.01,"1");
    f("quality.formReferenceK",QObject::tr("K odniesienia udziału formy"),"quality",m.quality.formReferenceK,1,400,1,"m");
    f("quality.formKScale",QObject::tr("Skala zmiany udziału formy względem K"),"quality",m.quality.formKScale,1,1000,1,"m");
    f("quality.formHillAmplitude",QObject::tr("Maksymalna zmiana udziału formy"),"quality",m.quality.formHillAmplitude,0,1,.005,"1");
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
    f("flight.windVariabilityScale",QObject::tr("Siła wiatru aktywująca rozrzut"),"flight",m.flight.windVariabilityScale,0.1,100,0.1,"m/s");
    f("flight.windHeadSpread",QObject::tr("Rozrzut wykorzystania wiatru pod narty"),"flight",m.flight.windHeadSpread,0,1,0.01,"1");
    f("flight.windTailSpread",QObject::tr("Rozrzut strat w wietrze w plecy"),"flight",m.flight.windTailSpread,0,1,0.01,"1");
    f("flight.windCrossSpread",QObject::tr("Rozrzut strat w wietrze bocznym"),"flight",m.flight.windCrossSpread,0,1,0.01,"1");
    f("flight.windHeadSaturation",QObject::tr("Skala nasycenia pomocy wiatru pod narty"),"flight",m.flight.windHeadSaturation,0,100,0.1,"m/s");
    f("flight.windAdverseOnset",QObject::tr("Siła wiatru aktywująca złe zdarzenia"),"flight",m.flight.windAdverseOnset,0.1,100,0.1,"m/s");
    f("flight.windAdverseExponent",QObject::tr("Stromość narastania szansy złego zdarzenia"),"flight",m.flight.windAdverseExponent,1,10,0.1,"1");
    f("flight.windHeadAdverseChance",QObject::tr("Maksymalna szansa złego zdarzenia: pod narty"),"flight",m.flight.windHeadAdverseChance,0,1,0.01,"1");
    f("flight.windHeadAdverseLoss",QObject::tr("Siła złego zdarzenia: pod narty"),"flight",m.flight.windHeadAdverseLoss,0,10,0.01,"1");
    f("flight.windTailAdverseChance",QObject::tr("Maksymalna szansa złego zdarzenia: w plecy"),"flight",m.flight.windTailAdverseChance,0,1,0.01,"1");
    f("flight.windTailAdverseLoss",QObject::tr("Siła złego zdarzenia: w plecy"),"flight",m.flight.windTailAdverseLoss,0,10,0.01,"1");
    f("flight.windCrossAdverseChance",QObject::tr("Maksymalna szansa złego zdarzenia: boczny"),"flight",m.flight.windCrossAdverseChance,0,1,0.01,"1");
    f("flight.windCrossAdverseLoss",QObject::tr("Siła złego zdarzenia: boczny"),"flight",m.flight.windCrossAdverseLoss,0,10,0.01,"1");
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
    f("wind.gustTravelMetresPerSecond",QObject::tr("Prędkość przemieszczania podmuchu"),"wind",m.gustTravelMetresPerSecond,0,100,0.1,"m/s");
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
    if(key=="quality.overallWeight")return QObject::tr("Udział OVR w bazowej jakości skoku na K odniesienia; resztę stanowi forma. Np. 0,70 przy OVR 80 i formie 60 daje jakość 74 przed perkami i losowością.");
    if(key=="quality.formReferenceK")return QObject::tr("Rozmiar skoczni, na którym obowiązuje ustawiony udział OVR i formy. Np. przy K odniesienia 110 forma waży więcej na K140, a mniej na K80.");
    if(key=="quality.formKScale")return QObject::tr("Określa, jak szybko udział formy zmienia się wraz z rozmiarem skoczni. Np. skala 100 daje łagodniejszą różnicę między K90 i K120 niż skala 20.");
    if(key=="quality.formHillAmplitude")return QObject::tr("Maksymalna zmiana udziału formy między skocznią odniesienia a bardzo małą lub dużą. Np. 0,05 pozwala zmienić udział formy o najwyżej 5 punktów procentowych.");
    if(key=="quality.negativeInfluence")return QObject::tr("Rozdziela siłę losowych strat i bonusów, nie szansę ich wystąpienia. Np. 0,65 zmienia wylosowane −10 na −13, a +10 na +7 przed wpływem perków.");
    if(key=="quality.hillPreferenceStrength")return QObject::tr("Wzmacnia wpływ charakteru skoczni na Lotnika i Specjalistę małych skoczni. Np. 0 wyłącza dodatkową preferencję skoczni; specjalizacja zależna od K pozostaje.");
    if(key=="quality.noise")return QObject::tr("Wybiera rozkład losowej jakości skoków. Np. Laplace’a daje więcej skoków blisko średniej i więcej skrajnych wpadek lub wyskoków niż rozkład normalny.");
    if(key=="flight.referenceQuality")return QObject::tr("Jakość skoku odpowiadająca odległości odniesienia przy bazowej belce i bez opadów. Np. zwiększenie z 70 do 80 sprawia, że jakość 70 zaczyna poniżej tej odległości.");
    if(key=="flight.referenceDistanceAtK")return QObject::tr("Początkowy zapas lotu jako część K, zanim zadziałają wiatr i geometria zeskoku. Np. 1,10 na K120 daje zapas 132 m; końcowa odległość może być inna.");
    if(key=="flight.qualityMetresAtK120")return QObject::tr("Metry początkowego zapasu za punkt jakości powyżej odniesienia na K120. Np. 0,8 i jakość wyższa o 10 dają dodatkowe 8 m zapasu przed wpływem wiatru.");
    if(key=="flight.referenceGate")return QObject::tr("Belka, przy której silnik wyznacza bazowy zapas lotu. Np. odniesienie 10 i start z 12 oznaczają doliczenie wpływu dwóch belek.");
    if(key=="flight.gateMetresAtK120")return QObject::tr("Zmiana początkowego zapasu za jedną belkę na K120, bez wiatru w plecy. Np. 2,0 i podniesienie o 3 belki dają dodatkowe 6 m zapasu.");
    if(key=="flight.gateTailwindInteraction")return QObject::tr("Wzmacnia wpływ różnicy belek przy wietrze w plecy. Np. 0,1 przy 2 m/s w plecy zwiększa ten wpływ o 20%.");
    if(key=="flight.stepMetres")return QObject::tr("Odległość między kolejnymi obliczeniami lotu; mniejsza daje dokładniejszą, ale wolniejszą symulację. Np. krok 0,5 m wymaga około dwa razy więcej obliczeń niż 1 m.");
    if(key=="flight.secondsPerK")return QObject::tr("Czas, po którym skoczek dociera do K w modelu pogody. Np. 4 s oznacza napotkanie wiatru przy K cztery sekundy po starcie.");
    if(key=="flight.windReservePerMetre")return QObject::tr("Siła wpływu wiatru pod narty i w plecy na podtrzymywanie lotu. Np. zwiększenie wartości wzmacnia zarówno pomoc wiatru pod narty, jak straty w wietrze w plecy.");
    if(key=="flight.crosswindDrag")return QObject::tr("Siła strat wywołanych wiatrem bocznym. Np. zwiększenie wartości skróci skoki przy 2 m/s z boku, ale nie zmieni skoków przy samym wietrze pod narty.");
    if(key=="flight.windResponseMetres")return QObject::tr("Dystans, na którym skoczek stopniowo reaguje na zmianę wiatru. Np. 20 m łagodzi wpływ krótkiego podmuchu bardziej niż 2 m.");
    if(key=="flight.earlyWindSensitivity")return QObject::tr("Wpływ wiatru na początku lotu względem końca lotu. Np. zwiększenie tej wartości wzmacnia pomoc podmuchu tuż za progiem.");
    if(key=="flight.lateWindSensitivity")return QObject::tr("Wpływ wiatru pod koniec lotu względem początku lotu. Np. zwiększenie tej wartości pozwala późnemu podmuchowi mocniej podtrzymać skok.");
    if(key=="flight.maximumWindAssistance")return QObject::tr("Maksymalne ograniczenie utraty zapasu dzięki korzystnemu wiatrowi. Np. 0,8 pozwala ograniczyć zużycie zapasu do 20% wartości bez tej pomocy.");
    if(key=="flight.windVariabilityScale")return QObject::tr("Siła wiatru, przy której losowe różnice reakcji stają się wyraźne. Np. zwiększenie z 1 do 3 m/s ogranicza różnice reakcji na słabe podmuchy.");
    if(key=="flight.windHeadSpread")return QObject::tr("Rozrzut wykorzystania silnego wiatru pod narty między skokami. Np. większa wartość pozwala jednemu skoczkowi zyskać na tym samym podmuchu więcej niż drugiemu; 0 wyłącza ten rozrzut.");
    if(key=="flight.windTailSpread")return QObject::tr("Rozrzut strat w silnym wietrze w plecy. Np. większa wartość pozwala dwóm podobnym skokom stracić różnie mimo tych samych warunków; 0 wyłącza ten rozrzut.");
    if(key=="flight.windCrossSpread")return QObject::tr("Rozrzut strat w silnym wietrze bocznym. Np. większa wartość daje bardziej zróżnicowane reakcje na ten sam boczny podmuch; 0 wyłącza ten rozrzut.");
    if(key=="flight.windHeadSaturation")return QObject::tr("Siła wiatru pod narty, od której dalsze wzmacnianie wiatru daje coraz mniej korzyści. Np. niższa wartość sprawia, że wzrost z 3 do 4 m/s pomaga mniej; 0 wyłącza nasycenie.");
    if(key=="flight.windAdverseOnset")return QObject::tr("Siła wiatru, wokół której narasta ryzyko utraty podtrzymania lotu. Np. większa wartość przesuwa to ryzyko w stronę silniejszych podmuchów; nie tworzy ostrego progu.");
    if(key=="flight.windAdverseExponent")return QObject::tr("Ostrość narastania ryzyka przy sile wiatru ustawionej jako próg złych zdarzeń. Np. większa wartość mocniej rozdziela słabszy, bezpieczniejszy wiatr od silniejszego, bardziej ryzykownego.");
    if(key=="flight.geometryStartRealHs")return QObject::tr("Miejsce rozpoczęcia ograniczania lotu przez wypłaszczenie. Np. 0,9 przy początku wypłaszczenia 140 m uruchamia opór od 126 m.");
    if(key=="flight.usableLandingBeyondRealHsAtK")return QObject::tr("Maksymalny zasięg lotu za początkiem wypłaszczenia, jako część K. Np. 0,1 na K120 dodaje do tego miejsca 12 m użytecznego zeskoku.");
    if(key=="flight.geometryResistance")return QObject::tr("Siła ograniczania lotu przy wypłaszczeniu zeskoku. Np. większa wartość utrudnia dolecenie daleko za HS mimo dużego zapasu po wybiciu.");
    if(key=="flight.exposureSections")return QObject::tr("Liczba odcinków w zapisie wiatru napotkanego podczas skoku. Np. 8 daje bardziej szczegółowy podgląd niż 4; nie jest to krok obliczania lotu.");
    if(key=="landing.sharedStyleDeviation")return QObject::tr("Losowe przesunięcie wszystkich not w tym samym skoku. Np. dodatnie losowanie podniesie wspólnie oceny pięciu sędziów, zanim pojawią się różnice między nimi.");
    if(key=="landing.judgeDeviation")return QObject::tr("Rozrzut oceny pojedynczego sędziego wokół wspólnej oceny skoku. Np. większa wartość częściej daje zestawy takie jak 17,5 / 18,0 / 18,5 zamiast pięciu równych not.");
    if(key=="landing.difficultLandingStylePenalty")return QObject::tr("Maksymalna dodatkowa kara w nocie każdego sędziego za trudne, dalekie lądowanie. Np. zwiększenie z 1 do 2 może mocniej obniżyć noty za skoki przy wypłaszczeniu.");
    if(key=="wind.mean")return QObject::tr("Bazowy wiatr wzdłuż skoczni, do którego dodają się podmuchy i lokalne różnice. Np. +1 oznacza 1 m/s pod narty, a −1 oznacza 1 m/s w plecy.");
    if(key=="wind.cross")return QObject::tr("Bazowy wiatr boczny; znak wybiera stronę, a wielkość siłę. Np. −2 i +2 oznaczają 2 m/s z przeciwnych stron.");
    if(key=="wind.temporalAmplitude")return QObject::tr("Siła wahań wiatru podczas konkursu, bez zmiany jego bazowego kierunku i siły. Np. większa wartość daje większą różnicę między warunkami dwóch kolejnych skoczków.");
    if(key=="wind.spatialAmplitude")return QObject::tr("Siła różnic wiatru między miejscami nad zeskokiem. Np. większa wartość ułatwia uzyskanie dobrych warunków przy progu i słabych przy lądowaniu w tym samym momencie.");
    if(key=="wind.correlationSeconds")return QObject::tr("Czas utrzymywania się podobnych warunków. Np. 60 s daje wolniejsze zmiany niż 10 s, bez zwiększania samej siły wiatru.");
    if(key=="wind.correlationMetres")return QObject::tr("Długość obszaru o podobnym wietrze. Np. 80 m daje rozleglejsze obszary podobnych warunków niż 20 m.");
    if(key=="wind.gustAmplitude")return QObject::tr("Siła dodatkowych, krótkich podmuchów. Np. zwiększenie z 0,5 do 1 m/s wzmacnia podmuchy, bez zwiększania ich częstości.");
    if(key=="wind.gustProbability")return QObject::tr("Szansa aktywnego podmuchu przy losowaniu w modelu wiatru. Np. 0,20 oznacza 20%, ale nie oznacza, że dokładnie 20% skoczków trafi na podmuch.");
    if(key=="wind.gustPeriodSeconds")return QObject::tr("Skala czasu podmuchów. Np. większa wartość rozciąga podmuchy w czasie, zamiast zwiększać ich siłę.");
    if(key=="wind.gustWidthMetres")return QObject::tr("Długość obszaru objętego jednym podmuchem. Np. 40 m pozwala temu samemu podmuchowi wpływać na dłuższy fragment lotu niż 10 m.");
    if(key=="wind.maximumStrength")return QObject::tr("Górny limit łącznej siły wiatru po zsumowaniu jego składników. Np. limit 3 m/s nie pozwala modelowi wygenerować silniejszego wiatru.");
    if(key=="wind.trendAmplitude")return QObject::tr("Siła zmian głównej tendencji wiatru podczas konkursu. Np. większa wartość ułatwia przejście od okresu wiatru pod narty do okresu wiatru w plecy.");
    if(key=="wind.trendPeriodSeconds")return QObject::tr("Długość okresu podobnej tendencji wiatru. Np. 180 s utrzymuje podobne tło dłużej niż 30 s.");
    if(key=="wind.trendTransitionSeconds")return QObject::tr("Czas płynnej zmiany jednej tendencji w drugą. Np. 20 s daje łagodniejsze przejście od wiatru w plecy do wiatru pod narty niż 2 s.");
    if(key=="wind.activityVariation")return QObject::tr("Różnica między spokojnymi i burzliwymi okresami w tym samym konkursie. Np. 0 wyłącza ten podział, a większa wartość mocniej oddziela oba rodzaje okresów.");
    if(key=="wind.activityCorrelationSeconds")return QObject::tr("Czas utrzymywania się spokojniejszego lub bardziej burzliwego okresu. Np. 120 s daje dłuższe epizody niż 20 s.");
    if(key=="wind.fastCorrelationSeconds")return QObject::tr("Czas drobnych, szybkich wahań wiatru. Np. większa wartość spowalnia te wahania bez zwiększania ich siły.");
    if(key=="athlete.overall")return QObject::tr("Umiejętności analizowanego skoczka w skali 1–100. Np. podniesienie OVR z 70 do 80 zwykle wydłuży jego skoki przy tej samej formie.");
    if(key=="athlete.form")return QObject::tr("Bieżąca dyspozycja analizowanego skoczka w skali 1–100. Np. forma 90 zamiast 50 poprawia bazową jakość skoku; siłę wpływu określa udział formy w silniku.");
    if(key=="athlete.landing")return QObject::tr("Styl lądowania analizowanego skoczka: 0 Niechlujne, 4 Stylista. Np. zmiana z 1 na 3 poprawia noty, ale nie zmniejsza ryzyka upadku.");
    if(key=="hill.realHs")return QObject::tr("Odległość początku wypłaszczenia, od którego trudniej kontynuować lot i lądować. Np. przesunięcie ze 140 na 145 m pozwala bezpieczniej osiągać dalsze odległości.");
    if(key=="hill.k")return QObject::tr("Rozmiar skoczni według punktu K; w labie zmienia proporcjonalnie także HS i początek wypłaszczenia. Np. K120→K60 zmniejsza te odległości o połowę; współczynniki punktacji ustawiasz osobno.");
    if(key=="gate")return QObject::tr("Belka startowa eksperymentu; wyższa zwykle wydłuża skoki. Np. 12 ustawia konkretną belkę, a 0 dobiera wspólną belkę automatycznie według wariantu Balans z gry.");
    if(key=="gate.reference")return QObject::tr("Belka odniesienia do rekompensaty, bez zmiany belki startowej. Np. przy odniesieniu 12 i starcie z 10 nalicza się rekompensatę za dwie belki; 0 używa wspólnej bazy.");
    if(key=="gate.coachReduction")return QObject::tr("Liczba stopni obniżenia belki przez trenera względem belki jury. Np. 2 przy belce jury 12 daje start z 10; rekompensata zależy m.in. od reguły 95% HS.");
    if(key=="clock.mode")return QObject::tr("Określa, czy między skokami upływa czas pogody. Np. Kontrolowane warunki daje wszystkim tę samą chwilę, a Przebieg konkursu pozwala trafić na różne podmuchy.");
    if(key=="clock.interval")return QObject::tr("Czas pogody między startami kolejnych skoczków. Np. 30 s daje starty w chwilach 0, 30, 60 s; nie oznacza rzeczywistego oczekiwania przed ekranem.");
    if(key=="dsq")return QObject::tr("Ryzyko DSQ pojedynczego skoku przy włączonych dyskwalifikacjach. Np. 0,01 oznacza średnio około jednej dyskwalifikacji na 100 prób, nie gwarantowaną liczbę.");
    if(group=="landing" && key.endsWith("Centre"))return QObject::tr("Miejsce połowy wzrostu ryzyka danego błędu, jako część odległości początku wypłaszczenia. Np. 1,05 przy wypłaszczeniu od 140 m oznacza środek wzrostu ryzyka przy 147 m.");
    if(group=="landing" && key.endsWith("Width"))return QObject::tr("Odległość, na której ryzyko błędu przechodzi od niskiego do wysokiego. Np. większa wartość rozciąga ten wzrost na więcej metrów, zamiast po prostu wszędzie obniżać ryzyko.");
    if(group=="landing" && key.endsWith("Base"))return QObject::tr("Ryzyko danego błędu przed doliczeniem wpływu odległości i bocznego wiatru. Np. 0,01 oznacza bazowe 1%; podpórka i dwie nogi są oceniane dopiero po wykluczeniu poważniejszych błędów.");
    if(group=="landing" && key.endsWith("Increase"))return QObject::tr("Maksymalny dodatkowy wzrost ryzyka błędu wraz z odległością. Np. 0,20 może dodać do 20 punktów procentowych przed uwzględnieniem kolejności błędów.");
    if(group=="landing" && key.startsWith("landing.crosswind"))return QObject::tr("Dodatkowe ryzyko tego błędu za 1 m/s wiatru bocznego. Np. 0,01 przy 2 m/s dodaje 2 punkty procentowe przed uwzględnieniem kolejności błędów.");
    if(key.startsWith("landing.styleMean."))return QObject::tr("Początkowa nota jednego sędziego dla tej klasy stylu. Np. 18,5 ustawia bazę 18,5 pkt, od której odejmuje się kary i do której dodaje się losowe wahania.");
    if(key.startsWith("landing.landingPenalty."))return QObject::tr("Kara w nocie każdego sędziego za wskazany rodzaj lądowania. Np. 3 odejmuje każdemu sędziemu 3 pkt; nie zwiększa szansy tego błędu.");
    if(key.startsWith("flight.wind") && key.endsWith("AdverseChance"))return QObject::tr("Maksymalne ryzyko utraty podtrzymania lotu przy bardzo silnym wietrze. Np. 0,20 oznacza limit 20%; przy słabszym wietrze ryzyko jest mniejsze.");
    if(key.startsWith("flight.wind") && key.endsWith("AdverseLoss"))return QObject::tr("Siła straty po wylosowaniu utraty podtrzymania lotu. Np. większa wartość mocniej skróci pechowy skok, ale nie zwiększy częstości takich zdarzeń.");
    if(key=="flight.precipitationLossPerLevel")return QObject::tr("Część początkowego zapasu lotu tracona za poziom śniegu. Np. 0,008 przy natężeniu 10 odbiera 8% zapasu, niekoniecznie 8% końcowej odległości.");
    if(key=="flight.rainLossPerLevel")return QObject::tr("Część zapasu lotu tracona za poziom deszczu ponad próg wpływu. Np. 0,003 i natężenie o 4 wyższe od progu dają stratę 1,2%, przed doliczeniem ulewy.");
    if(key=="flight.rainLossThreshold")return QObject::tr("Natężenie deszczu, do którego nie działa zwykła strata zapasu. Np. próg 3 oznacza, że przy natężeniu 5 nalicza się stratę za 2 poziomy; ulewa ma osobny próg.");
    if(key=="flight.rainHeavyThreshold")return QObject::tr("Natężenie deszczu, powyżej którego narasta dodatkowa strata od ulewy. Np. przy progu 7 ulewa nie dodaje straty przy natężeniu 6, ale dodaje ją przy 9.");
    if(key=="flight.rainHeavyLossAtMaximum")return QObject::tr("Dodatkowa strata zapasu lotu przy deszczu o natężeniu 10. Np. 0,15 odbiera 15% zapasu, oprócz zwykłej straty od deszczu.");
    if(key=="flight.rainHeavyExponent")return QObject::tr("Określa, jak szybko narasta strata między progiem ulewy a natężeniem 10. Np. wykładnik 2 daje w połowie tego zakresu 25% maksymalnej straty, a wykładnik 1 daje 50%.");
    if(key=="wind.gustTravelMetresPerSecond")return QObject::tr("Prędkość przesuwania się podmuchu wzdłuż zeskoku. Np. 5 m/s przesuwa jego środek o 10 m w dwie sekundy; 0 zatrzymuje go w miejscu.");
    if(key=="precipitation.scenario")return QObject::tr("Wybiera zachowanie opadów w czasie. Np. Narastające stopniowo zwiększają natężenie, a Zmienne wahają się wokół ustawionej bazy.");
    if(key=="precipitation.kind")return QObject::tr("Wybiera wzór wpływu opadów na skok. Np. śnieg odbiera zapas od pierwszego poziomu, a deszcz może nie powodować straty aż do ustawionego progu.");
    if(key=="precipitation.intensity")return QObject::tr("Początkowe natężenie opadów w skali 0–10; większe mocniej skraca skoki. Np. w trybie Stałe wartość 4 utrzymuje natężenie 4, a w trybie Silne baza wynosi co najmniej 8.");
    if(key=="precipitation.variation")return QObject::tr("Siła losowych zmian natężenia opadów. Np. zwiększenie wartości daje większe różnice między spokojną chwilą a silnym opadem; w trybie Stałe parametr nie działa.");
    if(key=="precipitation.correlationSeconds")return QObject::tr("Czas utrzymywania się podobnego natężenia opadów. Np. 60 s daje wolniejsze zmiany niż 10 s, bez zmiany bazowego natężenia.");
    if(key=="precipitation.increasePerMinute")return QObject::tr("Wzrost natężenia na minutę w trybie Narastające, do limitu 10. Np. 0,5 zwiększa natężenie o 1 w dwie minuty.");
    if(key=="measurement.temporalSamples")return QObject::tr("Liczba odczytów z każdego czujnika podczas lotu. Np. 5 odczytów lepiej wychwytuje zmianę podmuchu niż 1, ale liczy się dłużej; odczyt przed startem i metoda dotychczasowa używają własnych zasad.");
    if(key=="measurement.extentK")return QObject::tr("Długość obszaru z czujnikami, jako część K. Np. 1,2 na K120 ustawia zasięg 144 m; nie zmienia lotu, metody dotychczasowej ani pomiaru na trasie skoczka.");
    if(key=="measurement.denseSpacingMetres")return QObject::tr("Odstęp czujników wyłącznie w metodzie Gęsta sieć czujników. Np. 5 m daje gęstszą sieć niż 20 m, do limitu 128 czujników.");
    if(key=="measurement.weightPeak")return QObject::tr("Miejsce najmocniej uwzględnianych czujników w metodzie FIS — przybliżenie. Np. 0,5 wyróżnia środek sieci, a 0,75 jej dalszą część.");
    if(key=="measurement.weightStrength")return QObject::tr("Przewaga ważniejszych czujników nad pozostałymi w metodzie FIS — przybliżenie. Np. 0 daje wszystkim równe wagi; większa wartość wzmacnia wskazany szczyt wag.");
    if(key=="athlete.perk.0")return QObject::tr("Wybiera perk podejścia i jego poziom; 0 usuwa perk. Np. +2 daje Błyszczy na treningach 2, a −2 daje Oszczędza siły 2; dostępne są poziomy 1–4.");
    if(key=="athlete.perk.1")return QObject::tr("Wybiera specjalizację i jej poziom; 0 usuwa perk. Np. +3 daje Lotnika 3, a −3 daje Specjalistę małych skoczni 3; dostępne są poziomy 1–4.");
    if(key=="athlete.perk.2")return QObject::tr("Wybiera regularność i poziom perka; 0 usuwa perk. Np. +2 daje Regularnego 2, a −2 Nieprzewidywalnego 2 z większym rozrzutem, zwłaszcza strat; dostępne są poziomy 1–4.");
    if(key=="hill.perk.0")return QObject::tr("Charakter skoczni: dodatnie poziomy premiują wybicie, ujemne lot. Np. +2 wzmacnia specjalistów małych skoczni względem lotników; ręczna preferencja skoczni ma pierwszeństwo.");
    if(key=="hill.perk.1")return QObject::tr("Łatwość dalekiego lądowania: dodatnie poziomy odsuwają wypłaszczenie, ujemne je przybliżają. Np. −3 wcześniej utrudnia dalekie skoki; ręczny początek wypłaszczenia ma pierwszeństwo.");
    if(key=="hill.perk.2")return QObject::tr("Wpływ skoczni na automatycznie losowaną zmienność pogody. Np. +4 częściej daje loterię, a −4 spokojne warunki; ręczne warunki i wybrany preset pozostają bez zmian.");
    if(key=="hill.perk.3")return QObject::tr("Wpływ skoczni na automatycznie losowany kierunek wiatru. Np. +3 sprzyja wiatrowi pod narty, a −3 w plecy; ręczne warunki i wybrany preset pozostają bez zmian.");
    if(key=="hill.hs")return QObject::tr("Oficjalny rozmiar skoczni, używany m.in. przy rekompensacie za belkę trenera. Np. HS140 oznacza wymagane 133 m dla reguły 95% HS; wypłaszczenie ustawia się osobno.");
    if(key=="hill.takeoffBias")return QObject::tr("Dodatkowa preferencja wybicia lub lotu przy działaniu specjalizacji. Np. +0,5 przesuwa charakter w stronę wybicia, a −0,5 w stronę lotu; skoczek bez specjalizacji nie zyskuje bonusu.");
    if(key=="hill.pointsAtK")return QObject::tr("Punkty za odległość równą K, przed notami i rekompensatami. Np. wartość 60 daje za skok dokładnie na K bazowe 60 pkt.");
    if(key=="hill.pointsPerMetre")return QObject::tr("Punkty dodawane za metr ponad K i odejmowane za metr przed K. Np. 1,8 daje za 5 m ponad K dodatkowe 9 pkt.");
    if(key=="hill.gatePoints")return QObject::tr("Rekompensata za jedną belkę obniżenia względem odniesienia. Np. 5 pkt za belkę i obniżenie o 2 dają 10 pkt, jeśli zasady pozwalają je przyznać.");
    if(key=="hill.headwindPoints")return QObject::tr("Punkty odejmowane za 1 m/s zmierzonego wiatru pod narty. Np. współczynnik 8 i wiatr 1,5 m/s oznaczają odjęcie 12 pkt przy włączonej rekompensacie.");
    if(key=="hill.tailwindPoints")return QObject::tr("Punkty dodawane za 1 m/s zmierzonego wiatru w plecy. Np. współczynnik 12 i wiatr 1,5 m/s oznaczają dodanie 18 pkt przy włączonej rekompensacie.");
    if(key=="event")return QObject::tr("Rodzaj wydarzenia, który określa działanie perków podejścia i rozrzut skoków. Np. Błyszczy na treningach daje w kwalifikacjach połowę bonusu treningowego, a w konkursie karę.");
    if(key=="weather.month")return QObject::tr("Miesiąc używany tylko do automatycznego losowania opadów. Np. 1 oznacza styczeń, a 0 pomija porę roku; na półkuli południowej sezony są odwrócone.");
    if(key=="weather.surface")return QObject::tr("Nawierzchnia używana przy automatycznym losowaniu opadów. Np. na igelicie wylosowany opad będzie deszczem, a na śniegu możliwy jest także śnieg zależnie od miesiąca.");
    if(key=="field.skill")return QObject::tr("Środkowy OVR generowanej stawki, bez zmiany zawodników z paczki. Np. OVR 70 i rozpiętość 20 dają stawkę od 60 do 80.");
    if(key=="field.spread")return QObject::tr("Różnica OVR między najlepszym i najsłabszym w generowanej stawce. Np. rozpiętość 30 i środek 70 dają zakres 55–85; wczytana paczka pozostaje bez zmian.");
    if(key=="field.form")return QObject::tr("Wspólna forma pozostałych zawodników po ręcznej zmianie tego pola. Np. 60 ustawia im formę 60, także w paczce; analizowany skoczek i indywidualne poprawki mają osobne wartości.");
    if(key=="field.landing")return QObject::tr("Wspólny styl lądowania pozostałych zawodników po ręcznej zmianie pola. Np. 4 ustawia Stylistę, także w paczce; indywidualne poprawki mają pierwszeństwo.");
    if(key=="rules.dsq")return QObject::tr("Włącza losowanie DSQ według ustawionego ryzyka. Np. po wyłączeniu nie będzie dyskwalifikacji nawet przy ryzyku 0,1.");
    if(key=="rules.windCompensation")return QObject::tr("Włącza punkty rekompensaty za zmierzony wiatr. Np. po wyłączeniu wiatr nadal wydłuża lub skraca lot, ale nie dodaje ani nie odejmuje punktów.");
    if(key=="rules.gateCompensation")return QObject::tr("Włącza punkty rekompensaty za różnicę belek. Np. po wyłączeniu obniżona belka nadal skraca skok, ale nie daje za to punktów.");
    if(key=="rules.judges")return QObject::tr("Włącza doliczanie trzech środkowych not sędziowskich. Np. noty 17 / 18 / 18 / 19 / 20 dają 55 pkt; po wyłączeniu dają 0 pkt.");
    if(key=="rules.95hs")return QObject::tr("Uzależnia rekompensatę za obniżenie belki przez trenera od 95% HS. Np. na HS140 trzeba skoczyć co najmniej 133 m; zmiana belki przez jury nie podlega tej regule.");
    if(key=="rules.measurement")return QObject::tr("Wybiera sposób pomiaru średniego wiatru do rekompensaty. Np. odczyt przed startem może dać inną rekompensatę niż pomiar podczas lotu, choć skoczek trafia na ten sam wiatr.");
    if(key=="analysis.duration")return QObject::tr("Długość analizowanego przebiegu wiatru w sekundach. Np. 300 pokazuje pięć minut warunków, bez wydłużania samego konkursu.");
    if(key=="analysis.time")return QObject::tr("Chwila pogody, dla której pokazuje się wiatr na całym zeskoku. Np. 60 daje przekrój minutę po początku modelu, bez przesuwania startu zawodników.");
    if(key=="analysis.distance")return QObject::tr("Miejsce nad zeskokiem, dla którego pokazuje się zmiany wiatru w czasie. Np. 100 oznacza odczyt 100 m od progu.");
    if(key=="analysis.scale")return QObject::tr("Zakres siły wiatru odpowiadający pełnemu nasyceniu kolorów mapy. Np. 2 daje skalę od −2 do +2 m/s; silniejszy wiatr zachowuje kolor skrajny.");
    if(key=="analysis.timeSamples")return QObject::tr("Liczba chwil odczytu na wykresie i mapie wiatru. Np. 101 próbek w ciągu 100 s daje odczyt co sekundę; większa liczba liczy się dłużej.");
    if(key=="analysis.spaceSamples")return QObject::tr("Liczba miejsc odczytu wzdłuż zeskoku. Np. większa liczba pokazuje węższe obszary różnego wiatru, ale zwiększa czas obliczeń.");
    if(key=="analysis.jumpSamples")return QObject::tr("Liczba skoków używanych do analizy zależności od wiatru. Np. 1000 daje stabilniejsze rozkłady niż 100, ale wymaga więcej obliczeń.");
    if(key=="curve.extension")return QObject::tr("Zachowanie udziału wybicia poza najmniejszym i największym ustawionym K. Np. Stały udział zachowuje proporcję ze skrajnej kotwicy, a Liniowe przedłużenie kontynuuje zmianę, do granic 0–1.");
    if(key.startsWith("quality.eventDeviation."))return QObject::tr("Rozrzut losowej jakości dla wskazanego rodzaju wydarzenia, w punktach jakości. Np. 0 usuwa bazowe wahania jakości, a większa wartość daje większe bonusy i straty.");
    if(key.startsWith("quality.specialistBias."))return QObject::tr("Siła specjalizacji na wskazanym poziomie perka. Np. przy 70% lotu współczynnik 10 daje Lotnikowi +4 punkty jakości, a specjaliście małych skoczni −4.");
    if(key.startsWith("quality.trainingBonus."))return QObject::tr("Bonus jakości za Błyszczy na treningach na wskazanym poziomie. Np. 4 daje +4 w treningu i próbie oraz +2 w kwalifikacjach; kara konkursowa jest osobna.");
    if(key.startsWith("quality.competitionPenalty."))return QObject::tr("Kara jakości w konkursie za Błyszczy na treningach na wskazanym poziomie. Np. 2 odejmuje 2 punkty jakości, bez zmiany bonusu treningowego.");
    if(key.startsWith("quality.conservedEffortPenalty."))return QObject::tr("Kara jakości w treningu i próbie za Oszczędza siły na wskazanym poziomie. Np. 3 odejmuje 3 punkty jakości; w kwalifikacjach i konkursie ta kara nie działa.");
    if(key.startsWith("quality.conservedEffortDeviation."))return QObject::tr("Mnożnik rozrzutu jakości w treningu i próbie za Oszczędza siły. Np. 1,5 zwiększa rozrzut o połowę, a 1 pozostawia go bez zmian.");
    if(key.startsWith("quality.regularNegative."))return QObject::tr("Mnożnik wielkości losowych strat za Regularny na wskazanym poziomie. Np. 0,5 zmienia stratę −10 w −5, bez zmiany dodatnich losowań.");
    if(key.startsWith("quality.regularPositive."))return QObject::tr("Mnożnik wielkości losowych bonusów za Regularny na wskazanym poziomie. Np. 0,5 zmienia bonus +10 w +5, bez zmiany ujemnych losowań.");
    if(key.startsWith("quality.unpredictableNegative."))return QObject::tr("Mnożnik wielkości losowych strat za Nieprzewidywalny na wskazanym poziomie. Np. 1,5 zmienia stratę −10 w −15, bez zwiększania szansy ujemnego losowania.");
    if(key.startsWith("quality.unpredictablePositive."))return QObject::tr("Mnożnik wielkości losowych bonusów za Nieprzewidywalny na wskazanym poziomie. Np. 1,2 zmienia bonus +10 w +12, bez zwiększania szansy dodatniego losowania.");
    if(key.startsWith("field.perk."))return effectDescription("athlete.perk."+key.section('.',-1),"athlete")+" "+QObject::tr("Dotyczy pozostałych zawodników, także z paczki; indywidualne poprawki mają pierwszeństwo.");
    if(key.startsWith("wind.reference.")) {
        if(key.endsWith("positionK"))return QObject::tr("Miejsce lokalnej korekty wiatru jako część K; wiatr między punktami zmienia się płynnie. Np. 0,5 na K120 umieszcza korektę 60 m od progu.");
        if(key.endsWith("head"))return QObject::tr("Lokalna zmiana bazowego wiatru wzdłuż skoczni, płynnie łączona z sąsiednimi punktami. Np. baza −1 m/s i korekta +2 dają w tym miejscu +1 m/s przed podmuchami.");
        return QObject::tr("Lokalna zmiana bocznego wiatru, płynnie łączona z sąsiednimi punktami. Np. baza +1 m/s i korekta −2 dają −1 m/s z przeciwnej strony przed podmuchami.");
    }
    if(key.startsWith("generator.preset.")) {
        const auto field=key.section('.',3,3);
        QString help;
        if(field=="strength")help=QObject::tr("Zakres bazowej siły wiatru przed podmuchami i lokalnymi różnicami.");
        else if(field=="strength2")help=QObject::tr("Zakres siły rzadszego wariantu presetu Cisza, wybieranego w 15% losowań.");
        else if(field=="angle")help=QObject::tr("Zakres kierunku: w presetach kierunkowych jest odchyleniem, w Ciszy i Loterii pełnym kątem.");
        else if(field=="difference")help=QObject::tr("Zakres losowych lokalnych korekt wiatru; większe korekty bardziej różnicują miejsca nad zeskokiem.");
        else if(field=="mixedAmplitude")help=QObject::tr("Zakres siły przeciwnego wiatru między częściami zeskoku w presecie Różne kierunki na zeskoku.");
        else if(field=="temporalAmplitude")help=QObject::tr("Zakres siły wahań wiatru w czasie; większe wartości mocniej różnicują warunki kolejnych skoczków.");
        else if(field=="spatialAmplitude")help=QObject::tr("Zakres siły różnic wiatru wzdłuż zeskoku; większe wartości dają mniej jednolite warunki lotu.");
        else if(field=="trendAmplitude")help=QObject::tr("Zakres siły zmian głównej tendencji wiatru; większe wartości pozwalają na silniejsze zmiany tła.");
        else if(field=="trendPeriodSeconds")help=QObject::tr("Zakres czasu utrzymywania się podobnej tendencji wiatru; większe wartości wydłużają takie okresy.");
        else if(field=="gustAmplitude")help=QObject::tr("Zakres siły dodatkowych podmuchów, bez zmiany ich częstości.");
        else if(field=="gustProbability")help=QObject::tr("Zakres szansy aktywnego podmuchu; wartość 0,2 oznacza 20% przy losowaniu w modelu.");
        else if(field=="correlationSeconds")help=QObject::tr("Zakres czasu podobnych warunków; większe wartości spowalniają wahania wiatru.");
        else if(field=="activityVariation")help=QObject::tr("Zakres różnicy między spokojnymi i burzliwymi okresami konkursu; większe wartości mocniej je rozdzielają.");
        else if(field=="correlationK")help=QObject::tr("Zakres długości obszaru o podobnym wietrze, jako część K; wartość 0,5 na K120 oznacza 60 m.");
        else if(field=="gustWidthK")help=QObject::tr("Zakres szerokości podmuchu, jako część K; wartość 0,3 na K120 oznacza 36 m.");
        else if(field=="transitionSeconds")help=QObject::tr("Zakres czasu płynnego przechodzenia między tendencjami; większe wartości łagodzą zmianę wiatru.");
        return help+" "+(key.endsWith(".min")?QObject::tr("Ustawiasz minimum losowania w tym presecie. Np. minimum 1 i maksimum 2 pozwalają wylosować wartość od 1 do 2 w jednostce pola."):QObject::tr("Ustawiasz maksimum losowania w tym presecie. Np. minimum 1 i maksimum 2 pozwalają wylosować wartość od 1 do 2 w jednostce pola."));
    }
    if(key.startsWith("generator.auto.")) {
        const auto field=key.mid(15);
        if(field=="strengthScale")return QObject::tr("Mnożnik bazowej siły automatycznie losowanego wiatru, bez podmuchów i zmienności. Np. 0,5 zmienia propozycję 2 m/s na 1 m/s.");
        if(field=="variationScale")return QObject::tr("Wspólny mnożnik wahań w czasie i różnic na zeskoku w losowanych warunkach. Np. 2 podwaja oba składniki, bez podwajania bazowej siły, podmuchów ani tendencji.");
        if(field=="gustScale")return QObject::tr("Mnożnik siły automatycznie losowanych podmuchów. Np. 2 zmienia podmuch 0,5 m/s na 1 m/s, bez zmiany jego szansy wystąpienia.");
        if(field=="trendScale")return QObject::tr("Mnożnik siły zmian głównej tendencji automatycznie losowanego wiatru. Np. 0 wyłącza ten składnik, a 2 podwaja jego siłę bez zmiany czasu zmian.");
        if(field=="timeScale")return QObject::tr("Mnożnik czasu wahań i zmian tendencji w automatycznie losowanych warunkach. Np. 2 zmienia okres tendencji 60 s na 120 s oraz dwukrotnie wydłuża pozostałe objęte nim czasy.");
        if(field=="crossProbability")return QObject::tr("Szansa, że w automatycznie losowanych warunkach będzie dominował wiatr boczny. Np. 0,15 oznacza 15% propozycji, a nie wiatr o sile 0,15 m/s.");
        if(field=="correlationKStable")return QObject::tr("Długość obszaru podobnego wiatru w spokojnych warunkach losowanych automatycznie. Np. 0,5 na K120 oznacza skalę 60 m; większa wartość daje rozleglejsze obszary.");
        if(field=="correlationKVariable")return QObject::tr("Długość obszaru podobnego wiatru w zmiennych i loteryjnych warunkach losowanych automatycznie. Np. 0,5 na K120 oznacza skalę 60 m; mniejsza wartość daje drobniejsze różnice.");
        if(field=="gustWidthK")return QObject::tr("Szerokość automatycznie losowanego podmuchu jako część K. Np. 0,35 na K120 daje 42 m.");
        if(field=="rainProbability")return QObject::tr("Szansa deszczu zamiast śniegu, jeśli na śnieżnej nawierzchni poza zimnymi miesiącami wylosowano opad. Np. 0,8 daje 80% deszczu; nie zmienia szansy wystąpienia opadów.");
        if(field=="wetSnow")return QObject::tr("Bazowa szansa opadów na śnieżnej nawierzchni, przed dodatkami za miesiąc i kraj. Np. 0,3 oznacza bazowe 30%; dodatki mogą podnieść wynik do 100%.");
        if(field=="wetColdBonus")return QObject::tr("Dodatek do szansy opadów w zimnym miesiącu na śnieżnej nawierzchni. Np. 0,1 podnosi bazowe 30% do 40% przed dodatkiem za kraj.");
        if(field=="wetNorthBonus")return QObject::tr("Dodatek do szansy opadów na śniegu w Norwegii, Szwecji, Finlandii i Kanadzie. Np. 0,1 zwiększa szansę z 30% do 40% przed dodatkiem za miesiąc.");
        if(field=="wetMat")return QObject::tr("Szansa opadów na igelicie; wylosowany opad będzie deszczem. Np. 0,3 daje średnio deszcz w 30% propozycji warunków.");
        if(field.startsWith("headProbability."))return QObject::tr("Szansa wiatru pod narty na wskazanym poziomie dodatniego perka kierunku; ujemny perk odwraca kierunek. Np. 0,8 daje 80% pod narty przy dodatnim perku i 80% w plecy przy ujemnym.");
        if(field.startsWith("character"))return QObject::tr("Waga losowania spokojnej, zmiennej lub loteryjnej pogody w tym zestawie. Np. wagi 2/1/1 dają szanse 50%/25%/25% przed mieszaniem zestawów według perka skoczni.");
        if(field.startsWith("stable.") || field.startsWith("unstable."))return QObject::tr("Zapasowa waga starszego zestawu, używana tylko bez odpowiedniej wagi characterStable lub characterLottery. Np. gdy characterStable jest ustawione, zmiana stable nie wpływa na losowanie.");
        if(field.startsWith("precipitationPattern."))return QObject::tr("Waga wyboru przebiegu opadów, jeśli opady już wylosowano. Np. cztery równe wagi dają każdemu przebiegowi 25%; nie zmienia to szansy samych opadów.");
        if(field.startsWith("strength."))return QObject::tr("Dolna lub górna granica bazowej siły automatycznie losowanego wiatru, bez podmuchów. Np. minimum 1 i maksimum 2 dają bazę między 1 a 2 m/s.");
        if(field.startsWith("heavyPrecipitation."))return QObject::tr("Granica początkowego natężenia silnych opadów. Np. zakres 8–10 losuje bazę od 8 do 10; tryb Silne i tak podnosi niższą bazę do 8.");
        if(field.startsWith("precipitationVariation."))return QObject::tr("Granica siły wahań zmiennych opadów. Np. większy zakres pozwala wylosować większe różnice natężenia między chwilami, bez podnoszenia samej bazy.");
        if(field.startsWith("precipitation."))return QObject::tr("Granica bazowego natężenia zwykłych opadów w skali 0–10. Np. minimum 2 i maksimum 4 losują bazę od 2 do 4; silne opady mają osobny zakres.");
        const auto quantity=field.section('.',0,0);
        if(quantity=="temporal")return QObject::tr("Siła wahań w czasie dla wskazanego rodzaju losowanej pogody. Np. większa wartość daje większe różnice między warunkami kolejnych skoczków.");
        if(quantity=="spatial")return QObject::tr("Siła różnic na zeskoku dla wskazanego rodzaju losowanej pogody. Np. większa wartość ułatwia uzyskanie dobrego wiatru przy progu i słabego przy lądowaniu.");
        if(quantity=="gusts")return QObject::tr("Siła podmuchów dla wskazanego rodzaju losowanej pogody. Np. 1 zamiast 0,5 m/s podwaja ich siłę, bez zwiększania szansy wystąpienia.");
        if(quantity=="chance")return QObject::tr("Szansa aktywnego podmuchu dla wskazanego rodzaju losowanej pogody. Np. 0,25 daje 25% przy losowaniu w modelu; nie gwarantuje podmuchu u co czwartego skoczka.");
        if(quantity=="times")return QObject::tr("Czas utrzymywania się podobnego wiatru dla wskazanego rodzaju losowanej pogody. Np. 60 s spowalnia wahania w porównaniu z 10 s.");
        if(quantity=="trendAmplitude")return QObject::tr("Siła zmian głównej tendencji dla wskazanego rodzaju losowanej pogody. Np. większa wartość ułatwia wyraźne przejście z wiatru pod narty na wiatr w plecy.");
        if(quantity=="trendPeriod")return QObject::tr("Czas utrzymywania się podobnej tendencji dla wskazanego rodzaju losowanej pogody. Np. 180 s daje dłuższe okresy niż 30 s.");
        if(quantity=="transitions")return QObject::tr("Czas płynnej zmiany tendencji dla wskazanego rodzaju losowanej pogody. Np. 20 s daje łagodniejsze przejście między kierunkami niż 2 s.");
        if(quantity=="activity")return QObject::tr("Różnica między spokojnymi i burzliwymi okresami przy wskazanym rodzaju pogody. Np. 0 usuwa ten podział, a większa wartość mocniej rozdziela oba rodzaje okresów.");
    }

    if(key.startsWith("curve.k."))return QObject::tr("Rozmiar skoczni w wybranym punkcie krzywej specjalizacji. Np. przesunięcie kotwicy z K110 na K120 przenosi przypisany jej udział wybicia na K120; kolejne K muszą rosnąć.");
    if(key.startsWith("curve.takeoff."))return QObject::tr("Udział wybicia w wybranym punkcie krzywej specjalizacji; resztę stanowi lot. Np. 0,60 daje 60% wybicia i 40% lotu; wraz ze wzrostem K udział wybicia nie może rosnąć.");
    return {};

}
QJsonObject definition(const QString &key,const QString &label,const QString &group,double value,double lo,double hi,double step,const QString &unit,bool integer) {
    return {{"key",key},{"label",label},{"group",group},{"default",value},{"minimum",lo},{"maximum",hi},{"step",step},{"unit",unit},{"type",integer?"integer":"number"},{"help",effectDescription(key,group)+"\n"+QObject::tr("%1 · jednostka: %2 · domyślnie: %3").arg(key,unit).arg(value,0,'g',9)}};
}
}
QJsonArray parameters() {
    QJsonArray rows;
    const auto add=[&](const QString &key,const QString &label,const QString &group,auto &value,double lo,double hi,double step,const QString &unit){rows.append(definition(key,label,group,double(value),lo,hi,step,unit,!std::is_floating_point<typename std::decay<decltype(value)>::type>::value));};
    SimulationV2::EngineParameters engine;visitEngine(engine,add);
    const QHash<QString,QString> generatorLabels{
        {"strength",QObject::tr("Siła wiatru")},{"strength2",QObject::tr("Siła wiatru — rzadki wariant")},
        {"angle",QObject::tr("Odchylenie kierunku")},{"difference",QObject::tr("Różnice na zeskoku")},
        {"temporalAmplitude",QObject::tr("Amplituda w czasie")},{"spatialAmplitude",QObject::tr("Amplituda w przestrzeni")},
        {"trendAmplitude",QObject::tr("Amplituda tendencji")},{"trendPeriodSeconds",QObject::tr("Okres tendencji")},
        {"gustAmplitude",QObject::tr("Siła podmuchów")},{"gustProbability",QObject::tr("Szansa podmuchów")},
        {"correlationSeconds",QObject::tr("Czas podobnych warunków")},{"activityVariation",QObject::tr("Zmienność aktywności")},
        {"mixedAmplitude",QObject::tr("Odwrócenie wiatru na zeskoku")},{"transitionSeconds",QObject::tr("Czas przejścia tendencji")},
        {"correlationK",QObject::tr("Długość podobnych warunków / K")},{"gustWidthK",QObject::tr("Szerokość podmuchu / K")}
    };
    for(const auto &r:SimulationV2::weatherGeneratorRanges()) {
        if(r.field=="mixedAmplitude" && r.preset!=10)continue;
        const auto key=QString::fromStdString(r.key),field=QString::fromStdString(r.field);
        const bool angle=field=="angle",prob=field=="gustProbability" || field=="activityVariation",time=field.endsWith("Seconds"),scale=field.endsWith("K");
        const QString unit=angle?QStringLiteral("°"):time?QStringLiteral("s"):prob || scale?QStringLiteral("1"):QStringLiteral("m/s");
        const double lo=angle?-360:time?.1:scale?.01:0,hi=angle?360:prob?1:time?100000:scale?10:100;
        for(int end=0;end<2;++end){auto d=definition(key+(end?".max":".min"),QObject::tr("Preset %1 · %2 · %3").arg(r.preset+1).arg(generatorLabels.value(field,field),end?QObject::tr("maksimum"):QObject::tr("minimum")),"generator",end?r.high:r.low,lo,hi,prob?.01:.05,unit,false);d["preset"]=r.preset;rows.append(d);}
    }
    for(const auto &entry:QVector<QPair<QString,QString>>{{"strengthScale",QObject::tr("Automatyczna pogoda: skala siły")},{"variationScale",QObject::tr("Automatyczna pogoda: skala wahań")},{"gustScale",QObject::tr("Automatyczna pogoda: skala podmuchów")},{"trendScale",QObject::tr("Automatyczna pogoda: skala tendencji")},{"timeScale",QObject::tr("Automatyczna pogoda: skala czasu")}}) {
        auto d=definition("generator.auto."+entry.first,entry.second,"generator",1,entry.first=="timeScale"?.1:0,3,.05,"1",false);d["preset"]=-1;rows.append(d);
    }
    const QHash<QString,QString> automaticLabels{
        {"headProbability",QObject::tr("Szansa kierunku preferowanego przez skocznię")},{"characterNeutral",QObject::tr("Wagi charakteru pogody: neutralna skocznia")},{"characterStable",QObject::tr("Wagi charakteru pogody: spokojna skocznia")},{"characterLottery",QObject::tr("Wagi charakteru pogody: wietrzna skocznia")},
        {"temporal",QObject::tr("Amplituda zmian w czasie")},{"spatial",QObject::tr("Amplituda różnic na zeskoku")},{"gusts",QObject::tr("Amplituda podmuchów")},{"chance",QObject::tr("Szansa podmuchów")},{"times",QObject::tr("Czas podobnych warunków")},
        {"trendAmplitude",QObject::tr("Amplituda tendencji")},{"trendPeriod",QObject::tr("Okres tendencji")},{"transitions",QObject::tr("Czas przejścia tendencji")},{"activity",QObject::tr("Zmienność aktywności")},
        {"crossProbability",QObject::tr("Szansa dominującego wiatru bocznego")},{"strength.min",QObject::tr("Minimalna siła wiatru")},{"strength.max",QObject::tr("Maksymalna siła wiatru")},
        {"correlationKStable",QObject::tr("Długość podobnych warunków: spokojny charakter / K")},{"correlationKVariable",QObject::tr("Długość podobnych warunków: zmienny charakter / K")},{"gustWidthK",QObject::tr("Szerokość podmuchu / K")},
        {"rainProbability",QObject::tr("Szansa deszczu zamiast śniegu")},{"wetSnow",QObject::tr("Bazowa szansa opadów zimą")},{"wetColdBonus",QObject::tr("Dodatkowa szansa opadów w zimnych miesiącach")},{"wetNorthBonus",QObject::tr("Dodatkowa szansa opadów w krajach północnych")},{"wetMat",QObject::tr("Szansa opadów na igelicie")},
        {"precipitationPattern",QObject::tr("Wagi przebiegów opadów")},{"heavyPrecipitation.min",QObject::tr("Minimalne natężenie silnych opadów")},{"heavyPrecipitation.max",QObject::tr("Maksymalne natężenie silnych opadów")},
        {"precipitation.min",QObject::tr("Minimalne natężenie opadów")},{"precipitation.max",QObject::tr("Maksymalne natężenie opadów")},{"precipitationVariation.min",QObject::tr("Minimalna zmienność opadów")},{"precipitationVariation.max",QObject::tr("Maksymalna zmienność opadów")}
    };
    for(const auto &r:SimulationV2::weatherGeneratorScalars()){
        const auto field=QString::fromStdString(r.field);QString label=automaticLabels.value(field,field);if(r.level>=0)label+=QString(" [%1]").arg(r.level);
        const bool time=field=="times" || field=="trendPeriod" || field=="transitions";
        auto d=definition(QString::fromStdString(r.key),label,"generator",r.value,time?qMax(.1,r.minimum):r.minimum,r.maximum,time?1:.01,time?"s":QSet<QString>{"temporal","spatial","gusts","trendAmplitude","strength.min","strength.max"}.contains(field)?"m/s":"1",false);d["preset"]=-1;rows.append(d);
    }
    SimulationV2::WindParameters wind;visitWind(wind,add);
    SimulationV2::PrecipitationParameters precipitation;visitPrecipitation(precipitation,add);
    SimulationV2::FieldWindMeasurementParameters measurement;visitMeasurement(measurement,add);
    const auto input=[&](const char *key,const QString &label,const char *group,double value,double lo,double hi,double step,const char *unit="1"){rows.append(definition(key,label,group,value,lo,hi,step,unit,step==1));};
    input("gate",QObject::tr("Belka (0: automatyczna)"),"conditions",0,0,200,1,"1");
    input("gate.coachReduction",QObject::tr("Obniżenie belki przez trenera"),"rules",0,0,20,1,"1");
    input("gate.reference",QObject::tr("Belka odniesienia (0: wspólna baza)"),"rules",0,0,200,1,"1");
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
    input("hill.perk.0",QObject::tr("Perk skoczni: lotność / wybicie"),"hill",0,-4,4,1,"1");
    input("hill.perk.1",QObject::tr("Perk skoczni: trudne / przyjazne lądowanie"),"hill",0,-4,4,1,"1");
    input("hill.perk.2",QObject::tr("Perk skoczni: stabilny / loteryjny wiatr"),"hill",0,-4,4,1,"1");
    input("hill.perk.3",QObject::tr("Perk skoczni: kierunek wiatru"),"hill",0,-4,4,1,"1");
    input("weather.month",QObject::tr("Miesiąc propozycji pogody (0: nieznany)"),"weatherContext",0,0,12,1,"1");
    input("weather.surface",QObject::tr("Nawierzchnia: 0 śnieg, 1 igelit"),"weatherContext",0,0,1,1,"1");
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
    input("athlete.perk.0",QObject::tr("Podejście (−4…+4)"),"athlete",0,-4,4,1,"1");
    input("athlete.perk.1",QObject::tr("Specjalizacja (−4…+4)"),"athlete",0,-4,4,1,"1");
    input("athlete.perk.2",QObject::tr("Regularność (−4…+4)"),"athlete",0,-4,4,1,"1");
    input("field.perk.0",QObject::tr("Podejście (−4…+4)"),"field",0,-4,4,1,"1");
    input("field.perk.1",QObject::tr("Specjalizacja (−4…+4)"),"field",0,-4,4,1,"1");
    input("field.perk.2",QObject::tr("Regularność (−4…+4)"),"field",0,-4,4,1,"1");
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
    const QStringList events{QObject::tr("trening"),QObject::tr("seria próbna"),QObject::tr("kwalifikacje"),QObject::tr("konkurs")};
    const QStringList landingStyles{QObject::tr("Niechlujne"),QObject::tr("Niestaranne"),QObject::tr("Poprawne"),QObject::tr("Eleganckie"),QObject::tr("Stylista")};
    const QStringList landings{QObject::tr("telemark"),QObject::tr("dwie nogi"),QObject::tr("podpórka"),QObject::tr("upadek")};
    const QStringList presets{QObject::tr("Stały wiatr w plecy"),QObject::tr("Silny wiatr pod narty"),QObject::tr("Loteryjny"),QObject::tr("Zmienny boczny"),QObject::tr("Cisza"),QObject::tr("Stały pod narty"),QObject::tr("Silny w plecy"),QObject::tr("Stały boczny"),QObject::tr("Zmienny pod narty"),QObject::tr("Zmienny w plecy"),QObject::tr("Różne kierunki na zeskoku")};
    for(int i=0;i<rows.size();++i){auto d=rows[i].toObject();const auto key=d["key"].toString();auto label=d["label"].toString();int n=key.section('.',-1).toInt();
        if(key.startsWith("quality.eventDeviation."))label=QObject::tr("Losowość jakości — %1").arg(events.value(n));
        else if(key.startsWith("landing.landingPenalty."))label=QObject::tr("Kara w notach — %1").arg(landings.value(n));
        else if(key.startsWith("landing.styleMean."))label=QObject::tr("Bazowa nota — %1").arg(landingStyles.value(n));
        else if(key.startsWith("quality.") && label.contains('['))label=label.section(" [",0,0)+QObject::tr(" — poziom perka %1").arg(n);
        else if(key.startsWith("generator.preset."))label=presets.value(d["preset"].toInt())+" · "+label.section(" · ",1);
        else if(key.startsWith("generator.auto.") && label.contains('[')){
            QString context;
            if(key.contains("headProbability"))context=QObject::tr("poziom perka kierunku %1").arg(n);
            else if(key.contains("precipitationPattern"))context=QStringList{QObject::tr("stałe"),QObject::tr("zmienne"),QObject::tr("silne"),QObject::tr("narastające")}.value(n);
            else context=QStringList{QObject::tr("spokojna pogoda"),QObject::tr("zmienna pogoda"),QObject::tr("loteryjna pogoda")}.value(n);
            label=label.section(" [",0,0)+" — "+context;
        }
        if(key=="weather.surface")d["options"]=QJsonArray{QObject::tr("Śnieg"),QObject::tr("Igelit")};
        if(key=="curve.extension")d["options"]=QJsonArray{QObject::tr("Stały udział poza zakresem K"),QObject::tr("Liniowe przedłużenie krzywej")};
        d["label"]=label;rows[i]=d;
    }
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
