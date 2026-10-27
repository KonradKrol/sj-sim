#include "GlobalTranslators.h"

#include <QMessageBox>
#include <QApplication>
#include <QLibraryInfo>
#include <QLocale>
#include <QWidget>
#include <QDir>
#include "GlobalAppSettings.h"

GlobalTranslators* GlobalTranslators::m_globalTranslators = nullptr;

GlobalTranslators::GlobalTranslators()
{
    configTranslations();
}

GlobalTranslators *GlobalTranslators::get()
{
    if(m_globalTranslators == nullptr)
        m_globalTranslators = new GlobalTranslators;

    return m_globalTranslators;
}

void GlobalTranslators::configTranslations()
{
    globalTranslations.push_back(new QTranslator);
    globalTranslations.push_back(new QTranslator);
}

bool GlobalTranslators::setLanguage(QApplication *application, int language)
{
    if (!application || language < GlobalAppSettings::Polish || language > GlobalAppSettings::Czech)
        return false;
    if (language == activeLanguage)
        return true;

    QTranslator *next = language == GlobalAppSettings::Polish ? nullptr : globalTranslations.at(language - 1);
    const QString code = language == GlobalAppSettings::English ? QStringLiteral("en") : QStringLiteral("cs");
    const QString translationDirectory = QDir(application->applicationDirPath()).filePath(QStringLiteral("translations"));
    if (next && next->isEmpty() && !next->load(QStringLiteral("translation_%1.qm").arg(code), translationDirectory))
        return false;

    const QLocale locale(language == GlobalAppSettings::English ? "en_GB" : language == GlobalAppSettings::Czech ? "cs_CZ" : "pl_PL");
    QLocale::setDefault(locale);
    for (QWidget *window : application->topLevelWidgets())
        window->setLocale(locale);

    for (QTranslator *translator : globalTranslations)
        application->removeTranslator(translator);
    application->removeTranslator(&qtTranslation);
    if (language != GlobalAppSettings::English) {
        const QString qtCode = language == GlobalAppSettings::Polish ? QStringLiteral("pl") : QStringLiteral("cs");
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        const QString qtDirectory = QLibraryInfo::path(QLibraryInfo::TranslationsPath);
#else
        const QString qtDirectory = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
#endif
        if (qtTranslation.load(QStringLiteral("qtbase_") + qtCode, translationDirectory) ||
            qtTranslation.load(QStringLiteral("qtbase_") + qtCode, qtDirectory))
            application->installTranslator(&qtTranslation);
    }
    if (next)
        application->installTranslator(next);
    activeLanguage = language;
    return true;
}

QVector<QTranslator *> GlobalTranslators::getGlobalTranslations() const
{
    return globalTranslations;
}

void GlobalTranslators::setGlobalTranslations(const QVector<QTranslator *> &newGlobalTranslations)
{
    globalTranslations = newGlobalTranslations;
}
