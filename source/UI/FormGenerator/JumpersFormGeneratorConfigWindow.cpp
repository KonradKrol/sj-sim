#include "JumpersFormGeneratorConfigWindow.h"
#include "ui_JumpersFormGeneratorConfigWindow.h"
#include "../../form-generator/JumperFormGenerator.h"
#include "../../global/GlobalDatabase.h"
#include "../../utilities/functions.h"
#include "../EditorWidgets/EditStartListWithJumpersListsWindow.h"
#include <QMessageBox>
#include <QInputDialog>

JumpersFormGeneratorConfigWindow::JumpersFormGeneratorConfigWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JumpersFormGeneratorConfigWindow)
{
    ui->setupUi(this);

    tableModel = new JumperFormGeneratorsSettingsTableModel(this);
    ui->tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    removeFromTableAction = new QAction(this);
    this->addAction(removeFromTableAction);
    removeFromTableAction->setShortcut(Qt::CTRL | Qt::Key_D);
    connect(removeFromTableAction, &QAction::triggered, this, [this](){
        onRemoveFromTable(false);
    });
    alternativeRemoveFromTableAction = new QAction(this);
    this->addAction(alternativeRemoveFromTableAction);
    alternativeRemoveFromTableAction->setShortcut(Qt::CTRL | Qt::ALT | Qt::Key_D);
    connect(alternativeRemoveFromTableAction, &QAction::triggered, this, [this](){
        onRemoveFromTable(true);
    });
}

JumpersFormGeneratorConfigWindow::~JumpersFormGeneratorConfigWindow()
{
    delete ui;
}

void JumpersFormGeneratorConfigWindow::updateTable()
{
    ui->tableView->setModel(nullptr);
    ui->tableView->setModel(tableModel);
    ui->tableView->resizeColumnsToContents();
}

JumperFormGeneratorSettings JumpersFormGeneratorConfigWindow::getSettingsFromInputs()
{
    JumperFormGeneratorSettings settings;
    settings.setFormVariability(ui->doubleSpinBox_formVariability->value());
    settings.setMinForm(ui->doubleSpinBox_minForm->value());
    settings.setMaxForm(ui->doubleSpinBox_maxForm->value());
    settings.setFormBonus(ui->doubleSpinBox_formBonus->value());
    return settings;
}

QAction *JumpersFormGeneratorConfigWindow::getRemoveFromTableAction() const
{
    return removeFromTableAction;
}

void JumpersFormGeneratorConfigWindow::setRemoveFromTableAction(QAction *newRemoveFromTableAction)
{
    removeFromTableAction = newRemoveFromTableAction;
}

SimulationSave *JumpersFormGeneratorConfigWindow::getSave() const
{
    return save;
}

void JumpersFormGeneratorConfigWindow::setSave(SimulationSave *newSave)
{
    save = newSave;
}

JumperFormGeneratorsSettingsTableModel *JumpersFormGeneratorConfigWindow::getTableModel() const
{
    return tableModel;
}

void JumpersFormGeneratorConfigWindow::on_doubleSpinBox_tendVariability_editingFinished()
{

}

void JumpersFormGeneratorConfigWindow::on_doubleSpinBox_formBonus_editingFinished()
{
    double value = ui->doubleSpinBox_formBonus->value();
    for(auto & settings : tableModel->getGeneratorsSettingsReference())
    {
        settings.setFormBonus(value);
    }
    updateTable();
}

void JumpersFormGeneratorConfigWindow::on_doubleSpinBox_minForm_editingFinished()
{
    double value = ui->doubleSpinBox_minForm->value();
    for(auto & settings : tableModel->getGeneratorsSettingsReference())
    {
        settings.setMinForm(value);
    }
    updateTable();
}

void JumpersFormGeneratorConfigWindow::on_doubleSpinBox_maxForm_editingFinished()
{
    double value = ui->doubleSpinBox_maxForm->value();
    for(auto & settings : tableModel->getGeneratorsSettingsReference())
    {
        settings.setMaxForm(value);
    }
    updateTable();
}

