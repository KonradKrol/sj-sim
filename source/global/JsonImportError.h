#pragma once
#include <QFileInfo>
#include <QJsonParseError>
#include <QObject>

namespace JsonImportError {
inline QString file(const QString &path, const QString &reason) {
    return QObject::tr("Nie można wczytać pliku „%1”.\n%2\nWybierz poprawną paczkę wyeksportowaną z Sj.Sim.")
        .arg(QFileInfo(path).fileName(), reason);
}
inline QString syntax(const QByteArray &bytes, const QJsonParseError &error) {
    const auto prefix = bytes.left(error.offset);
    const int line = prefix.count('\n') + 1;
    const int column = QString::fromUtf8(prefix.mid(prefix.lastIndexOf('\n') + 1)).size() + 1;
    return QObject::tr("Nieprawidłowy JSON: wiersz %1, kolumna %2.\n%3")
        .arg(line).arg(column).arg(error.errorString());
}
}
