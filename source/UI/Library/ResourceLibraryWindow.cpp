#include "ResourceLibraryWindow.h"
#include "../../global/ResourceBundle.h"
#include "../../global/UserDataPaths.h"
#include "../../global/GlobalDatabase.h"
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QSettings>
#include <QMessageBox>
#include <QDirIterator>
#include <QSaveFile>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include <QSignalBlocker>
#include <QTimer>

ResourceLibraryWindow::ResourceLibraryWindow(QWidget *parent):QDialog(parent){
    setWindowTitle(tr("Biblioteka"));setObjectName("resourceLibrary");resize(940,680);
    // A layout is also the navigation host contract used by QuickUi::attach.
    auto *layout=new QVBoxLayout(this);auto *actions=new QHBoxLayout;
    auto *importButton=new QPushButton(tr("Wczytaj paczkę"),this);auto *exportButton=new QPushButton(tr("Eksportuj"),this);
    auto *folderButton=new QPushButton(tr("Otwórz folder"),this);auto *changeButton=new QPushButton(tr("Zmień folder"),this);
    actions->addWidget(importButton);actions->addWidget(exportButton);actions->addStretch();actions->addWidget(folderButton);actions->addWidget(changeButton);layout->addLayout(actions);
    auto *list=new QListWidget(this);layout->addWidget(list,1);auto *message=new QLabel(this);message->setWordWrap(true);layout->addWidget(message);
    connect(importButton,&QPushButton::clicked,this,&ResourceLibraryWindow::importArchive);connect(exportButton,&QPushButton::clicked,this,&ResourceLibraryWindow::exportSelected);
    connect(folderButton,&QPushButton::clicked,this,&ResourceLibraryWindow::openFolder);connect(changeButton,&QPushButton::clicked,this,&ResourceLibraryWindow::chooseFolder);
    connect(list,&QListWidget::itemChanged,this,[this](QListWidgetItem *item){const auto key=item->data(Qt::UserRole).toString();QTimer::singleShot(0,this,[this,key]{toggle(key);});});
    connect(this,&ResourceLibraryWindow::changed,this,[this,list,message,exportButton]{
        const QSignalBlocker blocker(list);list->clear();for(const auto &entry:resources()){const auto row=entry.toMap();auto *item=new QListWidgetItem(row["name"].toString()+" — "+row["category"].toString(),list);item->setData(Qt::UserRole,row["key"]);item->setCheckState(row["selected"].toBool()?Qt::Checked:Qt::Unchecked);}
        exportButton->setEnabled(!m_selected.isEmpty());message->setText(m_message.isEmpty()?dataPath():m_message);
    });refresh();
}
QString ResourceLibraryWindow::dataPath()const{return QDir::toNativeSeparators(UserDataPaths::root());}
QVariantList ResourceLibraryWindow::resources()const{auto rows=m_resources;for(auto &row:rows){auto v=row.toMap();v["selected"]=m_selected.contains(v["key"].toString());row=v;}return rows;}
void ResourceLibraryWindow::refresh(){m_resources=ResourceBundle::entries();QSet<QString> known;for(const auto &row:m_resources)known.insert(row.toMap()["key"].toString());m_selected.intersect(known);emit changed();}
void ResourceLibraryWindow::toggle(const QString &key){if(m_selected.contains(key))m_selected.remove(key);else for(const auto &row:m_resources)if(row.toMap()["key"]==key){m_selected.insert(key);break;}emit changed();}
void ResourceLibraryWindow::clearSelection(){m_selected.clear();emit changed();}
void ResourceLibraryWindow::exportSelected(){
    const auto file=QFileDialog::getSaveFileName(this,tr("Udostępnij zasoby"),"SjSim.sjsim-pack",tr("Paczka Sj.Sim (*.sjsim-pack)"));if(file.isEmpty())return;
    QString error;if(ResourceBundle::exportSelection(file,m_selected.values(),error))m_message=tr("Zapisano paczkę ze zdjęciami i zależnościami kalendarzy.");else m_message=error;emit changed();
}
void ResourceLibraryWindow::importArchive(){const auto file=QFileDialog::getOpenFileName(this,tr("Wczytaj paczkę"),{},tr("Paczka Sj.Sim (*.sjsim-pack)"));if(!file.isEmpty())importFile(file);}
bool ResourceLibraryWindow::installPackage(QWidget *owner,const QString &file,QStringList &installed,QString &error){
    QWidget *database=nullptr;for(auto *widget=owner;widget;widget=widget->parentWidget())if(widget->inherits("DatabaseEditorWindow")){database=widget;break;}
    if(database){bool ready=false;QMetaObject::invokeMethod(database,"prepareResourceImport",Qt::DirectConnection,Q_RETURN_ARG(bool,ready));if(!ready){error=tr("Najpierw popraw edytowane dane.");return false;}}
    const bool imported=ResourceBundle::importFile(file,installed,error);
    if(database)QMetaObject::invokeMethod(database,"finishResourceImport",Qt::DirectConnection,Q_ARG(QStringList,installed));
    else if(imported){auto *db=GlobalDatabase::get();db->loadPacks();db->loadPointsForPlacesPresets();db->getEditableGlobalCalendarPresets().clear();db->loadCalendarPresets();db->setLoadedSimulationSaves(false);}
    return imported;
}
bool ResourceLibraryWindow::importFile(const QString &path){
    const auto file=path.startsWith("file:")?QUrl(path).toLocalFile():path;
    QStringList added;QString error;if(!installPackage(this,file,added,error)){m_message=error;emit changed();return false;}
    m_message=tr("Wczytano zasoby: %1. Istniejące pliki pozostały bez zmian.").arg(added.size());refresh();emit libraryChanged();return true;
}

void ResourceLibraryWindow::openFolder(){QDesktopServices::openUrl(QUrl::fromLocalFile(UserDataPaths::root()));}
void ResourceLibraryWindow::chooseFolder(){
    if(QFileInfo::exists(QCoreApplication::applicationDirPath()+"/portable.txt")){m_message=tr("Działa tryb przenośny. Usuń portable.txt obok aplikacji, aby wybrać inny folder przy następnym uruchomieniu.");emit changed();return;}
    const auto target=QFileDialog::getExistingDirectory(this,tr("Wybierz nowy folder danych"));if(target.isEmpty()||QDir(target).absolutePath()==QDir(UserDataPaths::root()).absolutePath())return;
    const auto root=QFileInfo(UserDataPaths::root()).canonicalFilePath();const auto canonical=QFileInfo(target).canonicalFilePath();
    if(canonical.startsWith(root+"/")||root.startsWith(canonical+"/")||!QDir(target).entryList(QDir::AllEntries|QDir::NoDotAndDotDot|QDir::Hidden).isEmpty()){m_message=tr("Wybierz pusty folder poza obecnym folderem danych.");emit changed();return;}
    QCoreApplication::instance()->setProperty("pendingDataDirectory",QDir(target).absolutePath());
    m_message=tr("Folder zostanie zmieniony po zamknięciu aplikacji. Bieżące dane zostaną skopiowane, a oryginały zachowane.");emit changed();
}
