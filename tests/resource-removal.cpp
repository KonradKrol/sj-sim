#include <QApplication>
#include <QTemporaryDir>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <cstdlib>
#include "../source/global/UserDataPaths.h"
#include "../source/global/ResourceBundle.h"
static void check(bool value,const QString &message){if(!value){qCritical().noquote()<<"FAIL:"<<message;std::exit(1);}}
int main(int argc,char **argv){
    QTemporaryDir temp;qputenv("XDG_DATA_HOME",(temp.path()+"/xdg").toUtf8());
    QApplication app(argc,argv);check(argc==2,"repository argument");
    app.setProperty("sjSimDataRoot",temp.path()+"/library");app.setProperty("sjSimDefaultsRoot",QString(argv[1])+"/data/defaults");
    QString error;check(UserDataPaths::initialize(error),error);
    QStringList removed;
    check(!ResourceBundle::removeSelection({"Points/../../outside.json"},removed,error) && removed.isEmpty(),"reject paths outside library");
    const auto folder=UserDataPaths::path("Packs/Points");const auto original=QDir(folder).entryList({"*.json"},QDir::Files).first();
    const auto copy=folder+"/removal-test.json";check(QFile::copy(folder+"/"+original,copy),"test copy created");
    check(!ResourceBundle::removeSelection({"Points/removal-test.json","Points/missing.json"},removed,error) && QFile::exists(copy),"validate entire selection before touching any files");
    check(ResourceBundle::removeSelection({"Points/removal-test.json","Points/removal-test.json"},removed,error),error);
    check(removed.size()==1 && !QFile::exists(copy) && QFile::exists(folder+"/"+original),"remove only the selected resource once");
    qInfo()<<"PASS library removal: validation, traversal protection, duplicate selection and recoverable deletion";
}
