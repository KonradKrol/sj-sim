#include "GlobalTranslators.h"

#include <QMessageBox>

GlobalTranslators* GlobalTranslators::m_globalTranslators = nullptr;

GlobalTranslators::GlobalTranslators()
{
    configTranslations();
}

GlobalTranslators *GlobalTranslators::get()
{
    if(m_globalTranslators == nullptr)
        m_globalTranslators = new GlobalTranslators;

    return m_globalTranslators;
}

void GlobalTranslators::configTranslations()
{
    QTranslator * translator = new QTranslator;
    if(translator->load("translations/translation_en.qm") == false)
    {
        QMessageBox message(QMessageBox::Icon::Critical, "Nie można otworzyć pliku z angielskim tłumaczeniem", "Nie udało się otworzyć pliku translations/translation_en.qm\nUpewnij się, że istnieje tam taki plik lub ma on odpowiednie uprawnienia",  QMessageBox::StandardButton::Ok);
        message.setModal(true);
        message.exec();
    }
    globalTranslations.push_back(translator);
}

QVector<QTranslator *> GlobalTranslators::getGlobalTranslations() const
{
    return globalTranslations;
}

void GlobalTranslators::setGlobalTranslations(const QVector<QTranslator *> &newGlobalTranslations)
{
    globalTranslations = newGlobalTranslations;
}
