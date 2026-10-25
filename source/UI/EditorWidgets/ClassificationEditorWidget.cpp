#include "ClassificationEditorWidget.h"
#include "ui_ClassificationEditorWidget.h"

#include "../../competitions/CompetitionRules.h"
#include "../../global/GlobalDatabase.h"
#include <QFormLayout>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QSignalBlocker>
#include <QMessageBox>
#include <QStandardItemModel>

ClassificationEditorWidget::ClassificationEditorWidget(Classification * classification, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClassificationEditorWidget),
    classification(classification)
{
    ui->setupUi(this);

    if(classification != nullptr){
        pointsForPlaces = this->classification->getPointsForPlacesReference();
        altPointsForPlaces = this->classification->getAltPointsForPlacesReference();
    }

    tableModel = new PointsForPlacesTableModel(this);
    tableModel->setPointsForPlaces(&pointsForPlaces);
    ui->tableView_pointsForPlaces->setModel(tableModel);
    ui->tableView_pointsForPlaces->setSelectionMode(QAbstractItemView::SingleSelection);

    altTableModel = new PointsForPlacesTableModel(this);
    altTableModel->setPointsForPlaces(&altPointsForPlaces);
    ui->tableView_altPointsForPlaces->setModel(altTableModel);
    ui->tableView_altPointsForPlaces->setSelectionMode(QAbstractItemView::SingleSelection);

    action_add = new QAction(this);
    this->addAction(action_add);
    connect(action_add, &QAction::triggered, this, &ClassificationEditorWidget::addActionTriggered);

    action_remove = new QAction(this);
    this->addAction(action_remove);
    connect(action_remove, &QAction::triggered, this, &ClassificationEditorWidget::removeActionTriggered);

    connect(ui->comboBox_points, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](){
        if(ui->comboBox_points->currentIndex() == Classification::PointsForPlaces)
        {
            ui->label_shortcuts->hide();

            ui->tableView_pointsForPlaces->show();
            ui->label_preset->show();
            ui->comboBox_presets->show();

            ui->tableView_altPointsForPlaces->show();
            ui->label_altPreset->show();
            ui->comboBox_altPreset->show();
        }
        else if(ui->comboBox_points->currentIndex() == Classification::CompetitionPoints)
        {
            ui->label_shortcuts->hide();

            ui->tableView_pointsForPlaces->hide();
            ui->label_preset->hide();
            ui->comboBox_presets->hide();

            ui->tableView_altPointsForPlaces->hide();
            ui->label_altPreset->hide();
            ui->comboBox_altPreset->hide();
        }
    });

    ui->comboBox_presets->addItem(tr("Własne"));
    ui->comboBox_altPreset->addItem(tr("Własne"));
    for(auto & preset : GlobalDatabase::get()->getEditableGlobalPointsForPlacesPresets()){
        ui->comboBox_presets->addItem(preset.getName());
        ui->comboBox_altPreset->addItem(preset.getName());
    }
    connect(ui->comboBox_presets, qOverload<int>(&QComboBox::activated), this, [this](){
        const int index=ui->comboBox_presets->currentIndex()-1;const auto &presets=GlobalDatabase::get()->getEditableGlobalPointsForPlacesPresets();
        tableModel->replacePoints(index>=0&&index<presets.size()?presets[index].getPointsForPlaces():QMap<int,double>());
    });
    ui->comboBox_points->setItemText(0,tr("Klasyczne punkty za miejsca"));
    ui->comboBox_points->setItemText(1,tr("Naliczaj notę z konkursu"));
    for(auto *choice:{ui->comboBox_presets,ui->comboBox_altPreset})if(auto *model=qobject_cast<QStandardItemModel *>(choice->model()))if(auto *item=model->item(0))item->setEnabled(false);
    setupScoringControls();
    if(classification)fillInputs();
    else resetInputs();
    const auto refreshPresets=[this]{
        const auto &presets=GlobalDatabase::get()->getEditableGlobalPointsForPlacesPresets();
        const auto refresh=[&](QComboBox *choice,const QMap<int,double> &table){int match=0;const int selected=choice->currentIndex()-1;if(!table.isEmpty()){if(selected>=0 && selected<presets.size() && presets[selected].getPointsForPlaces()==table)match=selected+1;else for(int i=0;i<presets.size();++i)if(presets[i].getPointsForPlaces()==table){match=i+1;break;}}const QSignalBlocker block(choice);choice->setCurrentIndex(match);};
        refresh(ui->comboBox_presets,pointsForPlaces);refresh(ui->comboBox_altPreset,altPointsForPlaces);
    };
    for(auto *model:{tableModel,altTableModel}){connect(model,&QAbstractItemModel::dataChanged,this,refreshPresets);connect(model,&QAbstractItemModel::modelReset,this,refreshPresets);connect(model,&QAbstractItemModel::rowsInserted,this,refreshPresets);connect(model,&QAbstractItemModel::rowsRemoved,this,refreshPresets);}
    refreshPresets();

}

