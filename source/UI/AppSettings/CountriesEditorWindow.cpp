#include "CountriesEditorWindow.h"
#include "ui_CountriesEditorWindow.h"
#include "../../global/GlobalDatabase.h"

#include <QCollator>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QSortFilterProxyModel>
#include <algorithm>

CountriesEditorWindow::CountriesEditorWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CountriesEditorWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("Kraje"));
    ui->label_title->setText(tr("Kraje"));

    // Work on a copy sorted by name; nothing reaches the database before "Zapisz".
    draft=GlobalDatabase::get()->getCountriesReference();
    QCollator collator;collator.setCaseSensitivity(Qt::CaseInsensitive);
    std::stable_sort(draft.begin(),draft.end(),[&](const Country &a,const Country &b){return collator.compare(a.getName(),b.getName())<0;});
    model=new CountriesEditorTableModel(&draft,this);
    proxy=new QSortFilterProxyModel(this);
    proxy->setSourceModel(model);proxy->setFilterKeyColumn(-1);proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);

    auto *help=new QLabel(tr("Nazwę zmienisz, klikając ją dwukrotnie. Kod decyduje o fladze, dlatego po dodaniu kraju się nie zmienia."),this);
    help->setObjectName("countriesHelp");help->setWordWrap(true);
    search=new QLineEdit(this);search->setObjectName("countriesSearch");
    search->setPlaceholderText(tr("Szukaj kraju lub kodu"));search->setClearButtonEnabled(true);search->setAccessibleName(tr("Szukaj kraju"));
    connect(search,&QLineEdit::textChanged,proxy,&QSortFilterProxyModel::setFilterFixedString);
    ui->verticalLayout->insertWidget(1,help);
    ui->verticalLayout->insertWidget(2,search);

    ui->tableView->setModel(proxy);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(1,QHeaderView::ResizeToContents);

    auto *actions=new QHBoxLayout;
    auto *add=new QPushButton(tr("Dodaj kraj…"),this);add->setObjectName("countriesAdd");
    removeButton=new QPushButton(tr("Usuń kraj"),this);removeButton->setObjectName("countriesRemove");removeButton->setEnabled(false);
    actions->addWidget(add);actions->addWidget(removeButton);actions->addStretch();
    ui->verticalLayout->insertLayout(4,actions);
    connect(add,&QPushButton::clicked,this,&CountriesEditorWindow::addCountry);
    connect(removeButton,&QPushButton::clicked,this,&CountriesEditorWindow::removeSelectedCountry);
    connect(ui->tableView->selectionModel(),&QItemSelectionModel::selectionChanged,this,[this]{removeButton->setEnabled(selectedRow()>=0);});

    ui->pushButton_submit->setText(tr("Zapisz"));
    auto *cancel=new QPushButton(tr("Anuluj"),this);cancel->setObjectName("countriesCancel");
    connect(cancel,&QPushButton::clicked,this,&QDialog::reject);
    if(auto *row=ui->pushButton_submit->parentWidget()->findChild<QHBoxLayout *>("horizontalLayout_OKButton"))row->insertWidget(row->indexOf(ui->pushButton_submit),cancel);
    else ui->verticalLayout->addWidget(cancel);
}

CountriesEditorWindow::~CountriesEditorWindow()
{
    delete ui;
}

void CountriesEditorWindow::fill()
{
    // Kept for callers of the previous editor; the list is ready after construction.
}

int CountriesEditorWindow::selectedRow() const
{
    const auto rows=ui->tableView->selectionModel()?ui->tableView->selectionModel()->selectedRows():QModelIndexList();
    return rows.isEmpty()?-1:proxy->mapToSource(rows.first()).row();
}

