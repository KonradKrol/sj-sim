#ifndef GLOBALTRANSLATORS_H
#define GLOBALTRANSLATORS_H

#include <QTranslator>
#include <QVector>


class GlobalTranslators
{
private:
    GlobalTranslators();

    static GlobalTranslators * m_globalTranslators;
    QVector<QTranslator *> globalTranslations;
    void configTranslations();

public:
    GlobalTranslators(GlobalTranslators &) = delete;
    static GlobalTranslators * get();

    QVector<QTranslator *> getGlobalTranslations() const;
    void setGlobalTranslations(const QVector<QTranslator *> &newGlobalTranslations);
};

#endif // GLOBALTRANSLATORS_H
