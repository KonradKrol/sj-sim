#pragma once

static void rosterInputRegressions(MainWindow &main) {
    main.show();main.resize(1024,720);
    QInputDialog dialog(&main);dialog.setWindowTitle("Symulacja kilku skoków");dialog.setLabelText("Liczba kolejnych skoków");
    dialog.setInputMode(QInputDialog::IntInput);dialog.setIntRange(1,10000);dialog.setIntValue(5);
    auto *source=dialog.findChild<QSpinBox *>();check(source,"numeric input dialog has its source spin box");source->setObjectName("regressionJumpCount");
    dialog.show();QTest::qWait(220);dialog.activateWindow();scene(&dialog)->setFocus();
    auto *view=scene(&dialog);clickControl(view,"input-regressionJumpCount");
    QTest::keyClick(view,Qt::Key_A,Qt::ControlModifier);
    QString draft;
    for(char digit:QByteArray("123")) {
        QTest::keyClicks(view,QString(QChar(digit)));draft+=QChar(digit);QTest::qWait(260);
        check(item(view->rootObject(),"input-regressionJumpCount")->property("text").toString()==draft,"numeric dialog preserves typed digits across repeated backend refreshes");
    }
    clickControl(view,"numberIncrease-regressionJumpCount");QTest::qWait(160);
    check(source->value()==124,"numeric step first commits the typed draft then increments it");
    clickControl(view,"numberDecrease-regressionJumpCount");QTest::qWait(160);
    check(source->value()==123,"numeric decrement remains usable after typing and periodic refreshes");
    dialog.reject();QTest::qWait(80);
    QDialog help(&main);auto *layout=new QVBoxLayout(&help);auto *sourceButton=new QPushButton("Skok",&help);sourceButton->setObjectName("regressionHint");sourceButton->setToolTip("Naciśnij spację, aby oddać skok");layout->addWidget(sourceButton);help.show();QTest::qWait(180);
    auto *helpView=scene(&help);clickControl(helpView,"action-regressionHint");auto *button=item(helpView->rootObject(),"action-regressionHint");button->forceActiveFocus();QTest::keyClick(helpView,Qt::Key_Space);QTest::qWait(750);
    check(!button->property("hintVisible").toBool(),"repeated clicking and Space do not display an intrusive jump tooltip");
    QTest::keyClick(helpView,Qt::Key_F1);QTest::qWait(50);check(button->property("hintVisible").toBool(),"F1 still exposes the requested keyboard help");help.reject();QTest::qWait(80);
    qInfo()<<"PASS: numeric dialog typing, refresh stability and stepping";
}