ClassificationEditorWidget::~ClassificationEditorWidget()
{
    delete ui;
}

void ClassificationEditorWidget::resetInputs()
{
    loadingInputs=true;
    ui->lineEdit_name->clear();ui->comboBox_type->setCurrentIndex(0);
    baseType=0;basePoints.clear();baseAlternative.clear();options=ClassificationOptions();currentMethod=0;
    refreshMethodChoices();fillOptions();loadMethod();loadingInputs=false;
}

void ClassificationEditorWidget::fillInputs()
{
    if(!classification)return;
    loadingInputs=true;
    ui->lineEdit_name->setText(classification->getName());ui->comboBox_type->setCurrentIndex(classification->getClassificationType());
    baseType=classification->getPunctationType();basePoints=classification->getPointsForPlaces();baseAlternative=classification->getAltPointsForPlaces();
    options=classification->getOptions();currentMethod=0;
    refreshMethodChoices();fillOptions();loadMethod();loadingInputs=false;
}

Classification *ClassificationEditorWidget::getClassificationFromInputs()
{
    storeMethod();
    auto *result=new Classification(ui->lineEdit_name->text());
    result->setClassificationType(ui->comboBox_type->currentIndex());result->setPunctationType(baseType);
    result->setPointsForPlaces(basePoints);result->setAltPointsForPlaces(baseAlternative);
    options.ageFilter=ageFilter->isChecked();options.minimumAge=minimumAge->value();options.maximumAge=maximumAge->value();
    options.referenceYear=referenceYear->value();options.includeUnknownAge=includeUnknownAge->isChecked();
    options.countryExclude=countryExclude->isChecked();options.countrySelection=countrySelection->currentIndex();options.countryCount=countryCount->value();
    options.contextParameters=contextParameters->isChecked();result->setOptions(options);
    return result;
}

void ClassificationEditorWidget::on_pushButton_submit_clicked()
{
    emit submitted();
}

void ClassificationEditorWidget::addActionTriggered()
{
    int count = pointsForPlaces.count();
    tableModel->insertRow(count);
    emit tableModel->dataChanged(tableModel->index(count, 0), tableModel->index(count, 1));
}

void ClassificationEditorWidget::removeActionTriggered()
{
    int count = pointsForPlaces.count();
    tableModel->removeRow(count - 1);
}

Classification *ClassificationEditorWidget::getClassification() const
{
    return classification;
}

void ClassificationEditorWidget::setClassification(Classification *newClassification)
{
    classification = newClassification;
    tableModel->replacePoints(classification?classification->getPointsForPlaces():QMap<int,double>());
    altTableModel->replacePoints(classification?classification->getAltPointsForPlaces():QMap<int,double>());
}

void ClassificationEditorWidget::on_comboBox_altPreset_activated(int index)
{
    const int position=index-1;const auto &presets=GlobalDatabase::get()->getEditableGlobalPointsForPlacesPresets();
    altTableModel->replacePoints(position>=0&&position<presets.size()?presets[position].getPointsForPlaces():QMap<int,double>());
}

