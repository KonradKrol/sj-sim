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
    QString pageNote;  // "Miejsca 31–60 · obraz 2 z 4" when the results span several images
};

namespace ResultsImage {
ResultsCard fromCompetition(CompetitionInfo *competition, int limit=0);
ResultsCard fromClassification(Classification *classification, int limit=0);
QImage render(const ResultsCard &card, qreal pixelRatio=2.0);
// Splits long results into several cards with the same header.
QVector<ResultsCard> pages(const ResultsCard &card, int rowsPerPage);

// Asks how many places to include (10, 30 or all), then where to save. All places are split
// into images of 30 or 50 places (the player chooses); up to three are saved as separate PNG files, more as one ZIP archive.
// A single image is also copied to the clipboard. Returns false when cancelled or failed
// (the player is told why).
bool exportCompetition(QWidget *parent, CompetitionInfo *competition);
bool exportClassification(QWidget *parent, Classification *classification);

// The same images sent to the Discord webhook set in the settings. Sending is asynchronous and
// the player is told the result. When no webhook is set, explains where to set it.
bool publishCompetition(QWidget *parent, CompetitionInfo *competition);
bool publishClassification(QWidget *parent, Classification *classification);
}
