#ifndef ROUNDINFOEDITORWIDGET_H
#define ROUNDINFOEDITORWIDGET_H

#include <QWidget>
#include "../../competitions/RoundInfo.h"

namespace Ui {
class RoundInfoEditorWidget;
}

class RoundInfoEditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RoundInfoEditorWidget(bool hideGroupsInfo = true, int competitiorsCount = 1, bool sortStartList = true, short sortGroups = 0, bool KO = false, int KOCount = 0, int KOAdvance = 0, int selection = 0, QWidget *parent = nullptr);
    ~RoundInfoEditorWidget();

    void fillRoundInfoInput();
    void resetRoundInfoInput();
    int getCountFromInput();
    bool getSortStartListFromInput();
    int getSortAfterGroupsFromInput();
    bool getKORoundFromInput();
    int getKOGroupCountFromInput();
    int getAdvancingFromKOGroup();
    int getKOGroupsSelectionTypeFromInputs();

    void hideGroupsInfo();
    void showGroupsInfo();
    //---------------------//
    void hideKOCheckBox();
    void showKOCheckBox();

    void setKO(bool ok);

private:
    Ui::RoundInfoEditorWidget *ui;

    RoundInfo * roundInfo;
public:
    RoundInfo *getRoundInfo() const;
    void setRoundInfo(RoundInfo *newRoundInfo);
private slots:
    void on_comboBox_KOGroupsSelectionType_activated(int index);
};

#endif // ROUNDINFOEDITORWIDGET_H
