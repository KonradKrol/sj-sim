#include <QObject>
#include "PlaygroundExperiment.h"
#include <QCoreApplication>
#include <QFile>
#include <QSaveFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QTextStream>
#include <stdexcept>
#include <csignal>
#include <thread>
#include <chrono>
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
#include <QStringConverter>
#endif

namespace Playground {
namespace {
std::atomic_bool interrupted{false};
static_assert(std::atomic_bool::is_always_lock_free,"CLI cancellation requires lock-free atomics");
void interruptedSignal(int){interrupted.store(true,std::memory_order_relaxed);}
struct CancellationMonitor {
    std::atomic_bool finished{false};std::thread thread;
    explicit CancellationMonitor(const std::shared_ptr<Control> &control):thread([this,control]{while(!finished.load(std::memory_order_relaxed)){if(interrupted.load(std::memory_order_relaxed)){control->canceled.store(true,std::memory_order_relaxed);return;}std::this_thread::sleep_for(std::chrono::milliseconds(50));}}){}
    ~CancellationMonitor(){finished.store(true,std::memory_order_relaxed);thread.join();}
};
bool write(const QString &path,const QByteArray &bytes,QString &error){QSaveFile file(path);if(!file.open(QIODevice::WriteOnly) || file.write(bytes)!=bytes.size() || !file.commit()){error=QObject::tr("Nie udało się zapisać: ")+path;return false;}return true;}
QString csv(QString value){value.replace('"',"\"\"");return '"'+value+'"';}
QString number(const QJsonValue &value){return value.isDouble()?QString::number(value.toDouble(),'g',16):QString();}
}
bool exportReport(const QJsonObject &report,const QString &directory,QString &error){QDir dir(directory);if(!dir.mkpath(".")){error=QObject::tr("Nie można utworzyć folderu raportu.");return false;}const QStringList files{"experiment.json","report.json","athletes.csv","focus-samples.csv","comparisons.csv","README.txt"};for(const auto &file:files)if(dir.exists(file)){error=QObject::tr("Folder zawiera już raport. Wybierz nowy folder: ")+file;return false;}
    QString athletes="variant,athlete,name,starts,jumps,mean_place,wins,podiums,advanced,mean_total,mean_distance,mean_jump_points,falls,dsq\n",samples="variant,competition,place,total,gate,round,distance,points,wind,compensation,judges,takeoff,flight,random,fall,dsq\n",comparisons="variant,paired_place_n,paired_place_delta,paired_place_ci_low,paired_place_ci_high,paired_points_delta,paired_distance_delta\n";
    for(const auto &entry:report["variants"].toArray()){const auto variant=entry.toObject();const auto name=csv(variant["name"].toString());for(const auto &row:variant["athletes"].toArray()){const auto a=row.toObject();athletes+=QStringList{name,number(a["index"]),csv(a["name"].toString()),number(a["starts"]),number(a["jumps"]),number(a["place"].toObject()["mean"]),number(a["wins"]),number(a["podiums"]),number(a["advanced"]),number(a["total"].toObject()["mean"]),number(a["distance"].toObject()["mean"]),number(a["points"].toObject()["mean"]),number(a["falls"]),number(a["dsq"])}.join(',')+'\n';}
        for(const auto &row:variant["samples"].toArray()){const auto sample=row.toObject();for(const auto &attempt:sample["jumps"].toArray()){const auto jump=attempt.toObject();samples+=QStringList{name,number(sample["competition"]),number(sample["place"]),number(sample["points"]),number(sample["gate"]),number(jump["round"]),number(jump["distance"]),number(jump["points"]),number(jump["wind"]),number(jump["compensation"]),number(jump["judges"]),number(jump["takeoff"]),number(jump["flight"]),number(jump["random"]),jump["fall"].toBool()?"1":"0",jump["dsq"].toBool()?"1":"0"}.join(',')+'\n';}}
        const auto paired=variant["pairedPlace"].toObject();comparisons+=QStringList{name,number(paired["count"]),number(paired["mean"]),number(paired["ciLow"]),number(paired["ciHigh"]),number(variant["pairedPoints"].toObject()["mean"]),number(variant["pairedDistance"].toObject()["mean"])}.join(',')+'\n';
    }
    const QByteArray readme="Sj.Sim simulation playground\nexperiment.json: complete input for CLI/UI. report.json: all aggregate distributions, focus samples and paired comparisons.\nCSV uses UTF-8, comma separators, decimal dots and quoted text. Competition and athlete indices are zero-based; round numbers start at 1.\nPlace 0 in samples means unclassified; the last histogram bucket is unclassified. Team competitions report the athlete's team place and team total.\nDistance/points distributions exclude DSQ jumps. Place means exclude unclassified starts; counts remain available.\nPaired effects are variant minus baseline. Positive place delta means worse placing. 95% intervals use a normal approximation across independent competitions; jump means use competition-cluster standard errors. Histogram quantiles are approximated by bin midpoints.\nSamples contain the first configured number of competitions, not a random sample. Full macro statistics use every completed competition.\nExact numeric replay requires the same engine/build. Correlation is descriptive; use controlled variants for parameter effects.\nCanceled runs include only competitions completed in every variant. No game data or global settings are modified.\n";
    return write(dir.filePath(files[0]),QJsonDocument(report["config"].toObject()).toJson(QJsonDocument::Indented),error) && write(dir.filePath(files[2]),athletes.toUtf8(),error) && write(dir.filePath(files[3]),samples.toUtf8(),error) && write(dir.filePath(files[4]),comparisons.toUtf8(),error) && write(dir.filePath(files[5]),readme,error) && write(dir.filePath(files[1]),QJsonDocument(report).toJson(QJsonDocument::Compact),error);
}
int cli(int argc,char **argv){QCoreApplication app(argc,argv);app.setOrganizationName("SJ-Sim");app.setApplicationName("SJ-Sim-Playground");const auto args=app.arguments();QTextStream out(stdout),err(stderr);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    out.setEncoding(QStringConverter::Utf8);err.setEncoding(QStringConverter::Utf8);
#else
    out.setCodec("UTF-8");err.setCodec("UTF-8");
#endif
    const auto argument=[&](const QString &name){const int i=args.indexOf(name);return i>=0 && i+1<args.size()?args[i+1]:QString();};QString error;
    try{
        if(args.contains("--playground-example")){const auto path=argument("--playground-example");if(path.isEmpty()){err<<"Usage: sj-sim --playground-example experiment.json\n";return 2;}if(QFile::exists(path)){err<<"Output already exists\n";return 2;}if(!write(path,QJsonDocument(example()).toJson(QJsonDocument::Indented),error)){err<<error<<'\n';return 2;}out<<path<<'\n';return 0;}
        QFile file(argument("--config"));if(!file.open(QIODevice::ReadOnly)){err<<"Usage: sj-sim --playground --config experiment.json --output new-report-directory [--threads N] [--replay variant:competition]\n";return 2;}QJsonParseError parse;const auto document=QJsonDocument::fromJson(file.readAll(),&parse);if(parse.error!=QJsonParseError::NoError || !document.isObject())throw std::invalid_argument("Invalid experiment JSON");auto config=document.object();if(args.contains("--threads")){bool ok=false;const int threads=argument("--threads").toInt(&ok);if(!ok)throw std::invalid_argument("Invalid thread count");config["threads"]=threads;}if(!validate(config,error))throw std::invalid_argument(error.toStdString());
        if(args.contains("--replay")){const auto parts=argument("--replay").split(':');bool a=false,b=false;const int variant=parts.value(0).toInt(&a),competition=parts.value(1).toInt(&b);if(parts.size()!=2 || !a || !b)throw std::invalid_argument("Replay must be variant:competition (zero-based)");const auto result=replay(config,variant,competition);const auto output=argument("--output");if(output.isEmpty())out<<QJsonDocument(result).toJson(QJsonDocument::Indented);else {if(QFile::exists(output))throw std::invalid_argument("Replay output already exists");if(!write(output,QJsonDocument(result).toJson(QJsonDocument::Indented),error))throw std::runtime_error(error.toStdString());}return 0;}
        const auto output=argument("--output");if(output.isEmpty())throw std::invalid_argument("An output directory is required");err<<"Running "<<config["competitions"].toInt()<<" paired competitions across "<<config["variants"].toArray().size()<<" variants\n";err.flush();interrupted.store(false,std::memory_order_relaxed);std::signal(SIGINT,interruptedSignal);std::signal(SIGTERM,interruptedSignal);const auto control=std::make_shared<Control>();QJsonObject result;{CancellationMonitor monitor(control);result=run(config,control);}if(!exportReport(result,output,error))throw std::runtime_error(error.toStdString());out<<QJsonDocument(QJsonObject{{"completed",result["completed"]},{"canceled",result["canceled"]},{"elapsedMs",result["elapsedMs"]},{"threadsUsed",result["threadsUsed"]},{"output",QDir(output).absolutePath()}}).toJson(QJsonDocument::Compact)<<'\n';return result["canceled"].toBool()?130:0;
    }catch(const std::exception &exception){err<<exception.what()<<'\n';return 2;}
}
}
