#pragma once

#include <QImage>
#include <QString>
#include <QStringList>
#include <QVector>

class CompetitionInfo;
class Classification;
class QWidget;

// Shareable results card (PNG). The card is built from a plain data snapshot and drawn with
// QPainter, independently of the window size, theme or scroll position, so the same results
// always produce the same image on Windows, Linux and macOS.
struct ResultsCard
{
    struct Row
    {
        int position=0;
        QString country;   // FIS or ISO code used for the flag
        QString name;
        QString detail;    // country code, or team members
        QStringList values;
        QString total;
    };
    QString eyebrow;
    QString title;
    QString subtitle;
    QString titleCountry;
    QStringList valueColumns;
    QString totalColumn;
    QVector<Row> rows;
    int hiddenRows=0;
};

namespace ResultsImage {
ResultsCard fromCompetition(CompetitionInfo *competition, int limit=0);
ResultsCard fromClassification(Classification *classification, int limit=0);
QImage render(const ResultsCard &card, qreal pixelRatio=2.0);

// Asks how many places to include, then where to save the PNG. The saved image is also
// copied to the clipboard so it can be pasted straight into a chat. Returns false when
// cancelled or when saving failed (the player is told why).
bool exportCompetition(QWidget *parent, CompetitionInfo *competition);
bool exportClassification(QWidget *parent, Classification *classification);
}
