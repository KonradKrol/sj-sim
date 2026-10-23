#pragma once

static void navigationPerformance(MainWindow &main)
{
    main.show();main.resize(1240,790);QTest::qWait(150);
    QJsonArray measurements;
    const auto measure=[&](const QString &name,const std::function<std::unique_ptr<QDialog>()> &create){
        QElapsedTimer timer;timer.start();auto dialog=create();const auto construction=timer.elapsed();
        dialog->show();const auto presentation=timer.elapsed();QTest::qWait(20);
        auto *view=scene(dialog.get());check(view && view->status()==QQuickWidget::Ready,"navigation benchmark page loads");
        check(view->engine()==scene(&main)->engine(),"navigation screens share the QML component cache");
        check(qmlContext(view->rootObject())->contextProperty("ui").value<QuickBridge *>()==dialog->findChild<QuickBridge *>(),"each screen retains its own controller context");
        check(!view->grabFramebuffer().isNull(),"navigation benchmark produces its first frame");const auto frame=timer.elapsed();
        measurements.append(QJsonObject{{"screen",name},{"construction_ms",double(construction)},
            {"presentation_ms",double(presentation-construction)},{"ready_ms",double(frame)}});
        qInfo()<<"PASS: navigation benchmark"<<name<<"constructor"<<construction<<"presentation"<<presentation-construction<<"first frame"<<frame;
        dialog->hide();QTest::qWait(40);
    };
    for(int pass=0;pass<3;++pass){
        measure("competition",[&]{return std::make_unique<CompetitionConfigWindow>(CompetitionConfigWindow::SingleCompetition,&main);});
        measure("database",[&]{return std::make_unique<DatabaseEditorWindow>(&main);});
        measure("seasons",[&]{return std::make_unique<SimulationSavesWindow>(&main);});
        measure("single-jumps",[&]{return std::make_unique<SingleJumpsConfigWindow>(&main);});
    }
    QFile file(output+"/navigation-benchmark.json");check(file.open(QFile::WriteOnly) && file.write(QJsonDocument(measurements).toJson())>0,"write navigation measurements");
}
