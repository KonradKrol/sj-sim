#pragma once

#include <QCoreApplication>
#include <QDir>
#include "UserDataPaths.h"
#include "CountryCodes.h"
#include <QFileInfo>
#include <QUrl>

namespace AppPaths {
// Read-only files shipped with the game: flags, translations and the Defaults library.
// A macOS application bundle keeps them in Contents/Resources (the executable lives in
// Contents/MacOS); Windows and Linux ship them beside the executable.
inline QString resources()
{
#ifdef Q_OS_MACOS
    return QDir(QCoreApplication::applicationDirPath() + QStringLiteral("/../Resources")).absolutePath();
#else
    return QCoreApplication::applicationDirPath();
#endif
}

// Player's own flags: <data library>/Flags/<ISO code>.svg or .png. They take precedence over
// the flags shipped with the game, which may be read-only (AppImage, macOS bundle).
inline QString userFlagsFolder()
{
    return QDir(UserDataPaths::root()).filePath(QStringLiteral("Flags"));
}

// File name expected for a country's flag, e.g. "POL.svg" (FIS codes are converted to ISO).
inline QString flagFileName(const QString &countryCode)
{
    return CountryCodes::toIso(countryCode).toUpper()+QStringLiteral(".svg");
}

// Absolute path of a country's flag: the player's own flag when present, otherwise the
// shipped one (which may not exist for unusual codes).
inline QString flagFile(const QString &countryCode)
{
    const QString iso=CountryCodes::toIso(countryCode).toUpper();
    const QDir own(userFlagsFolder());
    for(const char *extension:{".svg",".png"}){
        const QString candidate=own.filePath(iso+QLatin1String(extension));
        if(QFileInfo::exists(candidate))return candidate;
    }
    return QDir(QDir(resources()).filePath(QStringLiteral("flags"))).filePath(CountryCodes::toIso(countryCode)+QStringLiteral(".svg"));
}

inline QString flagUrl(const QString &countryCode)
{
    return QUrl::fromLocalFile(flagFile(countryCode)).toString();
}

// Folder for automatically exported result files given by a relative path such as
// "results/single-jumps". Windows and Linux keep the established location relative to the
// working directory. A macOS bundle must stay unmodified after signing, so there the files
// go to the player's data library instead.
inline QString output(const QString &relative)
{
#ifdef Q_OS_MACOS
    if(QDir::isRelativePath(relative))return QDir(UserDataPaths::root()).filePath(relative);
#endif
    return relative;
}
}