void JumpersFormGeneratorConfigWindow::on_listWidget_questions_doubleClicked(const QModelIndex &index)
{
    if(index.row() == 0)
    {
        QMessageBox::information(this, tr("Po co jest generator formy?"), tr(
        "Generator formy jest narzędziem służącym do pół-automatycznej zmiany formy zawodników pomiędzy okresem czasu (między zawodami, weekendami zawodów, sezonami).\nPosiada on wiele opcji, które służą do możliwie jak najwierniejszego odwzorowaniu zmiany formy prawdziwych skoczków między zawodami."
                                                                             ), QMessageBox::Ok);
    }
    else if(index.row() == 1)
    {
        QMessageBox::information(this, tr("Jak działa generator formy?"), tr(
        "Na podstawie wielu czynników generator losuje jak bardzo zmieni się forma zawodnika. Użytkownik musi wpisać jedynie jaką zmienność ma mieć generator i ewentualnie jakie bonusy mają dostać zawodnicy (lub w jakim zakresie ma zmieścić się ich forma), a resztę robi komputer. Każdy zawodnik ma swoją \"niestabilność formy\" która dodatkowo wpływa na ewentualną zmianę formy i która również jest automatycznie zmieniana. Oprócz tego istnieją inne mechanizmy generatora formy, np. takie które zapobiegają zbyt częstemu występowaniu skrajnych form zawodnika."), QMessageBox::Ok);
    }
    else if(index.row() == 2)
    {
        QMessageBox::information(this, tr("Do czego służą poszczególne parametry?"), tr(
        "Każdy parametr generatora formy jest wyjaśniony po najchaniu kursorem na jego nazwę."), QMessageBox::Ok);
    }
    else if(index.row() == 3)
    {
        //QMessageBox::information(this, tr("Jak ustalić formę na start nowego sezonu?"), tr(
                                // "Aby osiągnąć taki efekt, że zawodnicy na start następnego sezonu mają całkiem inną formę z ułamkiem formy z końcówki poprzedniego sezonu, należy wykonać 3 czynności z generatorem formy:\n1. Wygenerować nową tendencję i formę zawodników. Dzięki temu zawodnicy mają odświeżoną formę na nowy sezon. Trzeba ustawić dużą zmienność tendencji i dużą zmienność formy, a także powinno się ustawić wyrównanie tendencji powyżej 0 (aby forma zawodników wyrównała się do środka na początek sezonu). Zalecane jest też ustawienie minusowego bonusu formy w wysokości około -10, jeśli początek sezonu to \"okres przygotowawczy\" (Dzięki temu na start przygotowań nikt nie będzie miał bardzo wysokiej formy co nie było by do końca realistyczne).\n2. Ustalić każdemu zawodnikowi neutralną tendencję formy (Trzeba ustawić minimalną i maksymalną tendencję na 0. To samo robimy ze zmiennością tendencji i formy, też ustawiamy na 0). Resztę trzeba zostawić domyślnie. Robimy to po to, aby każdy zawodnik miał nową niezbyt mocną tendencję ponieważ przez dużą moc tendencji jaka byłaby przez punkt 1, byłyby nierealistycznie duże spadki i wzrosty formy.\n3. Ustalić każdemu zawodnikowi nową, niezbyt mocną tendencję poprzez ustawienie średniej zmienności tendencji i ZEROWEJ zmienności formy (formy zmieniać nie chcemy). Można również ustawić według uznania bonus tendencji jeżeli chce się aby zawodnikom na start sezonu forma raczej rosła niż spadała (można też zrobić na odwrót).\nPo wykonaniu tych 3 kroków, wszyscy zawodnicy są gotowi do ciekawej walki w następnym sezonie."), QMessageBox::Ok);
    }
}


void JumpersFormGeneratorConfigWindow::on_pushButton_saveAsPreset_clicked()
{
    bool ok = false;
    QString text = QInputDialog::getText(this, tr("Nazwa presetu"), tr("Podaj nazwę jaką ma mieć ten preset generatora formy. Jeżeli istnieje już preset o takiej nazwie, zostanie on nadpisany."), QLineEdit::Normal, "", &ok);
    if(ok == true)
    {
        bool textExsists = false;
        int index=0;
        for(auto & p : GlobalDatabase::get()->getEditableGlobalCalendarPresets())
        {
            if(p.getName() == text){
                textExsists = true;
                break;
            }
            index++;
        }
        if(textExsists == false)
            index = (-1);

        JumperFormGeneratorSettingsPreset * preset;
        if(index == -1){
            GlobalDatabase::get()->getEditableFormGeneratorPresets().push_back(getSettingsFromInputs());
            preset = &GlobalDatabase::get()->getEditableFormGeneratorPresets().last();
        }
        else{
            GlobalDatabase::get()->getEditableFormGeneratorPresets()[index]=JumperFormGeneratorSettingsPreset(getSettingsFromInputs());
            preset = &GlobalDatabase::get()->getEditableFormGeneratorPresets()[index];
        }
        preset->setName(text);
        GlobalDatabase::get()->writeFormGeneratorPresets();
    }
}


