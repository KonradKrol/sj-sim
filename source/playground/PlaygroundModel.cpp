#include "PlaygroundModel.h"
#include "../simulation-v2/WeatherCalibration.h"
#include <QObject>
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
    if(group=="generator")return QObject::tr("Ustawienie generatora określa, jakie warunki są losowane przed konkursem. Minimum i maksimum wyznaczają zakres losowania. Waga oznacza częstość względem pozostałych wag, nie procent. Wpływ wiatru na skok ustawisz osobno w modelu lotu.");
    if(key.startsWith("hill.perk."))return QObject::tr("Poziomy −3…+3. Pary: lotność / wybicie, trudne / przyjazne lądowanie, stabilność / loteryjność, wiatr w plecy / pod narty. Ręczna preferencja lub real HS ma pierwszeństwo przed odpowiednim perkiem.");
    if(key.startsWith("quality.form") || key=="quality.overallWeight")return QObject::tr("Udział formy = 1 − udział OVR + amplituda × tanh((K − K odniesienia) / skala), ograniczony do 0–1. Amplituda 0 wyłącza zależność od K.");
    if(key.startsWith("flight.wind") && key.contains("Adverse"))return QObject::tr("Szansa = maksimum × r^wykładnik / (1 + r^wykładnik), gdzie r to siła wiatru / skala złych zdarzeń. Siła zdarzenia zwiększa zużycie zapasu lotu; zero wyłącza stratę.");
    if(key=="flight.windHeadSaturation")return QObject::tr("Pomoc wiatru pod narty rośnie coraz wolniej. Zero wyłącza nasycenie. Złe zdarzenia nadal zależą od rzeczywistej siły wiatru.");
    if(key=="flight.windVariabilityScale" || key.endsWith("Spread"))return QObject::tr("Rozrzut rośnie płynnie z siłą wiatru. Reakcja jest losowana raz na skok, nie w każdym kroku lotu. Zerowy rozrzut wyłącza ten składnik losowości.");
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
        auto d=definition(QString::fromStdString(r.key),label,"generator",r.value,time?qMax(.1,r.minimum):r.minimum,r.maximum,time?1:.01,time?"s":"1",false);d["preset"]=-1;rows.append(d);
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
    const QStringList landings{QObject::tr("telemark"),QObject::tr("dwie nogi"),QObject::tr("podpórka"),QObject::tr("upadek")};
    const QStringList presets{QObject::tr("Stały wiatr w plecy"),QObject::tr("Silny wiatr pod narty"),QObject::tr("Loteryjny"),QObject::tr("Zmienny boczny"),QObject::tr("Cisza"),QObject::tr("Stały pod narty"),QObject::tr("Silny w plecy"),QObject::tr("Stały boczny"),QObject::tr("Zmienny pod narty"),QObject::tr("Zmienny w plecy"),QObject::tr("Różne kierunki na zeskoku")};
    for(int i=0;i<rows.size();++i){auto d=rows[i].toObject();const auto key=d["key"].toString();auto label=d["label"].toString();int n=key.section('.',-1).toInt();
        if(key.startsWith("quality.eventDeviation."))label=QObject::tr("Losowość jakości — %1").arg(events.value(n));
        else if(key.startsWith("landing.landingPenalty."))label=QObject::tr("Kara w notach — %1").arg(landings.value(n));
        else if(key.startsWith("landing.styleMean."))label=QObject::tr("Bazowa nota — klasa lądowania %1").arg(n);
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
