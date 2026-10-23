#pragma once
#include <QFormLayout>
#include <QSet>

static void uniqueUiProjection(QuickNode *root,const QString &screen)
{
    QSet<QuickNode *> seen;
    std::function<void(QuickNode *)> visit=[&](QuickNode *node){
        if(!node)return;
        const auto values=node->values();
        check(!seen.contains(node),screen+": one projection per source: "+values.value("name").toString()+" "+values.value("text").toString());
        seen.insert(node);
        for(const auto &child:node->children())visit(child.value<QuickNode *>());
        for(const auto &key:{"editor","list","toolbar"})visit(values.value(key).value<QuickNode *>());
        for(const auto &child:values.value("actionsRow").toList())visit(child.value<QuickNode *>());
    };
    visit(root);
}

static void uiDuplicationRegressions(MainWindow &main)
{
    main.show();main.resize(1024,720);
    {
        QDialog fixture(&main);auto *form=new QFormLayout(&fixture);
        auto *checkBox=new QCheckBox("Full-width setting");checkBox->setObjectName("spanningCheck");form->addRow(checkBox);
        auto *buttons=new QHBoxLayout;
        auto *button=new QPushButton("Full-width action");button->setObjectName("spanningAction");buttons->addWidget(button);form->addRow(buttons);
        auto *hint=new QLabel("Full-width description");hint->setObjectName("spanningHint");form->addRow(hint);
        auto *input=new QLineEdit;input->setObjectName("labelledInput");form->addRow("Labelled field",input);
        auto *unlabelled=new QLineEdit;unlabelled->setObjectName("unlabelledInput");form->setWidget(form->rowCount(),QFormLayout::FieldRole,unlabelled);
        fixture.show();QTest::qWait(120);auto *bridge=fixture.findChild<QuickBridge *>();check(bridge,"form projection bridge");
        uniqueUiProjection(bridge->root(),"form spanning rows");
        const auto checkVisual=[&](const QString &name){
            int count=0;std::function<void(QQuickItem *)> visit=[&](QQuickItem *item){if(item->objectName()==name)++count;for(auto *child:item->childItems())visit(child);};
            visit(scene(&fixture)->rootObject());check(count==1,"one rendered control: "+name);
        };
        for(const auto &name:{"check-spanningCheck","action-spanningAction","input-labelledInput","input-unlabelledInput"})checkVisual(name);
        fixture.reject();
    }
    {
        Classification definition;QDialog fixture(&main);auto *layout=new QVBoxLayout(&fixture);
        auto *editor=new ClassificationEditorWidget(&definition);layout->addWidget(editor);fixture.show();QTest::qWait(120);
        auto *bridge=fixture.findChild<QuickBridge *>();uniqueUiProjection(bridge->root(),"classification editor");
        editor->findChild<QCheckBox *>("classificationAdvanced")->setChecked(true);QTest::qWait(80);bridge->refresh();
        uniqueUiProjection(bridge->root(),"classification advanced filters");
        scene(&fixture)->grabFramebuffer().save(output+"/deduplicated-classification.png");fixture.reject();
    }
    {
        QDialog fixture(&main);auto *layout=new QVBoxLayout(&fixture);auto *editor=new CompetitionRulesEditorWidget;layout->addWidget(editor);
        fixture.show();QTest::qWait(120);auto *bridge=fixture.findChild<QuickBridge *>();uniqueUiProjection(bridge->root(),"competition rules");
        scene(&fixture)->grabFramebuffer().save(output+"/deduplicated-rules.png");fixture.reject();
    }
    {
        DatabaseEditorWindow database(&main);database.show();QTest::qWait(120);auto *bridge=database.findChild<QuickBridge *>();
        auto *tabs=database.findChild<QTabWidget *>("tabWidget_main");check(tabs,"database tabs");
        const QStringList editSlots{"onJumpersListViewDoubleClicked","onHillsListViewDoubleClicked","onCompetitionRulesListViewDoubleClicked","onPointsForPlacesPresetsListViewDoubleClicked"};
        for(int index=0;index<tabs->count();++index){
            tabs->setCurrentIndex(index);bridge->refresh();QTest::qWait(80);
            auto *list=tabs->currentWidget()->findChild<DatabaseItemsListView *>();check(list,"database list");
            if(index<editSlots.size() && list->getListModel()->rowCount()>0){
                QMetaObject::invokeMethod(&database,editSlots[index].toLatin1().constData(),Qt::DirectConnection,Q_ARG(QModelIndex,list->getListModel()->index(0,0)));
                bridge->refresh();QTest::qWait(80);
            }
            uniqueUiProjection(bridge->root(),QString("database tab %1").arg(index));
        }
        database.hide();
    }
    {
        CompetitionConfigWindow configuration(CompetitionConfigWindow::SingleCompetition,&main);configuration.show();QTest::qWait(120);
        auto *bridge=configuration.findChild<QuickBridge *>();
        const int count=bridge->root()->values().value("options").toStringList().size();
        for(int index=0;index<count;++index){
            bridge->edit(bridge->root()->values().value("id").toInt(),index);QTest::qWait(80);bridge->refresh();
            uniqueUiProjection(bridge->root(),QString("competition tab %1").arg(index));
        }
        configuration.reject();
    }
    qInfo()<<"PASS: unique form rows, classification filters, competition rules, database catalogs and competition pages";
}
