#pragma once
#include "HillProfile.h"
#include <QObject>
#include <QVariantList>
#include <QVariantMap>
#include <QVector>

class HillStudioController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList packs READ packs NOTIFY changed)
    Q_PROPERTY(QVariantList profiles READ profiles NOTIFY changed)
    Q_PROPERTY(QVariantMap profile READ profile NOTIFY changed)
    Q_PROPERTY(QVariantList selectedPerks READ selectedPerks NOTIFY perksChanged)
    Q_PROPERTY(double realHs READ realHs NOTIFY perksChanged)
    Q_PROPERTY(QVariantList countries READ countries CONSTANT)
    Q_PROPERTY(QVariantList legacyHills READ legacyHills NOTIFY changed)
    Q_PROPERTY(int packIndex READ packIndex NOTIFY changed)
    Q_PROPERTY(int selectedIndex READ selectedIndex NOTIFY changed)
    Q_PROPERTY(QString packName READ packName NOTIFY changed)
    Q_PROPERTY(QString message READ message NOTIFY statusChanged)
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY historyChanged)
    Q_PROPERTY(bool canRedo READ canRedo NOTIFY historyChanged)
    QString m_root,m_message;
    mutable QVariantList m_countryOptions;
    QVector<QJsonObject> m_packs,m_undo,m_redo;
    int m_pack=0,m_selected=-1;
    QJsonObject currentPack()const;
    bool persist(const QJsonObject &pack,QString &error,bool validate=true)const;
    bool commit(const QJsonObject &pack,int selection,const QString &message={},bool perksOnly=false);
    bool validatePack(const QJsonObject &pack,QString &error)const;
    void status(const QString &message);
public:
    explicit HillStudioController(QObject *parent=nullptr,const QString &root={});
    QVariantList packs()const;
    QVariantList profiles()const;
    QVariantMap profile()const;
    QVariantList selectedPerks()const;
    double realHs()const;
    QVariantList countries()const;
    QVariantList legacyHills()const;
    int packIndex()const{return m_pack;}
    int selectedIndex()const{return m_selected;}
    QString packName()const;
    QString message()const{return m_message;}
    bool canUndo()const{return !m_undo.isEmpty();}
    bool canRedo()const{return !m_redo.isEmpty();}
    Q_INVOKABLE void selectPack(int index);
    void invalidateText(){emit changed();}
    Q_INVOKABLE void createPack();
    Q_INVOKABLE void renamePack(const QString &name);
    Q_INVOKABLE void select(int index);
    Q_INVOKABLE void add();
    Q_INVOKABLE void duplicate();
    Q_INVOKABLE void remove();
    Q_INVOKABLE void setValue(const QString &key,const QVariant &value);
    Q_INVOKABLE void setPerk(int pair,int side,int level,bool toggle=true);
    Q_INVOKABLE void copyLegacy(int index);
    Q_INVOKABLE void undo();
    Q_INVOKABLE void redo();
    Q_INVOKABLE void exportPack();
    Q_INVOKABLE void importPack();
    bool exportTo(const QString &file);
    bool importFrom(const QString &file);
signals:
    void changed();
    void perksChanged();
    void historyChanged();
    void statusChanged();
};
