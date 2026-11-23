#include "CountriesController.h"

#include "../../global/CountryCodes.h"
#include "../../global/GlobalDatabase.h"
#include "../../global/AppPaths.h"
#include <QDesktopServices>
#include <QCollator>
#include <QDir>
#include <QFileInfo>
#include <QRegularExpression>
#include <QUrl>
#include <algorithm>

CountriesController::CountriesController(QObject *parent) : QObject(parent)
{
    m_draft=GlobalDatabase::get()->getCountriesReference();
    QCollator collator;collator.setCaseSensitivity(Qt::CaseInsensitive);
    std::stable_sort(m_draft.begin(),m_draft.end(),[&](const Country &a,const Country &b){return collator.compare(a.getName(),b.getName())<0;});
    for(const auto &jumper:GlobalDatabase::get()->getEditableGlobalJumpers())++m_athletes[jumper.getCountryCode().toUpper()];
    for(const auto &hill:GlobalDatabase::get()->getEditableGlobalHills())++m_hills[hill.getCountryCode().toUpper()];
    if(!m_draft.isEmpty())m_selected=0;
}

QString CountriesController::flagUrl(const QString &code)
{
    return AppPaths::flagUrl(code);
}

bool CountriesController::flagExists(const QString &code)
{
    return code.size()==3 && QFileInfo::exists(AppPaths::flagFile(code));
}

QString CountriesController::flagsFolder() const
{
    return QDir::toNativeSeparators(AppPaths::userFlagsFolder());
}

void CountriesController::openFlagsFolder()
{
    QDir().mkpath(AppPaths::userFlagsFolder());
    QDesktopServices::openUrl(QUrl::fromLocalFile(AppPaths::userFlagsFolder()));
}

int CountriesController::usage(const QString &code,bool hills) const
{
    return (hills?m_hills:m_athletes).value(code.toUpper());
}

QVariantMap CountriesController::row(int index) const
{
    const Country &country=m_draft.at(index);
    const int athletes=usage(country.getAlpha3(),false),hills=usage(country.getAlpha3(),true);
    QString used;
    if(athletes && hills)used=tr("%1 zaw. · %2 skoczni").arg(athletes).arg(hills);
    else if(athletes)used=tr("%1 zaw.").arg(athletes);
    else if(hills)used=tr("%1 skoczni").arg(hills);
    else used=tr("nieużywany");
    return {{"index",index},{"name",country.getName()},{"code",country.getAlpha3()},{"iso",country.getAlpha2()},
            {"flag",flagUrl(country.getAlpha3())},{"hasFlag",flagExists(country.getAlpha3())},
            {"flagFileName",AppPaths::flagFileName(country.getAlpha3())},
            {"ownFlag",QFileInfo(AppPaths::flagFile(country.getAlpha3())).absolutePath()==QDir(AppPaths::userFlagsFolder()).absolutePath()},
            {"athletes",athletes},{"hills",hills},{"usage",used},{"removable",athletes==0 && hills==0}};
}

QVariantList CountriesController::countries() const
{
    QVariantList rows;
    const QString needle=m_search.trimmed();
    for(int i=0;i<m_draft.size();++i){
        const Country &country=m_draft.at(i);
        if(!needle.isEmpty() && !country.getName().contains(needle,Qt::CaseInsensitive)
           && !country.getAlpha3().contains(needle,Qt::CaseInsensitive) && !country.getAlpha2().contains(needle,Qt::CaseInsensitive))continue;
        auto value=row(i);value["selected"]=i==m_selected;rows.append(value);
    }
    return rows;
}

void CountriesController::setSearch(const QString &value)
{
    if(value==m_search)return;
    m_search=value;emit changed();
}

QVariantMap CountriesController::current() const
{
    return m_selected>=0 && m_selected<m_draft.size()?row(m_selected):QVariantMap();
}

void CountriesController::select(int index)
{
    if(index<0 || index>=m_draft.size() || index==m_selected)return;
    m_selected=index;emit changed();
}

QString CountriesController::rename(int index,const QString &name)
{
    if(index<0 || index>=m_draft.size())return {};
    const QString clean=name.simplified();
    if(clean.isEmpty())return tr("Nazwa nie może być pusta.");
    if(clean==m_draft[index].getName())return {};
    m_draft[index].setName(clean);m_dirty=true;emit changed();
    return {};
}

QVariantMap CountriesController::checkNew(const QString &name,const QString &code,const QString &iso) const
{
    const QString cleanName=name.simplified(),cleanCode=code.trimmed().toUpper(),cleanIso=iso.trimmed().toUpper();
    QVariantMap problems;
    if(cleanName.isEmpty())problems["name"]=tr("Wpisz nazwę, np. Polska.");
    static const QRegularExpression three(QStringLiteral("^[A-Z]{3}$")),two(QStringLiteral("^[A-Z]{2}$"));
    if(!three.match(cleanCode).hasMatch())problems["code"]=tr("Wpisz 3 litery, np. POL.");
    else for(const auto &existing:m_draft)if(existing.getAlpha3().compare(cleanCode,Qt::CaseInsensitive)==0){
        problems["code"]=tr("Ten kod ma już kraj „%1”.").arg(existing.getName());break;
    }
    if(!two.match(cleanIso).hasMatch())problems["iso"]=tr("Wpisz 2 litery, np. PL.");
    const bool hasFlag=flagExists(cleanCode);
    return {{"problems",problems},{"ready",problems.isEmpty()},{"name",cleanName},{"code",cleanCode},{"iso",cleanIso},
            {"flag",cleanCode.size()==3?flagUrl(cleanCode):QString()},{"hasFlag",hasFlag},
            {"flagFileName",cleanCode.size()==3?AppPaths::flagFileName(cleanCode):QString()}};
}

bool CountriesController::add(const QString &name,const QString &code,const QString &iso)
{
    const auto check=checkNew(name,code,iso);
    if(!check.value("ready").toBool())return false;
    m_draft.append(Country(check.value("name").toString(),check.value("iso").toString(),check.value("code").toString()));
    m_selected=m_draft.size()-1;m_search.clear();m_dirty=true;emit changed();
    return true;
}

bool CountriesController::removeSelected()
{
    if(m_selected<0 || m_selected>=m_draft.size() || !row(m_selected).value("removable").toBool())return false;
    m_draft.remove(m_selected);
    m_selected=m_draft.isEmpty()?-1:std::min(m_selected,int(m_draft.size())-1);
    m_dirty=true;emit changed();
    return true;
}

void CountriesController::save()
{
    GlobalDatabase::get()->getCountriesReference()=m_draft;
    m_dirty=false;
    emit finished(true);
}

void CountriesController::cancel()
{
    emit finished(false);
}
