#include <QApplication>
#include <QDirIterator>
#include <QImageReader>
#include <QDebug>
#include <cstdlib>
static QString asset;static QStringList invalid;
static void message(QtMsgType,const QMessageLogContext &,const QString &text){if(text.contains("Invalid path data")){invalid.append(asset);fprintf(stderr,"INVALID SVG: %s\n",qPrintable(asset));}}
int main(int argc,char **argv){QApplication app(argc,argv);if(argc!=2)return 2;const auto previous=qInstallMessageHandler(message);
 for(const auto &folder:{"/source/UI/Quick/icons","/flags"}){QDirIterator it(QString(argv[1])+folder,{"*.svg"},QDir::Files,QDirIterator::Subdirectories);while(it.hasNext()){asset=it.next();QImageReader reader(asset);reader.read();}}
 qInstallMessageHandler(previous);if(!invalid.isEmpty())return 1;qInfo()<<"PASS SVG paths parse on the deployed Qt version";
}
