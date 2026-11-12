#include <QApplication>
#include <QElapsedTimer>
#include <QFile>
#include <QJsonDocument>
#include <iostream>
#include "../source/seasons/SimulationSave.h"
int main(int argc,char**argv){QApplication app(argc,argv);for(int n=0;n<3;++n){QElapsedTimer t;t.start();QFile f(argv[1]);f.open(QFile::ReadOnly);auto doc=QJsonDocument::fromJson(f.readAll());auto parse=t.nsecsElapsed();std::unique_ptr<SimulationSave> save(SimulationSave::getFromJson(doc.object()["simulation-save"].toObject(),nullptr));auto end=t.nsecsElapsed();if(!save)return 2;std::cout<<"parse_ms="<<parse/1e6<<" restore_ms="<<(end-parse)/1e6<<" total_ms="<<end/1e6<<std::endl;}}