void ClassificationEditorWidget::setupScoringControls()
{
    ui->comboBox_type->setToolTip(QString());ui->comboBox_points->setToolTip(QString());
    auto *methods=new QWidget(this);auto *form=new QFormLayout(methods);
    methodChoice=new QComboBox(methods);methodChoice->setObjectName("classificationScoringMethod");
    methodName=new QLineEdit(methods);methodName->setObjectName("classificationMethodName");
    form->addRow(tr("Sposób naliczania punktów"),methodChoice);form->addRow(tr("Nazwa sposobu"),methodName);
    auto *buttons=new QHBoxLayout;
    auto *add=new QPushButton(tr("Dodaj sposób (kopia bieżącego)"),methods);add->setObjectName("classificationAddMethod");
    removeMethod=new QPushButton(tr("Usuń sposób"),methods);removeMethod->setObjectName("classificationRemoveMethod");
    buttons->addWidget(add);buttons->addWidget(removeMethod);form->addRow(buttons);
    auto *methodHint=new QLabel(tr("Każdy konkurs wybiera jeden sposób osobno dla każdej klasyfikacji. Tabele mogą mieć dowolną liczbę miejsc i własne wartości punktów."),methods);
    methodHint->setWordWrap(true);form->addRow(methodHint);ui->verticalLayout_2->insertWidget(2,methods);
    connect(methodChoice,qOverload<int>(&QComboBox::currentIndexChanged),this,[this](int index){
        if(loadingInputs || index<0)return;storeMethod();currentMethod=index;loadMethod();
    });
    connect(methodName,&QLineEdit::editingFinished,this,[this]{if(!loadingInputs){storeMethod();emit configurationChanged();}});
    connect(add,&QPushButton::clicked,this,[this]{
        storeMethod();ClassificationScoringMethod method;method.name=tr("Sposób %1").arg(options.methods.size()+1);
        method.type=ui->comboBox_points->currentIndex();method.points=pointsForPlaces;method.alternativePoints=altPointsForPlaces;
        options.methods.append(method);currentMethod=options.methods.size();refreshMethodChoices();loadMethod();emit configurationChanged();
    });
    connect(removeMethod,&QPushButton::clicked,this,[this]{
        if(currentMethod<1 || currentMethod>options.methods.size())return;
        options.methods.removeAt(currentMethod-1);currentMethod=0;refreshMethodChoices();loadMethod();emit configurationChanged();
    });
    ui->label_shortcuts->hide();
    auto rowButtons=[this](bool alternative){
        auto *panel=new QWidget(this);auto *row=new QHBoxLayout(panel);
        auto *add=new QPushButton(tr("Dodaj punktowane miejsce"),panel);
        auto *remove=new QPushButton(tr("Usuń ostatnie miejsce"),panel);
        add->setObjectName(alternative?"classificationAltAddPlace":"classificationAddPlace");
        remove->setObjectName(alternative?"classificationAltRemovePlace":"classificationRemovePlace");
        row->addWidget(add);row->addWidget(remove);
        auto *model=alternative?altTableModel:tableModel;
        connect(add,&QPushButton::clicked,this,[model]{model->insertRow(model->rowCount());});
        connect(remove,&QPushButton::clicked,this,[model]{if(model->rowCount()>0)model->removeRow(model->rowCount()-1);});
        auto *table=alternative?ui->tableView_altPointsForPlaces:ui->tableView_pointsForPlaces;
        ui->verticalLayout->insertWidget(ui->verticalLayout->indexOf(table),panel);
    };
    rowButtons(false);rowButtons(true);
    auto *question=new QPushButton("?",this);question->setObjectName("classificationAlternativeHelp");question->setAccessibleName(tr("Pomoc: źródła punktów"));
    ui->horizontalLayout_6->addWidget(question);
    connect(question,&QPushButton::clicked,this,[this,question]{QMessageBox::information(this,tr("Źródła punktów"),question->toolTip());});
    connect(ui->comboBox_type,qOverload<int>(&QComboBox::currentIndexChanged),this,[this]{refreshAlternativeLabel();});
    auto *advancedToggle=new QCheckBox(tr("Filtry zaawansowane"),this);advancedToggle->setObjectName("classificationAdvanced");
    auto *advanced=new QWidget(this);advanced->setObjectName("classificationAdvancedPanel");auto *fields=new QFormLayout(advanced);
    ageFilter=new QCheckBox(tr("Włącz filtr wieku"),advanced);ageFilter->setObjectName("classificationAgeFilter");fields->addRow(ageFilter);
    auto spin=[&](const char *name,int min,int max,const QString &label){auto *input=new QSpinBox(advanced);input->setObjectName(name);input->setRange(min,max);fields->addRow(label,input);return input;};
    minimumAge=spin("classificationMinimumAge",1,100,tr("Minimalny wiek (włącznie)"));
    maximumAge=spin("classificationMaximumAge",1,100,tr("Maksymalny wiek (włącznie)"));
    referenceYear=new QSpinBox(advanced);referenceYear->setObjectName("classificationReferenceYear");referenceYear->setRange(1,9999);referenceYear->hide();
    includeUnknownAge=new QCheckBox(tr("Uwzględniaj skoczków o nieznanym wieku"),advanced);includeUnknownAge->setObjectName("classificationUnknownAge");fields->addRow(includeUnknownAge);
    auto *ageHint=new QLabel(tr("Wiek rocznikowy jest stały przez sezon, według roku jego rozpoczęcia. W konkursie drużynowym cały skład musi spełniać filtr wieku, aby drużyna zdobyła punkty za swoje miejsce."),advanced);ageHint->setWordWrap(true);fields->addRow(ageHint);
    countrySelection=new QComboBox(advanced);countrySelection->setObjectName("classificationCountrySelection");countrySelection->addItems({tr("Wszyscy"),tr("N najlepszych z każdego kraju"),tr("N najgorszych z każdego kraju")});fields->addRow(tr("Wybór skoczków z kraju"),countrySelection);
    countryExclude=new QCheckBox(tr("Wyklucz wybranych skoczków"),advanced);countryExclude->setObjectName("classificationCountryExclude");fields->addRow(countryExclude);
    countryCount=spin("classificationCountryCount",1,10000,tr("Liczba N"));
    auto *countryHint=new QLabel(tr("Tylko klasyfikacja drużynowa i tylko konkursy indywidualne: uwzględnij albo wyklucz wybrane N wyników kraju osobno w każdym konkursie. Nie dotyczy konkursów drużynowych."),advanced);countryHint->setWordWrap(true);fields->addRow(countryHint);
    contextParameters=new QCheckBox(tr("Pozwól zmieniać mnożnik punktów w konkursach"),advanced);contextParameters->setObjectName("classificationContextParameters");fields->addRow(contextParameters);
    auto *contextHint=new QLabel(tr("Konkurs może mnożyć naliczane punkty, np. przez 0,5 lub 2. Domyślny mnożnik to 1."),advanced);contextHint->setWordWrap(true);fields->addRow(contextHint);
    ui->verticalLayout_2->insertWidget(ui->verticalLayout_2->count()-1,advancedToggle);
    ui->verticalLayout_2->insertWidget(ui->verticalLayout_2->count()-1,advanced);advanced->hide();
    connect(advancedToggle,&QCheckBox::toggled,advanced,&QWidget::setVisible);
    for(auto *input:{minimumAge,maximumAge,referenceYear,countryCount})connect(input,qOverload<int>(&QSpinBox::valueChanged),this,[this]{if(!loadingInputs)emit configurationChanged();});
    for(auto *check:{ageFilter,includeUnknownAge,contextParameters,countryExclude})connect(check,&QCheckBox::toggled,this,[this]{if(!loadingInputs)emit configurationChanged();});
    connect(minimumAge,qOverload<int>(&QSpinBox::valueChanged),maximumAge,&QSpinBox::setMinimum);
    refreshAlternativeLabel();
}

