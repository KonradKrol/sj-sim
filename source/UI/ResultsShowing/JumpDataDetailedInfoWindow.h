#ifndef JUMPDATADETAILEDINFOWINDOW_H
#define JUMPDATADETAILEDINFOWINDOW_H

#include <QWidget>
#include "../../simulator/JumpData.h"
#include "JumpWindInfoWidget.h"
#include <dpp/dpp.h>

namespace Ui {
class JumpDataDetailedInfoWindow;
}

class JumpDataDetailedInfoWindow : public QWidget
{
    Q_OBJECT

public:
    explicit JumpDataDetailedInfoWindow(JumpData * jumpData = nullptr, QWidget *parent = nullptr);
    ~JumpDataDetailedInfoWindow();

    void fillJumpInformations();
    void showOrHideByAppSettings();

    void removeJumperInfoTitle();
    void removeSimulationInformationsLayouts();
    void removeWindsInfoLayout();

    dpp::embed getEmbedForJumpInfo();

private:
    Ui::JumpDataDetailedInfoWindow *ui;

    JumpData * jumpData;
    JumpWindInfoWidget * windInfoWidget;
public:
    JumpData *getJumpData() const;
    void setJumpData(JumpData *newJumpData);
    Ui::JumpDataDetailedInfoWindow *getUi() const;

    JumpWindInfoWidget *getWindInfoWidget() const;

private slots:
    void on_pushButton_clicked();
};

#endif // JUMPDATADETAILEDINFOWINDOW_H
