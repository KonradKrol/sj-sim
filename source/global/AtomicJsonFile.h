#pragma once
#include <QJsonDocument>
#include <QMessageBox>
#include <QSaveFile>
#include <QDir>
#include <QFileInfo>
#include <QRegularExpression>

namespace AtomicJsonFile {
inline QString namedPath(const QString &directory,const QString &name,bool notify=false) {
    const auto reject=[&] {
        if(notify)QMessageBox::warning(nullptr,QObject::tr("Nieprawidłowa nazwa presetu"),QObject::tr("Nazwa presetu zawiera niedozwolone znaki lub wskazuje skrót do innego pliku. Wybierz inną nazwę."));
        return QString();
    };
    if(name.trimmed().isEmpty() || name=="." || name==".." || name.contains(QRegularExpression("[\\\\/:*?\"<>|\\x00-\\x1f]")))return reject();
    const QFileInfo target(QDir(directory).absoluteFilePath(name+".json"));
    return target.isSymLink()?reject():target.absoluteFilePath();
}

inline bool write(const QString &path,const QJsonDocument &document,bool notify=true) {
    QSaveFile file(path);
    file.setDirectWriteFallback(false);
    const auto bytes=document.toJson(QJsonDocument::Indented);
    if(file.open(QIODevice::WriteOnly) && file.write(bytes)==bytes.size() && file.commit())return true;
    file.cancelWriting();
    if(notify)QMessageBox::critical(nullptr,QObject::tr("Błąd zapisu danych"),
        QObject::tr("Nie udało się bezpiecznie zapisać pliku %1. Poprzedni plik pozostał bez zmian.").arg(path));
    return false;
}
}
