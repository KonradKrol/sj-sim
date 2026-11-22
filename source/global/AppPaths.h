#pragma once

#include <QCoreApplication>
#include <QDir>
#include "UserDataPaths.h"

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