void CountriesEditorWindow::addCountry()
{
    QDialog form(this);form.setObjectName("addCountryDialog");form.setWindowTitle(tr("Dodaj kraj"));
    auto *layout=new QFormLayout(&form);
    auto *name=new QLineEdit(&form);name->setObjectName("newCountryName");
    auto *code=new QLineEdit(&form);code->setObjectName("newCountryCode");code->setMaxLength(3);code->setPlaceholderText(QStringLiteral("POL"));
    auto *iso=new QLineEdit(&form);iso->setObjectName("newCountryIso");iso->setMaxLength(2);iso->setPlaceholderText(QStringLiteral("PL"));
    code->setValidator(new QRegularExpressionValidator(QRegularExpression(QStringLiteral("[A-Za-z]{0,3}")),code));
    iso->setValidator(new QRegularExpressionValidator(QRegularExpression(QStringLiteral("[A-Za-z]{0,2}")),iso));
    layout->addRow(tr("Nazwa"),name);
    layout->addRow(tr("Kod (3 litery)"),code);
    layout->addRow(tr("Kod ISO (2 litery)"),iso);
    auto *hint=new QLabel(tr("Kod z 3 liter pojawia się przy zawodnikach i wybiera flagę. Kod ISO służy do flag na Discordzie."),&form);hint->setWordWrap(true);
    layout->addRow(hint);
    auto *buttons=new QDialogButtonBox(&form);
    buttons->addButton(tr("Dodaj"),QDialogButtonBox::AcceptRole);buttons->addButton(tr("Anuluj"),QDialogButtonBox::RejectRole);
    layout->addRow(buttons);
    connect(buttons,&QDialogButtonBox::rejected,&form,&QDialog::reject);
    connect(buttons,&QDialogButtonBox::accepted,&form,[&]{
        const QString countryName=name->text().simplified(),countryCode=code->text().toUpper(),isoCode=iso->text().toUpper();
        QString problem;
        if(countryName.isEmpty())problem=tr("Wpisz nazwę kraju.");
        else if(countryCode.size()!=3)problem=tr("Kod musi mieć 3 litery, np. POL.");
        else if(isoCode.size()!=2)problem=tr("Kod ISO musi mieć 2 litery, np. PL.");
        else for(const auto &existing:draft)if(existing.getAlpha3().compare(countryCode,Qt::CaseInsensitive)==0){problem=tr("Kraj z kodem %1 już istnieje: %2.").arg(countryCode,existing.getName());break;}
        if(!problem.isEmpty()){QMessageBox::warning(&form,tr("Dodaj kraj"),problem);return;}
        form.accept();
    });
    if(form.exec()!=QDialog::Accepted)return;
    model->addCountry(Country(name->text().simplified(),iso->text().toUpper(),code->text().toUpper()));
    search->clear();
    const QModelIndex added=proxy->mapFromSource(model->index(model->rowCount()-1,0));
    ui->tableView->selectRow(added.row());ui->tableView->scrollTo(added);
}

void CountriesEditorWindow::removeSelectedCountry()
{
    const int row=selectedRow();
    if(row<0)return;
    const Country country=draft.at(row);
    int athletes=0,hills=0;
    for(const auto &jumper:GlobalDatabase::get()->getEditableGlobalJumpers())if(jumper.getCountryCode().compare(country.getAlpha3(),Qt::CaseInsensitive)==0)++athletes;
    for(const auto &hill:GlobalDatabase::get()->getEditableGlobalHills())if(hill.getCountryCode().compare(country.getAlpha3(),Qt::CaseInsensitive)==0)++hills;
    if(athletes || hills){
        QMessageBox::information(this,tr("Nie można usunąć kraju"),
            tr("Kraju %1 używają zawodnicy (%2) lub skocznie (%3). Zmień im kraj, zanim go usuniesz.").arg(country.getName()).arg(athletes).arg(hills));
        return;
    }
    if(QMessageBox::question(this,tr("Usuń kraj"),tr("Usunąć kraj %1 (%2)? Zmiana zapisze się po kliknięciu „Zapisz”.").arg(country.getName(),country.getAlpha3()),
                             QMessageBox::Yes|QMessageBox::Cancel,QMessageBox::Cancel)!=QMessageBox::Yes)return;
    model->removeCountry(row);
}

void CountriesEditorWindow::on_pushButton_submit_clicked()
{
    GlobalDatabase::get()->getCountriesReference()=draft;
    accept();
}
