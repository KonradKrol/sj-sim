#pragma once
#include <QDialog>
#include <QVariantList>
#include <QSet>

class ResourceLibraryWindow : public QDialog {
    Q_OBJECT
    Q_PROPERTY(QVariantList resources READ resources NOTIFY changed)
    Q_PROPERTY(QString dataPath READ dataPath CONSTANT)
    Q_PROPERTY(QString message READ message NOTIFY changed)
    Q_PROPERTY(int selectedCount READ selectedCount NOTIFY changed)
public:
    explicit ResourceLibraryWindow(QWidget *parent=nullptr);
    static bool installPackage(QWidget *owner,const QString &file,QStringList &installed,QString &error);
    QVariantList resources() const;
    QString dataPath() const;
    QString message() const{return m_message;}
    int selectedCount()const{return m_selected.size();}
    Q_INVOKABLE void toggle(const QString &key);
    Q_INVOKABLE void clearSelection();
    Q_INVOKABLE void exportSelected();
    Q_INVOKABLE void removeSelected();
    Q_INVOKABLE void importArchive();
    Q_INVOKABLE bool importFile(const QString &path);
    Q_INVOKABLE void openFolder();
    Q_INVOKABLE void chooseFolder();
signals:
    void changed();
    void libraryChanged();
private:
    QVariantList m_resources;
    QSet<QString> m_selected;
    QString m_message;
    void refresh();
};