void JumpersFormGeneratorConfigWindow::on_pushButton_loadPreset_clicked()
{
    bool ok = false;
    QStringList presetsNames;
    for(auto & preset : GlobalDatabase::get()->getEditableFormGeneratorPresets())
        presetsNames.push_back(preset.getName());
    QString itemText = QInputDialog::getItem(this, tr("Wybierz preset generatora formy"), tr("Wybierz z listy preset generatora formy który chcesz wczytać"), presetsNames,0, false, &ok);
    if(ok == true)
    {
        JumperFormGeneratorSettingsPreset * preset = &GlobalDatabase::get()->getEditableFormGeneratorPresets()[presetsNames.indexOf(itemText)];
        for(auto & settings : tableModel->getGeneratorsSettingsReference())
        {
            Jumper * jumper = settings.getJumper();
            settings = preset->getSettings();
            settings.setJumper(jumper);
        }
        JumperFormGeneratorSettings * settings = &preset->getSettingsReference();
        ui->doubleSpinBox_formVariability->setValue(settings->getFormVariability());
        ui->doubleSpinBox_formBonus->setValue(settings->getFormBonus());
        ui->doubleSpinBox_minForm->setValue(settings->getMinForm());
        ui->doubleSpinBox_maxForm->setValue(settings->getMaxForm());
    }
}


void JumpersFormGeneratorConfigWindow::on_pushButton_generate_clicked()
{
    JumperFormGenerator generator;
    for(auto & jumperSettings : tableModel->getGeneratorsSettingsReference())
    {
        generator.setSettings(jumperSettings);
        generator.setInstability(save->getJumperFormInstability(jumperSettings.getJumper()));
        generator.generateJumperForm();
    }

    QMessageBox::information(this, tr("Generowanie formy"), tr("Pomyślnie zaktualizowano tendencję formy i formę zawodników"), QMessageBox::Ok);
}

void JumpersFormGeneratorConfigWindow::onRemoveFromTable(bool alternative)
{
    QSet<int> rows;
    for(auto & index : ui->tableView->selectionModel()->selectedIndexes())
        rows.insert(index.row());
    qDebug()<<"rows count: "<<rows.count();
    if(alternative && ui->tableView->selectionModel()->selectedRows().count() < tableModel->rowCount() && ui->tableView->selectionModel()->selectedIndexes().count() > 0)
    {
        qDebug()<<"alt";
        int count = tableModel->rowCount();
        QSet<int> tempRows;
        for(int i=0; i<count; i++)
        {
            qDebug()<<i<<" ("<<rows.contains(i)<<")";
            if(rows.contains(i) == false)
            {
                tempRows.insert(i);
            }
        }
        qDebug()<<"size "<<tempRows.size();
        rows = tempRows;
    }
    int removed = 0;
    QList<int> r = rows.values();
    std::sort(r.begin(), r.end(), std::less<int>());
    for(auto & row : qAsConst(r))
    {
        qDebug()<<"b << "<<row<<" (removed: "<<removed<<")";
        MyFunctions::removeFromVector(tableModel->getGeneratorsSettingsReference(), &tableModel->getGeneratorsSettingsReference()[row - removed]);
        removed++;
    }
    updateTable();
}


void JumpersFormGeneratorConfigWindow::on_doubleSpinBox_formVariability_editingFinished()
{
    double value = ui->doubleSpinBox_formVariability->value();
    for(auto & settings : tableModel->getGeneratorsSettingsReference())
    {
        settings.setFormVariability(value);
    }
    updateTable();
}

bool settingsVectorContainsByJumper(const QVector<JumperFormGeneratorSettings> & settings, Jumper * jumper)
{
    for(auto & s : settings)
    {
        if(s.getJumper() == jumper)
            return true;
    }
    return false;
}
void settingsVectorRemoveByJumper(QVector<JumperFormGeneratorSettings> & settings, Jumper * jumper)
{
    int i=0;
    for(auto & setts : settings)
    {
        if(setts.getJumper() == jumper)
        {
            settings.remove(i);
            return;
        }
        i++;
    }
}

void JumpersFormGeneratorConfigWindow::on_pushButton_jumpersLists_clicked()
{
    EditStartListWithJumpersListsWindow * window = new EditStartListWithJumpersListsWindow(this);
    window->setLists(&save->getJumpersListsReference());
    window->setupWidgets();
    if(window->exec() == QDialog::Accepted)
    {
        QSet<Jumper *> addedJumpers;
        QVector<JumperFormGeneratorSettings> settings = tableModel->getGeneratorsSettingsReference();
        settings.detach();
        for(auto & status : window->constructJumpersListsStatuses())
        {
            if(status.second == JumpersListsListItemWidget::Select)
            {
                for(auto & jumper : status.first->getJumpersReference())
                {
                    if(settingsVectorContainsByJumper(settings, jumper) == false)
                    {
                        JumperFormGeneratorSettings setts;
                        setts.setJumper(jumper);
                        settings.push_back(setts);
                    }
                    addedJumpers.insert(jumper);
                }
            }
            else if(status.second == JumpersListsListItemWidget::Unselect)
            {
                for(auto & jumper : status.first->getJumpersReference())
                {
                    if(settingsVectorContainsByJumper(settings, jumper) == true && addedJumpers.contains(jumper) == false)
                    {
                        settingsVectorRemoveByJumper(settings, jumper);
                    }
                }
            }
        }
        tableModel->setGeneratorsSettings(settings);
        updateTable();
    }
}

