#pragma once
#include "AthleteProfile.h"
#include <QObject>
#include <QVariantMap>
#include <QVariantList>
#include <QVector>
#include <QJsonArray>

class AthleteStudioController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList packs READ packs NOTIFY uiChanged)
    Q_PROPERTY(QVariantList profiles READ profiles NOTIFY uiChanged)
    Q_PROPERTY(QVariantMap profile READ profile NOTIFY uiChanged)
    Q_PROPERTY(QVariantList selectedPerks READ selectedPerks NOTIFY perksChanged)
    Q_PROPERTY(QVariantList countries READ countries CONSTANT)
    Q_PROPERTY(QVariantList legacyPacks READ legacyPacks CONSTANT)
    Q_PROPERTY(QString packName READ packName NOTIFY uiChanged)
    Q_PROPERTY(QString packDirectory READ packDirectory NOTIFY uiChanged)
    Q_PROPERTY(int packIndex READ packIndex NOTIFY uiChanged)
    Q_PROPERTY(int selectedIndex READ selectedIndex NOTIFY uiChanged)
    Q_PROPERTY(QString message READ message NOTIFY statusChanged)
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY historyChanged)
    Q_PROPERTY(bool canRedo READ canRedo NOTIFY historyChanged)
    QString m_root,m_message,m_lastPhoto;
    mutable QVariantList m_countryOptions;
    QVector<QJsonObject> m_packs;
    QVector<QJsonObject> m_undo,m_redo;
    int m_pack=0,m_selected=-1;
    bool m_changePending=false;
    void notifyChanged();
    bool commit(const QJsonObject &next,int selection,const QString &message={},bool perksOnly=false);
    bool persist(const QJsonObject &pack,QString &error) const;
    QString directory(const QJsonObject &pack) const;
    void status(const QString &message);
    QJsonObject currentPack() const;
    bool readPack(const QString &file,QJsonObject &pack,QString &error) const;
    QString resolvedPhotoPath() const;
public:
    explicit AthleteStudioController(QObject *parent=nullptr,const QString &root={});
    QVariantList packs() const;
    QVariantList profiles() const;
    QVariantMap profile() const;
    QVariantList selectedPerks() const;
    QVariantList countries() const;
    QVariantList legacyPacks() const;
    Q_INVOKABLE QVariantList legacyAthletes(int pack) const;
    QString packName() const;
    QString packDirectory() const{return directory(currentPack());}
    Q_INVOKABLE void openPackFolder();
    int packIndex() const{return m_pack;}
    int selectedIndex() const{return m_selected;}
    QString message() const{return m_message;}
    bool canUndo() const{return !m_undo.isEmpty();}
    bool canRedo() const{return !m_redo.isEmpty();}
    Q_INVOKABLE void selectPack(int index);
    void invalidateText(){notifyChanged();}
    Q_INVOKABLE void renamePack(const QString &name);
    Q_INVOKABLE void createPack();
    Q_INVOKABLE void select(int index);
    Q_INVOKABLE void add();
    Q_INVOKABLE void duplicate();
    Q_INVOKABLE void remove();
    Q_INVOKABLE void setValue(const QString &key,const QVariant &value);
    Q_INVOKABLE void setPerk(int pair,int side,int level,bool toggle=true);
    Q_INVOKABLE void undo();
    Q_INVOKABLE void redo();
    Q_INVOKABLE void copyLegacy(int pack,int athlete);
    Q_INVOKABLE bool setPhoto(const QString &file);
    Q_INVOKABLE void choosePhoto();
    Q_INVOKABLE void clearPhoto();
    Q_INVOKABLE void exportPack();
    Q_INVOKABLE void importPack();
    bool exportTo(const QString &newDirectory);
    bool importFrom(const QString &file);
signals:
    void changed();
    void uiChanged();
    void perksChanged();
    void historyChanged();
    void statusChanged();
};
