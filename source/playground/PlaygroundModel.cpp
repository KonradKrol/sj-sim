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
    if(key=="quality.overallWeight")return QObject::tr("Ile w jakości skoku waży OVR zamiast formy. 0,70 oznacza 70% OVR i 30% formy na skoczni o K odniesienia. Zwiększ, aby stałe umiejętności miały większe znaczenie.");
    if(key=="quality.formReferenceK")return QObject::tr("Na skoczni o tym K obowiązuje podstawowy udział OVR i formy. Na większych skoczniach forma waży więcej, na mniejszych mniej.");
    if(key=="quality.formKScale")return QObject::tr("Jak duża różnica punktu K jest potrzebna, aby wyraźnie zmienić udział formy. Większa wartość oznacza łagodniejszą zmianę między rozmiarami skoczni.");
    if(key=="quality.formHillAmplitude")return QObject::tr("Maksymalna korekta udziału formy zależna od wielkości skoczni. 0,05 to do 5 punktów procentowych; 0 wyłącza tę zależność.");
    if(key=="quality.negativeInfluence")return QObject::tr("Jak mocne są losowe straty względem losowych bonusów. Powyżej 0,50 straty są mocniejsze. To nie jest prawdopodobieństwo słabego skoku.");
    if(key=="quality.hillPreferenceStrength")return QObject::tr("Siła dodatkowego premiowania lotnika lub specjalisty małych skoczni przez charakter skoczni. Większa wartość wzmacnia tę różnicę; neutralny skoczek nie zyskuje.");
    if(key=="quality.noise")return QObject::tr("Kształt losowych wahań jakości skoku. Rozkład Laplace’a częściej daje duże odstępstwa niż normalny przy tym samym odchyleniu.");
    if(key=="flight.referenceQuality")return QObject::tr("Jakość skoku, dla której wyznaczono odległość odniesienia. Podniesienie tej wartości utrudnia osiągnięcie tej samej odległości przy niezmienionych umiejętnościach.");
    if(key=="flight.referenceDistanceAtK")return QObject::tr("Bazowa odległość jako część punktu K: 1 oznacza K, 1,10 oznacza 110% K. Większa wartość wydłuża bazowy skok.");
    if(key=="flight.qualityMetresAtK120")return QObject::tr("Ile metrów daje jeden dodatkowy punkt jakości na K120. Większa wartość zwiększa różnice odległości wynikające z OVR, formy i jakości wykonania.");
    if(key=="flight.referenceGate")return QObject::tr("Belka, dla której określono bazową odległość silnika. To punkt odniesienia modelu lotu, a nie wybór belki do tego eksperymentu.");
    if(key=="flight.gateMetresAtK120")return QObject::tr("Wpływ podniesienia belki o jeden stopień na K120. Większa wartość zwiększa różnice odległości między belkami.");
    if(key=="flight.gateTailwindInteraction")return QObject::tr("Dodatkowa siła wpływu belki przy wietrze w plecy. Większa wartość sprawia, że wybór belki silniej wpływa na odległość w tych warunkach.");
    if(key=="flight.stepMetres")return QObject::tr("Co ile metrów silnik przelicza lot. Mniejszy krok daje dokładniejsze obliczenia, ale wydłuża symulację; może też lekko zmienić wynik.");
    if(key=="flight.secondsPerK")return QObject::tr("Czas dotarcia do punktu K w modelu lotu. Większa wartość oznacza późniejsze napotkanie kolejnych podmuchów.");
    if(key=="flight.windReservePerMetre")return QObject::tr("Jak mocno wiatr wzdłuż skoczni zmienia tempo utraty zapasu lotu. Zwiększenie wzmacnia wpływ wiatru na odległość.");
    if(key=="flight.crosswindDrag")return QObject::tr("Siła niekorzystnego wpływu wiatru bocznego na lot. Większa wartość zwiększa stratę odległości.");
    if(key=="flight.windResponseMetres")return QObject::tr("Dystans potrzebny na dostosowanie reakcji do zmiany wiatru. Większa wartość wygładza wpływ krótkich podmuchów.");
    if(key=="flight.earlyWindSensitivity")return QObject::tr("Wrażliwość początku lotu na wiatr. Większa wartość wzmacnia znaczenie warunków tuż po wybiciu.");
    if(key=="flight.lateWindSensitivity")return QObject::tr("Wrażliwość końca lotu na wiatr. Większa wartość wzmacnia znaczenie warunków bliżej lądowania.");
    if(key=="flight.maximumWindAssistance")return QObject::tr("Ograniczenie pomocy wiatru w podtrzymywaniu lotu. Większa wartość pozwala wiatrowi mocniej zmniejszyć utratę zapasu lotu.");
    if(key=="flight.windVariabilityScale")return QObject::tr("Siła wiatru, przy której różnice w wykorzystaniu podmuchu stają się wyraźne. Większa wartość przesuwa ten efekt ku silniejszemu wiatrowi.");
    if(key=="flight.windHeadSpread")return QObject::tr("Jak różnie skoczkowie mogą wykorzystać silny wiatr pod narty. Większa wartość daje szerszy rozrzut reakcji; 0 wyłącza tę część losowości.");
    if(key=="flight.windTailSpread")return QObject::tr("Jak różne mogą być straty przy silnym wietrze w plecy. Większa wartość zwiększa rozrzut strat między skokami.");
    if(key=="flight.windCrossSpread")return QObject::tr("Jak różne mogą być straty przy silnym wietrze bocznym. Większa wartość zwiększa rozrzut strat między skokami.");
    if(key=="flight.windHeadSaturation")return QObject::tr("Od jakiej skali wiatru pod narty korzyść zaczyna rosnąć wyraźnie wolniej. Większa wartość opóźnia nasycenie; 0 je wyłącza. Ryzyko złego zdarzenia działa osobno.");
    if(key=="flight.windAdverseOnset")return QObject::tr("Skala siły wiatru, przy której rośnie ryzyko niekorzystnego zdarzenia. Większa wartość przesuwa ryzyko ku silniejszemu wiatrowi. To płynna zmiana, nie sztywny próg.");
    if(key=="flight.windAdverseExponent")return QObject::tr("Jak gwałtownie rośnie ryzyko wokół skali złych zdarzeń. Większa wartość oznacza wyraźniejsze przejście: mniej ryzyka poniżej tej skali, więcej powyżej.");
    if(key=="flight.geometryStartRealHs")return QObject::tr("Gdzie zaczyna narastać opór wypłaszczenia, jako część odległości początku wypłaszczenia. Większa wartość przesuwa początek oporu dalej.");
    if(key=="flight.usableLandingBeyondRealHsAtK")return QObject::tr("Dodatkowa długość użytecznego zeskoku jako część K. Większa wartość pozwala modelowi lotu sięgać dalej za początek wypłaszczenia.");
    if(key=="flight.geometryResistance")return QObject::tr("Jak mocno wypłaszczony zeskok ogranicza dalszy lot. Większa wartość mocniej ogranicza bardzo długie skoki.");
    if(key=="flight.exposureSections")return QObject::tr("Na ile odcinków dzielić zapis wiatru napotkanego podczas lotu. Więcej odcinków daje bardziej szczegółowe odtwarzanie.");
    if(key=="landing.sharedStyleDeviation")return QObject::tr("Losowa zmiana wspólna dla not wszystkich sędziów w jednym skoku. Większa wartość zwiększa wahania ogólnej oceny stylu.");
    if(key=="landing.judgeDeviation")return QObject::tr("Jak bardzo ocena pojedynczego sędziego może odbiegać od wspólnej oceny skoku. Większa wartość zwiększa różnice między sędziami.");
    if(key=="landing.difficultLandingStylePenalty")return QObject::tr("Dodatkowe obniżenie not za lądowanie daleko na zeskoku. Większa wartość mocniej obniża styl przy trudnych lądowaniach.");
    if(key=="wind.mean")return QObject::tr("Bazowy wiatr wzdłuż skoczni. Dodatnie wartości oznaczają wiatr pod narty, ujemne — w plecy. Lokalne korekty i podmuchy dodają się do tej wartości.");
    if(key=="wind.cross")return QObject::tr("Bazowy wiatr boczny. Wartość bezwzględna określa siłę, znak rozróżnia strony. Większa siła może utrudnić lot i lądowanie.");
    if(key=="wind.temporalAmplitude")return QObject::tr("Siła zmian wiatru w czasie konkursu. Większa wartość daje większe różnice warunków między kolejnymi chwilami.");
    if(key=="wind.spatialAmplitude")return QObject::tr("Siła różnic wiatru między miejscami nad zeskokiem. Większa wartość oznacza mniej jednolite warunki na trasie lotu.");
    if(key=="wind.correlationSeconds")return QObject::tr("Jak długo warunki pozostają podobne. Większa wartość oznacza wolniejsze zmiany wiatru.");
    if(key=="wind.correlationMetres")return QObject::tr("Na jak długim odcinku zeskoku wiatr pozostaje podobny. Większa wartość daje większe obszary z podobnymi warunkami.");
    if(key=="wind.gustAmplitude")return QObject::tr("Siła dodatkowych podmuchów. Zwiększ, aby podmuchy mocniej odróżniały się od tła.");
    if(key=="wind.gustProbability")return QObject::tr("Szansa aktywnego podmuchu w modelu wiatru. 0,20 oznacza 20% dla losowania w modelu, nie 20% konkursów z podmuchami.");
    if(key=="wind.gustPeriodSeconds")return QObject::tr("Odstęp czasowy używany do generowania podmuchów. Większa wartość rozciąga ich przebieg w czasie.");
    if(key=="wind.gustWidthMetres")return QObject::tr("Rozmiar podmuchu wzdłuż zeskoku. Większa wartość pozwala podmuchowi obejmować dłuższą część lotu.");
    if(key=="wind.maximumStrength")return QObject::tr("Górny limit łącznej siły wiatru. Zwiększenie dopuszcza silniejsze warunki, ale samo ich nie wymusza.");
    if(key=="wind.trendAmplitude")return QObject::tr("Siła powolnych zmian dominującego wiatru podczas konkursu. Większa wartość daje wyraźniejsze okresy korzystniejszych i gorszych warunków.");
    if(key=="wind.trendPeriodSeconds")return QObject::tr("Długość okresów dominującej tendencji wiatru. Większa wartość oznacza dłuższe okresy przed zmianą tendencji.");
    if(key=="wind.trendTransitionSeconds")return QObject::tr("Czas przechodzenia między tendencjami. Większa wartość daje łagodniejszą i wolniejszą zmianę.");
    if(key=="wind.activityVariation")return QObject::tr("Różnica między spokojniejszymi i bardziej aktywnymi okresami pogody. Większa wartość wzmacnia te kontrasty.");
    if(key=="wind.activityCorrelationSeconds")return QObject::tr("Długość epizodu spokojniejszej lub bardziej aktywnej pogody. Większa wartość wydłuża podobne epizody.");
    if(key=="wind.fastCorrelationSeconds")return QObject::tr("Tempo drobnych, szybkich zmian wiatru. Większa wartość spowalnia te zmiany.");
    if(key=="athlete.overall")return QObject::tr("Ogólny poziom umiejętności analizowanego skoczka, od 1 do 100. Wyższy OVR zwykle zwiększa odległość. Jego znaczenie względem formy ustawisz w regułach symulacji.");
    if(key=="athlete.form")return QObject::tr("Aktualna dyspozycja analizowanego skoczka, od 1 do 100. Wyższa forma zwykle wydłuża skoki. Jej udział zależy od ustawień silnika i wielkości skoczni.");
    if(key=="athlete.landing")return QObject::tr("Klasa stylu lądowania: od 0 do 4. Wyższa klasa poprawia noty. W tym modelu nie zmienia prawdopodobieństwa upadku.");
    if(key=="hill.realHs")return QObject::tr("Od tej odległości zeskok zaczyna się wypłaszczać. Większa wartość przesuwa trudne lądowania dalej. To osobny parametr od oficjalnego HS.");
    if(key=="hill.k")return QObject::tr("Punkt konstrukcyjny skoczni w metrach. Zmiana w laboratorium skaluje także HS i początek wypłaszczenia, zachowując ich proporcje. Punktacja wymaga osobnego ustawienia.");
    if(key=="gate")return QObject::tr("Belka startowa dla eksperymentu. Wyższa zwykle wydłuża skoki. 0 dobiera wspólną belkę automatycznie na podstawie wariantu „Balans z gry”.");
    if(key=="gate.reference")return QObject::tr("Belka odniesienia do rekompensaty punktowej. 0 używa wspólnej belki bazowej. Nie zmienia fizycznej belki, z której skacze zawodnik.");
    if(key=="gate.coachReduction")return QObject::tr("O ile stopni trener obniża belkę względem belki jury. Większa wartość skraca rozbieg; rekompensata zależy od reguł konkursu.");
    if(key=="clock.mode")return QObject::tr("Kontrolowane warunki: wszyscy startują w tej samej chwili modelu pogody. Przebieg konkursu: czas płynie między skokami i każdy może trafić na inny wiatr.");
    if(key=="clock.interval")return QObject::tr("Ile sekund czasu pogody upływa między startami. Zwiększenie zmienia warunki napotykane przez kolejnych skoczków, nie szybkość działania programu.");
    if(key=="dsq")return QObject::tr("Prawdopodobieństwo dyskwalifikacji pojedynczego skoku. 0,01 oznacza 1%. Działa, gdy dyskwalifikacje są włączone w zasadach.");
    if(group=="landing" && key.endsWith("Centre"))return QObject::tr("Odległość najszybszego wzrostu ryzyka jako wielokrotność początku wypłaszczenia. 1 oznacza dokładnie tę odległość. Zwiększenie przesuwa wzrost ryzyka dalej.");
    if(group=="landing" && key.endsWith("Width"))return QObject::tr("Szerokość przejścia od mniejszego do większego ryzyka. Większa wartość oznacza łagodniejszy wzrost rozłożony na dłuższy odcinek; nie oznacza po prostu mniejszego ryzyka wszędzie.");
    if(group=="landing" && key.endsWith("Base"))return QObject::tr("Bazowy poziom ryzyka przed wzrostem związanym z odległością. 0,01 oznacza 1%. Podpórkę losuje się po wykluczeniu upadku, a dwie nogi po wykluczeniu upadku i podpórki.");
    if(group=="landing" && key.endsWith("Increase"))return QObject::tr("Dodatkowy wzrost ryzyka przy długich skokach. Większa wartość zwiększa ryzyko za środkiem krzywej. Podpórka i dwie nogi są oceniane dopiero po wykluczeniu poważniejszego błędu.");
    if(group=="landing" && key.startsWith("landing.crosswind"))return QObject::tr("Dodatkowe ryzyko za każdy 1 m/s bocznego wiatru. Większa wartość mocniej utrudnia ten rodzaj lądowania. 0 wyłącza ten wpływ.");
    if(key.startsWith("landing.styleMean."))return QObject::tr("Bazowa nota jednego sędziego dla tej klasy lądowania. Zwiększenie podnosi noty; kary za błędy są odejmowane osobno.");
    if(key.startsWith("landing.landingPenalty."))return QObject::tr("Ile punktów odejmuje sędzia za ten rodzaj lądowania. Większa wartość obniża noty, ale nie zwiększa ryzyka tego zdarzenia.");
    if(key.startsWith("flight.wind") && key.endsWith("AdverseChance"))return QObject::tr("Górna granica szansy niekorzystnego zdarzenia przy bardzo silnym wietrze. 0,20 oznacza do 20%. Zwiększenie podnosi ryzyko; rzeczywista szansa rośnie płynnie z siłą wiatru.");
    if(key.startsWith("flight.wind") && key.endsWith("AdverseLoss"))return QObject::tr("Jak dotkliwa jest strata po wylosowaniu niekorzystnego zdarzenia. Większa wartość mocniej skraca skok; nie zwiększa szansy wystąpienia zdarzenia.");
    if(key=="flight.precipitationLossPerLevel")return QObject::tr("Ile początkowego zapasu lotu odbiera jeden poziom śniegu. 0,008 oznacza 0,8% na poziom, czyli 8% przy natężeniu 10. To nie jest bezpośrednia strata metrów.");
    if(key=="flight.rainLossPerLevel")return QObject::tr("Utrata początkowego zapasu lotu za każdy poziom deszczu powyżej progu wpływu. 0,003 oznacza 0,3% na poziom; strata od ulewy dodaje się osobno.");
    if(key=="flight.rainLossThreshold")return QObject::tr("Do tego natężenia liniowa strata od deszczu wynosi zero. Podniesienie progu zmniejsza ten składnik straty. Nie wyłącza osobnego wpływu ulewy.");
    if(key=="flight.rainHeavyThreshold")return QObject::tr("Natężenie, powyżej którego zaczyna działać dodatkowa strata od ulewy. Podniesienie progu ogranicza ją do mocniejszych opadów.");
    if(key=="flight.rainHeavyLossAtMaximum")return QObject::tr("Dodatkowa część zapasu lotu tracona przy deszczu o natężeniu 10. 0,15 oznacza 15%; liniowa strata od deszczu jest doliczana osobno.");
    if(key=="flight.rainHeavyExponent")return QObject::tr("Kształt narastania dodatkowej straty od ulewy między jej progiem a natężeniem 10. Większy wykładnik zmniejsza stratę pośrednią, bez zmiany straty przy 10.");
    if(key=="wind.gustTravelMetresPerSecond")return QObject::tr("O ile metrów na sekundę przemieszcza się podmuch wzdłuż zeskoku. Większa wartość szybciej przenosi jego wpływ między miejscami; 0 pozostawia go w miejscu.");
    if(key=="precipitation.scenario")return QObject::tr("Wybierz przebieg opadów. Stałe utrzymują natężenie, zmienne wahają się wokół niego, silne mają bazę co najmniej 8, a narastające zwiększają natężenie z czasem.");
    if(key=="precipitation.kind")return QObject::tr("Śnieg i deszcz mają osobne wzory straty zapasu lotu. Śnieg działa liniowo od pierwszego poziomu; deszcz ma próg wpływu oraz dodatkowy składnik ulewy.");
    if(key=="precipitation.intensity")return QObject::tr("Bazowe natężenie w skali 0–10. Wyższe natężenie skraca skoki według współczynników śniegu lub deszczu. W przebiegu silnym baza wynosi co najmniej 8.");
    if(key=="precipitation.variation")return QObject::tr("Siła losowych wahań natężenia. Działa w przebiegu zmiennym, słabiej w silnym i narastającym; w stałym i bez opadów nie ma wpływu.");
    if(key=="precipitation.correlationSeconds")return QObject::tr("Skala czasu podobnego natężenia opadów. Większa wartość spowalnia wahania; nie zmienia bazowej siły opadów.");
    if(key=="precipitation.increasePerMinute")return QObject::tr("O ile poziomów na minutę rośnie natężenie w przebiegu narastającym. 0,5 daje wzrost o 1 poziom w dwie minuty, do limitu 10.");
    if(key=="measurement.temporalSamples")return QObject::tr("Liczba odczytów z każdego czujnika podczas lotu. Więcej próbek dokładniej uwzględnia zmiany w czasie, ale zwiększa koszt obliczeń. Pomiar przed skokiem zawsze używa jednej próbki; metoda dotychczasowa ma własne próbkowanie.");
    if(key=="measurement.extentK")return QObject::tr("Zasięg rozmieszczenia czujników jako część K: 1,2 na K120 oznacza 144 m. Nie zmienia lotu ani metody opartej na wietrze napotkanym przez skoczka. Metoda dotychczasowa używa własnego zasięgu.");
    if(key=="measurement.denseSpacingMetres")return QObject::tr("Docelowy odstęp między czujnikami gęstej sieci. Mniejszy odstęp daje więcej czujników, do limitu 128. Pozostałe metody go nie używają.");
    if(key=="measurement.weightPeak")return QObject::tr("Miejsce największej wagi czujników w przybliżeniu FIS, jako część zasięgu pomiaru. 0,5 wyróżnia środek sieci, 0,75 dalszą część zeskoku.");
    if(key=="measurement.weightStrength")return QObject::tr("Jak mocno czujniki bliżej szczytu wag dominują w przybliżeniu FIS. 0 daje równe wagi. Zmienia rekompensatę, a nie fizyczny wpływ wiatru.");
    if(key=="athlete.perk.0")return QObject::tr("0: bez perka; +1…+3: Błyszczy na treningach; −1…−3: Oszczędza siły. Większa wartość bezwzględna wzmacnia wybrany perk. Zmieniasz tylko analizowanego skoczka.");
    if(key=="athlete.perk.1")return QObject::tr("0: bez perka; +1…+3: Lotnik; −1…−3: Specjalista małych skoczni. Lotnik zyskuje przy przewadze lotu, specjalista przy przewadze wybicia. Zmieniasz tylko analizowanego skoczka.");
    if(key=="athlete.perk.2")return QObject::tr("0: bez perka; +1…+3: Regularny; −1…−3: Nieprzewidywalny. Dodatni poziom ogranicza rozrzut, ujemny zwiększa go, zwłaszcza w stronę strat. Siłę określają współczynniki perków.");
    if(key=="hill.perk.0")return QObject::tr("Dodatnie poziomy premiują wybicie, ujemne lot; 0 jest neutralne. Działa na specjalizację skoczka. Ręcznie ustawiona preferencja skoczni ma pierwszeństwo.");
    if(key=="hill.perk.1")return QObject::tr("Dodatnie poziomy przesuwają wypłaszczenie dalej, ujemne bliżej HS. Ręczna zmiana początku wypłaszczenia ma pierwszeństwo przed tym perkiem.");
    if(key=="hill.perk.2")return QObject::tr("Dodatnie poziomy zwiększają szansę loteryjnej pogody, ujemne spokojnej. Działa tylko przy automatycznej propozycji pogody; nie zmienia ręcznego pola ani wybranego presetu.");
    if(key=="hill.perk.3")return QObject::tr("Dodatnie poziomy zwiększają szansę wiatru pod narty, ujemne w plecy. Dotyczy automatycznej propozycji pogody, nie ręcznego pola ani wybranego presetu.");
    if(key=="hill.hs")return QObject::tr("Oficjalny rozmiar skoczni w metrach. Służy m.in. regule 95% HS. Początek wypłaszczenia jest osobnym parametrem; samo zwiększenie HS nie zwiększa zapasu lotu.");
    if(key=="hill.takeoffBias")return QObject::tr("Przesuwa udział wybicia i lotu w działaniu specjalizacji: +1 w stronę wybicia, −1 w stronę lotu. Nie daje dodatkowego skilla skoczkowi bez perka specjalizacji.");
    if(key=="hill.pointsAtK")return QObject::tr("Punkty za odległość równą K. Większa wartość podnosi bazę punktacji każdego skoku, bez zmiany jego odległości.");
    if(key=="hill.pointsPerMetre")return QObject::tr("Ile punktów daje każdy metr ponad K lub odbiera każdy metr przed K. Zwiększenie wzmacnia znaczenie odległości względem not i rekompensat.");
    if(key=="hill.gatePoints")return QObject::tr("Rekompensata punktowa za obniżenie belki o jeden stopień względem odniesienia. Nie zmienia wpływu belki na odległość.");
    if(key=="hill.headwindPoints")return QObject::tr("Ile punktów odjąć za każdy 1 m/s zmierzonego wiatru pod narty, gdy rekompensaty są włączone. Nie zmienia samego lotu.");
    if(key=="hill.tailwindPoints")return QObject::tr("Ile punktów dodać za każdy 1 m/s zmierzonego wiatru w plecy, gdy rekompensaty są włączone. Nie zmienia samego lotu.");
    if(key=="gate")return QObject::tr("Belka startowa eksperymentu. Wyższa zwykle wydłuża skoki. 0 uruchamia dobór automatyczny; nie oznacza fizycznej belki zerowej.");
    if(key=="gate.reference")return QObject::tr("Belka, względem której naliczana jest rekompensata. 0 oznacza wspólną bazę eksperymentu. To nie belka odniesienia wzoru odległości.");
    if(key=="gate.coachReduction")return QObject::tr("O ile stopni trener obniża belkę wybranego skoczka. Zmniejsza odległość; ewentualne punkty za obniżenie zależą od zasad, w tym reguły 95% HS.");
    if(key=="event")return QObject::tr("Rodzaj skoku wpływa na perki podejścia i odchylenie jakości. Błyszczy na treningach ma połowę bonusu w kwalifikacjach, a karę w konkursie.");
    if(key=="weather.month")return QObject::tr("Miesiąc używany przez automatyczne losowanie opadów; 0 pomija porę roku. Nie ustawia daty sezonu. Na półkuli południowej pory roku są odwrócone.");
    if(key=="weather.surface")return QObject::tr("Nawierzchnia dla automatycznego losowania opadów. Igelit wybiera deszcz, śnieg dopuszcza zależność od miesiąca. Nie nadpisuje ręcznie ustawionego rodzaju opadów.");
    if(key=="field.skill")return QObject::tr("Środkowy OVR syntetycznej stawki. Pozostali skoczkowie rozkładają się wokół niego według rozpiętości OVR. Nie zmienia zawodników z wczytanej paczki.");
    if(key=="field.spread")return QObject::tr("Różnica OVR między najmocniejszym a najsłabszym w syntetycznej stawce. 30 wokół OVR 70 daje zakres 55–85, ograniczony do 1–100. Nie zmienia wczytanej paczki.");
    if(key=="field.form")return QObject::tr("Forma pozostałych zawodników, także z wczytanej paczki po ręcznej zmianie tego pola. Analizowany skoczek ma osobne pole formy; indywidualne poprawki mają pierwszeństwo.");
    if(key=="field.landing")return QObject::tr("Klasa stylu pozostałych zawodników, od 0 do 4; po ręcznej zmianie nadpisuje także paczkę. Wyższa poprawia noty, nie zmienia ryzyka upadku. Indywidualne poprawki mają pierwszeństwo.");
    if(key=="rules.dsq")return QObject::tr("Włącz losowanie dyskwalifikacji z ustawionym ryzykiem DSQ. Wyłączenie usuwa DSQ z eksperymentu, niezależnie od wartości ryzyka.");
    if(key=="rules.windCompensation")return QObject::tr("Dolicz rekompensatę za zmierzony wiatr. Wyłączenie zmienia punktację; wiatr nadal wpływa na lot.");
    if(key=="rules.gateCompensation")return QObject::tr("Dolicz rekompensatę za różnicę belki względem odniesienia. Wyłączenie nie usuwa wpływu belki na odległość.");
    if(key=="rules.judges")return QObject::tr("Dolicz trzy środkowe noty spośród pięciu sędziów. Wyłączenie usuwa punkty za styl, bez zmiany odległości i ryzyka lądowania.");
    if(key=="rules.95hs")return QObject::tr("Przy obniżeniu belki przez trenera przyznaj rekompensatę dopiero po osiągnięciu 95% HS. Nie dotyczy zwykłej zmiany belki przez jury.");
    if(key=="rules.measurement")return QObject::tr("Sposób wyznaczenia średniego wiatru do rekompensaty. Możesz porównać odczyt przed startem, czujniki podczas lotu lub wiatr napotkany przez skoczka. Nie zmienia fizycznego pola wiatru.");
    if(key=="analysis.duration")return QObject::tr("Jak długi przebieg wiatru obejmuje analiza, w sekundach. Zwiększenie pokazuje dłuższy fragment pogody; nie wydłuża konkursu.");
    if(key=="analysis.time")return QObject::tr("Sekunda modelu pogody, dla której pokazać przekrój wiatru wzdłuż zeskoku. Nie przesuwa czasu startu w konkursie.");
    if(key=="analysis.distance")return QObject::tr("Miejsce nad zeskokiem, w metrach od progu, dla którego analizować zmiany wiatru w czasie.");
    if(key=="analysis.scale")return QObject::tr("Zakres kolorów mapy wiatru po obu stronach zera. Mniejszy zakres mocniej uwidacznia drobne różnice, ale nasyca kolory dla silnego wiatru.");
    if(key=="analysis.timeSamples")return QObject::tr("Liczba chwil próbkowania na wykresie i mapie wiatru. Więcej próbek daje dokładniejszy obraz zmian, kosztem dłuższych obliczeń.");
    if(key=="analysis.spaceSamples")return QObject::tr("Liczba miejsc próbkowania wzdłuż zeskoku. Więcej próbek ujawnia drobniejsze różnice przestrzenne, ale zwiększa koszt analizy.");
    if(key=="analysis.jumpSamples")return QObject::tr("Liczba testowych skoków do analizy wiatru. Większa próba stabilizuje rozkłady i zależności, ale wydłuża obliczenia.");
    if(key=="curve.extension")return QObject::tr("Zachowanie udziału wybicia poza skrajnymi punktami K krzywej: utrzymaj skrajną wartość albo przedłuż ostatni odcinek liniowo. Wynik pozostaje w zakresie 0–1.");
    if(key.startsWith("quality.eventDeviation."))return QObject::tr("Odchylenie losowej jakości w tym rodzaju skoku. Większa wartość zwiększa rozrzut zarówno strat, jak bonusów; 0 usuwa ten składnik losowości. Jednostką jest punkt jakości, nie metr.");
    if(key.startsWith("quality.specialistBias."))return QObject::tr("Siła specjalizacji na tym poziomie perka. Wzmacnia korzyść lotnika na skoczni preferującej lot oraz jego stratę na skoczni preferującej wybicie; dla specjalisty małych skoczni działa odwrotnie.");
    if(key.startsWith("quality.trainingBonus."))return QObject::tr("Punkty jakości dodawane za Błyszczy na treningach na tym poziomie. Pełny bonus działa w treningu i próbie, połowa w kwalifikacjach; w konkursie używana jest osobna kara.");
    if(key.startsWith("quality.competitionPenalty."))return QObject::tr("Punkty jakości odejmowane w konkursie za Błyszczy na treningach na tym poziomie. Większa kara skraca skoki konkursowe; nie zmienia bonusu w treningu.");
    if(key.startsWith("quality.conservedEffortPenalty."))return QObject::tr("Punkty jakości odejmowane w treningu i próbie za Oszczędza siły na tym poziomie. Nie jest to bonus konkursowy; w konkursie i kwalifikacjach ta kara nie działa.");
    if(key.startsWith("quality.conservedEffortDeviation."))return QObject::tr("Mnożnik rozrzutu jakości w treningu i próbie za Oszczędza siły na tym poziomie. 1 nie zmienia rozrzutu, 1,5 zwiększa go o połowę. Nie działa w konkursie ani kwalifikacjach.");
    if(key.startsWith("quality.regularNegative."))return QObject::tr("Mnożnik losowych strat jakości za Regularny na tym poziomie. 0,5 zmniejsza je o połowę, 1 pozostawia bez zmian. Nie zmienia dodatnich losowań.");
    if(key.startsWith("quality.regularPositive."))return QObject::tr("Mnożnik losowych bonusów jakości za Regularny na tym poziomie. 0,5 zmniejsza je o połowę, 1 pozostawia bez zmian. Nie zmienia ujemnych losowań.");
    if(key.startsWith("quality.unpredictableNegative."))return QObject::tr("Mnożnik losowych strat jakości za Nieprzewidywalny na tym poziomie. 1,5 zwiększa wielkość strat o połowę, a nie szansę ich wylosowania.");
    if(key.startsWith("quality.unpredictablePositive."))return QObject::tr("Mnożnik losowych bonusów jakości za Nieprzewidywalny na tym poziomie. 1,2 zwiększa wielkość bonusów o 20%, a nie szansę ich wylosowania.");
    if(key.startsWith("field.perk."))return effectDescription("athlete.perk."+key.section('.',-1),"athlete")+" "+QObject::tr("W tym polu ustawiasz pozostałych skoczków stawki, także z wczytanej paczki; nie analizowanego zawodnika. Indywidualne poprawki zawodników mają pierwszeństwo.");
    if(key.startsWith("wind.reference.")) {
        if(key.endsWith("positionK"))return QObject::tr("Położenie punktu korekty wiatru jako część K: 0,5 na K120 to 60 m od progu. Między punktami wiatr zmienia się płynnie; nie są to sztywne sektory.");
        if(key.endsWith("head"))return QObject::tr("Lokalna korekta bazowego wiatru wzdłuż skoczni w tym punkcie. +1 dodaje 1 m/s pod narty, −1 w plecy. Korekty między punktami są interpolowane.");
        return QObject::tr("Lokalna korekta bocznego wiatru w tym punkcie, w m/s. Znak określa stronę. Dodaje się do bazowego wiatru bocznego i płynnie łączy z sąsiednimi punktami.");
    }
    if(key.startsWith("generator.preset.")) {
        const auto field=key.section('.',3,3);
        QString help;
        if(field=="strength")help=QObject::tr("Bazowa siła wiatru przed doliczeniem lokalnych różnic i zmian w czasie, w m/s.");
        else if(field=="strength2")help=QObject::tr("Bazowa siła wiatru w rzadszym wariancie ciszy, wybieranym z szansą 15%, w m/s.");
        else if(field=="angle")help=QObject::tr("Kąt w stopniach: w presetach o ustalonym kierunku jest odchyleniem od niego, a w ciszy i loterii pełnym kierunkiem 0–360°.");
        else if(field=="difference")help=QObject::tr("Maksymalna losowa korekta składowych wiatru w kolejnych punktach zeskoku, w m/s. Większa wartość daje bardziej różnorodny profil początkowy.");
        else if(field=="mixedAmplitude")help=QObject::tr("Siła odwrócenia wiatru między częściami zeskoku w scenariuszu różnych kierunków, w m/s.");
        else if(field=="temporalAmplitude")help=QObject::tr("Siła wahań wiatru w czasie, w m/s. Większa wartość zwiększa różnice warunków między chwilami konkursu.");
        else if(field=="spatialAmplitude")help=QObject::tr("Siła różnic wiatru wzdłuż zeskoku, w m/s. Większa wartość daje mniej jednolite warunki na trasie lotu.");
        else if(field=="trendAmplitude")help=QObject::tr("Siła zmian dominującej tendencji wiatru podczas konkursu, w m/s. Większa wartość dopuszcza wyraźniejsze zmiany kierunku i siły.");
        else if(field=="trendPeriodSeconds")help=QObject::tr("Długość okresu tendencji w sekundach. Większa wartość oznacza dłuższe utrzymywanie się podobnego tła wiatru.");
        else if(field=="gustAmplitude")help=QObject::tr("Siła dodatkowych podmuchów w m/s. Większa wartość wzmacnia chwilowe odstępstwa od tła.");
        else if(field=="gustProbability")help=QObject::tr("Szansa aktywnego podmuchu w modelu. 0,2 oznacza 20% dla losowania podmuchu, nie odsetek konkursów.");
        else if(field=="correlationSeconds")help=QObject::tr("Skala podobieństwa wiatru w czasie, w sekundach. Większa wartość spowalnia zmiany.");
        else if(field=="activityVariation")help=QObject::tr("Kontrast między spokojnymi i aktywnymi okresami konkursu. Większa wartość mocniej różnicuje te epizody.");
        else if(field=="correlationK")help=QObject::tr("Długość podobnych warunków jako część K. 0,5 na K120 oznacza skalę 60 m; większa wartość daje szersze obszary podobnego wiatru.");
        else if(field=="gustWidthK")help=QObject::tr("Szerokość podmuchu jako część K. 0,3 na K120 oznacza 36 m; większa wartość obejmuje dłuższą część lotu.");
        else if(field=="transitionSeconds")help=QObject::tr("Czas przejścia między tendencjami w sekundach. Większa wartość wygładza zmianę dominującego wiatru.");
        return help+" "+(key.endsWith(".min")?QObject::tr("Dolna granica losowania tego parametru w wybranym presecie; nie może przekroczyć maksimum."):QObject::tr("Górna granica losowania tego parametru w wybranym presecie; nie może być mniejsza od minimum."));
    }
    if(key.startsWith("generator.auto.")) {
        const auto field=key.mid(15);
        if(field=="strengthScale")return QObject::tr("Mnożnik bazowej siły automatycznie proponowanego wiatru. 1 zachowuje siłę, 0,5 zmniejsza ją o połowę. Nie skaluje podmuchów ani zmian w czasie.");
        if(field=="variationScale")return QObject::tr("Wspólny mnożnik wahań w czasie i różnic przestrzennych w automatycznej propozycji. Nie zmienia bazowej siły, podmuchów ani tendencji.");
        if(field=="gustScale")return QObject::tr("Mnożnik siły podmuchów w automatycznej propozycji. 2 podwaja amplitudę, bez zmiany prawdopodobieństwa podmuchu.");
        if(field=="trendScale")return QObject::tr("Mnożnik amplitudy tendencji w automatycznej propozycji. Większa wartość wzmacnia zmiany dominującego wiatru, bez zmiany ich okresu.");
        if(field=="timeScale")return QObject::tr("Mnożnik czasu korelacji, okresu tendencji i czasu przejścia w automatycznej propozycji. 2 dwukrotnie spowalnia te procesy.");
        if(field=="crossProbability")return QObject::tr("Szansa dominującego wiatru bocznego w automatycznej propozycji. 0,15 oznacza 15%. Nie jest siłą wiatru bocznego.");
        if(field=="correlationKStable")return QObject::tr("Skala podobnego wiatru wzdłuż zeskoku w spokojnym wariancie automatycznej pogody, jako część K. Większa wartość daje dłuższe obszary podobnych warunków.");
        if(field=="correlationKVariable")return QObject::tr("Skala podobnego wiatru wzdłuż zeskoku w zmiennym i loteryjnym wariancie automatycznej pogody, jako część K. Większa wartość daje dłuższe obszary podobnych warunków.");
        if(field=="gustWidthK")return QObject::tr("Szerokość automatycznie proponowanego podmuchu jako część K. 0,35 na K120 to 42 m.");
        if(field=="rainProbability")return QObject::tr("Szansa wybrania deszczu zamiast śniegu poza zimnymi miesiącami, na nawierzchni śnieżnej. Na igelicie opadem zawsze jest deszcz; samo wystąpienie opadów jest losowane osobno.");
        if(field=="wetSnow")return QObject::tr("Bazowa szansa opadów na nawierzchni śnieżnej. Dodatki za zimny miesiąc i kraj północny sumują się z nią, do maksymalnie 100%.");
        if(field=="wetColdBonus")return QObject::tr("Dodatkowe prawdopodobieństwo opadów w zimnym miesiącu na nawierzchni śnieżnej. 0,1 dodaje 10 punktów procentowych do bazowej szansy.");
        if(field=="wetNorthBonus")return QObject::tr("Dodatkowe prawdopodobieństwo opadów w Norwegii, Szwecji, Finlandii i Kanadzie, na śniegu. 0,1 dodaje 10 punktów procentowych.");
        if(field=="wetMat")return QObject::tr("Szansa opadów na igelicie; 0,3 oznacza 30%. Wylosowanym opadem jest deszcz.");
        if(field.startsWith("headProbability."))return QObject::tr("Szansa wiatru pod narty dla tego poziomu perka kierunku. Przy ujemnym perku szansa jest odwrócona, np. 0,84 daje 84% wiatru w plecy. Poziom 0 dotyczy skoczni neutralnej.");
        if(field.startsWith("character"))return QObject::tr("Względna waga tego charakteru pogody: spokojnego, zmiennego albo loteryjnego. Zwiększenie podnosi jego szansę względem pozostałych wag. Perk skoczni płynnie miesza wagi neutralne z wagami stabilnymi lub loteryjnymi.");
        if(field.startsWith("stable.") || field.startsWith("unstable."))return QObject::tr("Zapasowa waga charakteru pogody dla skoczni stabilnej lub loteryjnej. Działa tylko przy braku odpowiedniej wagi characterStable lub characterLottery; zwykle zmieniaj tamte pola.");
        if(field.startsWith("precipitationPattern."))return QObject::tr("Względna waga tego przebiegu opadów, gdy opady zostały już wylosowane. Większa waga zwiększa jego szansę względem pozostałych; nie zwiększa szansy wystąpienia samych opadów.");
        if(field.startsWith("strength."))return QObject::tr("Granica losowania bazowej siły wiatru w automatycznej propozycji, w m/s. Podmuchy i lokalne różnice są dodawane osobno. Minimum nie może przekraczać maksimum.");
        if(field.startsWith("heavyPrecipitation."))return QObject::tr("Granica losowania początkowego natężenia silnych opadów, w skali 0–10. W przebiegu silnym model i tak stosuje bazę co najmniej 8.");
        if(field.startsWith("precipitationVariation."))return QObject::tr("Granica losowania amplitudy wahań w zmiennym przebiegu opadów. Większa amplituda daje większe różnice natężenia między chwilami; nie ustala jego poziomu bazowego.");
        if(field.startsWith("precipitation."))return QObject::tr("Granica losowania bazowego natężenia zwykłych opadów, w skali 0–10. Silny przebieg ma osobny zakres. Minimum nie może przekraczać maksimum.");
        const auto quantity=field.section('.',0,0);
        if(quantity=="temporal")return QObject::tr("Amplituda wahań wiatru w czasie dla wskazanego charakteru pogody, w m/s. Większa wartość zwiększa różnice między chwilami konkursu.");
        if(quantity=="spatial")return QObject::tr("Amplituda różnic wiatru wzdłuż zeskoku dla wskazanego charakteru pogody, w m/s. Większa wartość daje mniej jednolite warunki na trasie lotu.");
        if(quantity=="gusts")return QObject::tr("Siła podmuchów dla wskazanego charakteru pogody, w m/s. Nie zmienia ich szansy wystąpienia.");
        if(quantity=="chance")return QObject::tr("Szansa aktywnego podmuchu dla wskazanego charakteru pogody. 0,25 oznacza 25% dla losowania w modelu podmuchów.");
        if(quantity=="times")return QObject::tr("Czas korelacji wiatru dla wskazanego charakteru pogody, w sekundach. Większa wartość spowalnia zmiany.");
        if(quantity=="trendAmplitude")return QObject::tr("Amplituda dominującej tendencji dla wskazanego charakteru pogody, w m/s. Większa wartość daje silniejsze zmiany tła wiatru.");
        if(quantity=="trendPeriod")return QObject::tr("Okres tendencji dla wskazanego charakteru pogody, w sekundach. Większa wartość wydłuża utrzymywanie się tła wiatru.");
        if(quantity=="transitions")return QObject::tr("Czas przejścia między tendencjami dla wskazanego charakteru pogody, w sekundach. Większa wartość daje łagodniejszą zmianę.");
        if(quantity=="activity")return QObject::tr("Kontrast spokojnych i aktywnych epizodów dla wskazanego charakteru pogody. Większa wartość silniej różnicuje ich aktywność.");
    }

    if(key.startsWith("curve.k."))return QObject::tr("Punkt K kotwicy krzywej specjalizacji. Przesunięcie zmienia rozmiary skoczni, między którymi interpolowany jest udział wybicia. Punkty K muszą rosnąć.");
    if(key.startsWith("curve.takeoff."))return QObject::tr("Udział wybicia w tej kotwicy: 0,60 oznacza 60% wybicia i 40% lotu. Zmienia korzyści specjalisty względem lotnika; udziały muszą maleć lub pozostawać równe wraz ze wzrostem K.");
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
    input("hill.perk.0",QObject::tr("Perk skoczni: lotność / wybicie"),"hill",0,-3,3,1,"1");
    input("hill.perk.1",QObject::tr("Perk skoczni: trudne / przyjazne lądowanie"),"hill",0,-3,3,1,"1");
    input("hill.perk.2",QObject::tr("Perk skoczni: stabilny / loteryjny wiatr"),"hill",0,-3,3,1,"1");
    input("hill.perk.3",QObject::tr("Perk skoczni: kierunek wiatru"),"hill",0,-3,3,1,"1");
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