void ClassificationEditorWidget::refreshAlternativeLabel()
{
    ui->label_altPreset->setText(ui->comboBox_type->currentIndex()==Classification::Individual
        ?tr("Punkty zawodnika za miejsce jego drużyny w konkursie drużynowym")
        :tr("Punkty drużyny (kraju) z indywidualnych wyników jej skoczków"));
    ui->label_altPreset->setWordWrap(true);
    const auto help=ui->comboBox_type->currentIndex()==Classification::Individual
        ?tr("Aby zawodnik otrzymywał punkty na swoje konto za miejsce swojej drużyny, wybierz „Konkurs drużynowy” i ustaw tę punktację. W „Konkurs indywidualny” ustawisz punkty za jego własne miejsce.")
        :tr("Aby drużyna (kraj) otrzymywała punkty za miejsca swoich skoczków, np. jak w Pucharze Narodów, wybierz „Konkurs indywidualny” i ustaw tę punktację. W „Konkurs drużynowy” ustawisz punkty za miejsce drużyny.");
    if(auto *question=findChild<QPushButton *>("classificationAlternativeHelp"))question->setToolTip(help);
    ui->tableView_altPointsForPlaces->setToolTip(help);
}
void ClassificationEditorWidget::storeMethod()
{
    if(loadingInputs)return;
    if(currentMethod==0){basePoints=pointsForPlaces;baseAlternative=altPointsForPlaces;baseType=ui->comboBox_points->currentIndex();}
    else if(currentMethod<=options.methods.size()){
        auto &m=options.methods[currentMethod-1];m.points=pointsForPlaces;m.alternativePoints=altPointsForPlaces;m.type=ui->comboBox_points->currentIndex();
        if(!methodName->text().trimmed().isEmpty())m.name=methodName->text().trimmed();
        methodChoice->setItemText(currentMethod,m.name);
    }
}
void ClassificationEditorWidget::loadMethod()
{
    const bool previous=loadingInputs;loadingInputs=true;
    const bool custom=currentMethod>0 && currentMethod<=options.methods.size();
    methodName->setReadOnly(!custom);removeMethod->setEnabled(custom && !property("quickScoringLocked").toBool());
    methodName->setText(custom?options.methods[currentMethod-1].name:tr("Domyślny"));
    ui->comboBox_points->setCurrentIndex(custom?options.methods[currentMethod-1].type:baseType);
    tableModel->replacePoints(custom?options.methods[currentMethod-1].points:basePoints);
    altTableModel->replacePoints(custom?options.methods[currentMethod-1].alternativePoints:baseAlternative);
    // Apply visibility also when the selected type has not emitted a change.
    const bool places=ui->comboBox_points->currentIndex()==Classification::PointsForPlaces;
    for(QWidget *w:QVector<QWidget *>{ui->tableView_pointsForPlaces,ui->tableView_altPointsForPlaces,ui->label_preset,ui->comboBox_presets,ui->label_altPreset,ui->comboBox_altPreset})w->setVisible(places);
    loadingInputs=previous;
}
void ClassificationEditorWidget::refreshMethodChoices()
{
    const QSignalBlocker blocker(methodChoice);methodChoice->clear();methodChoice->addItem(tr("Domyślny"));
    for(const auto &m:options.methods)methodChoice->addItem(m.name);
    methodChoice->setCurrentIndex(currentMethod);
}
void ClassificationEditorWidget::fillOptions()
{
    ageFilter->setChecked(options.ageFilter);minimumAge->setValue(options.minimumAge);maximumAge->setValue(options.maximumAge);
    referenceYear->setValue(options.referenceYear);includeUnknownAge->setChecked(options.includeUnknownAge);
    countryExclude->setChecked(options.countryExclude);countrySelection->setCurrentIndex(options.countrySelection);countryCount->setValue(options.countryCount);contextParameters->setChecked(options.contextParameters);
    findChild<QCheckBox *>("classificationAdvanced")->setChecked(options.ageFilter || options.countrySelection!=0 || options.contextParameters);
}
