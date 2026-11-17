#include "../source/seasons/SeasonCalendarPreset.h"
#include <QApplication>
#include <QTemporaryDir>
#include "../source/global/UserDataPaths.h"
#include "../source/global/ResourceBundle.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QDate>
#include <QSet>
#include <iostream>
int main(int argc,char **argv){
 QApplication app(argc,argv);
 QCoreApplication::setOrganizationName("SjSim-Historical-Calendar-Test");QCoreApplication::setApplicationName("Isolated");
 QTemporaryDir temporary;app.setProperty("sjSimDataRoot",temporary.path());app.setProperty("sjSimDefaultsRoot",QString::fromLocal8Bit(argv[1])+"/data/defaults");QString initError;if(!UserDataPaths::initialize(initError))return 8;
 QFile file(QString::fromLocal8Bit(argv[1])+"/data/community/historical-calendars/Kalendarze-FIS-2023-2025.sjsim-pack");
 if(!file.open(QIODevice::ReadOnly))return 1;
 const QByteArray magic("SJSIM-PACK\n1\n");auto bytes=file.readAll();if(!bytes.startsWith(magic))return 2;
 const auto bundle=QJsonDocument::fromJson(bytes.mid(magic.size())).object();int calendars=0;
 for(const auto &v:bundle["resources"].toArray()){
  const auto resource=v.toObject();if(resource["kind"].toString()!="Calendars")continue;
  const auto preset=resource["document"].toObject()["calendar-preset"].toObject();QString error;
  if(!SeasonCalendarPreset::validateJson(preset,error)){std::cerr<<error.toStdString();return 3;}
  const auto calendar=preset["calendar"].toObject();const auto events=calendar["competitions"].toArray();QDate previous;QSet<QString> earlier;
  for(const auto &entry:events){auto e=entry.toObject();const auto date=QDate::fromString(e["scheduled-date"].toString(),Qt::ISODate);
   if(!date.isValid() || (previous.isValid() && date<previous))return 4;
   previous=date;
   for(const auto &key:{"advancement-competition-id","trial-round-id"})if(e.contains(key) && !earlier.contains(e[key].toString()))return 5;
   for(auto id:e["training-ids"].toArray())if(!earlier.contains(id.toString()))return 6;
   earlier.insert(e["id"].toString());
  }
  ++calendars;std::cout<<preset["name"].toString().toStdString()<<": valid, chronological, resolved earlier links\n";
 }
 QStringList installed;QString error;
 if(!ResourceBundle::importFile(file.fileName(),installed,error)){std::cerr<<error.toStdString();return 9;}
 std::cout<<"Full bundle import into an isolated library: PASS ("<<installed.size()<<" resources)\n";
 return calendars==4?0:7;
}
