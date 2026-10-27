#ifndef GLOBALTRANSLATORS_H
#define GLOBALTRANSLATORS_H

#include <QTranslator>
#include <QVector>
class QApplication;


class GlobalTranslators
{
private:
    GlobalTranslators();

    static GlobalTranslators * m_globalTranslators;
    QVector<QTranslator *> globalTranslations;
    QTranslator qtTranslation;
    int activeLanguage = -1;
    void configTranslations();

public:
    GlobalTranslators(GlobalTranslators &) = delete;
    static GlobalTranslators * get();
    bool setLanguage(QApplication *application, int language);

    QVector<QTranslator *> getGlobalTranslations() const;
    void setGlobalTranslations(const QVector<QTranslator *> &newGlobalTranslations);
};

#endif // GLOBALTRANSLATORS_H
