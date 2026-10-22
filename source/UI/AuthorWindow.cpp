#include "AuthorWindow.h"
#include "ThanksWindow.h"
#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextBrowser>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QPushButton>

AuthorWindow::AuthorWindow(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("O autorze"));
    resize(960,640);
    auto *layout = new QHBoxLayout(this);
    auto *photo = new QLabel(this);
    photo->setPixmap(QPixmap(":/img/author/konrad.jpg").scaled(280,560,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    photo->setAlignment(Qt::AlignTop);
    photo->setAccessibleName(tr("Zdjęcie Konrada Króla"));
    biography = new QTextBrowser(this);
    biography->setOpenExternalLinks(true);
    layout->addWidget(photo);
    auto *body = new QVBoxLayout;
    body->addWidget(biography,1);
    auto *acknowledgements = new QPushButton(tr("Podziękowania"),this);
    connect(acknowledgements,&QPushButton::clicked,this,&AuthorWindow::showCredits);
    body->addWidget(acknowledgements,0,Qt::AlignLeft);
    layout->addLayout(body,1);
    updateText();
}

QVariantMap AuthorWindow::details() const
{
    return {{"heading",tr("Cześć, jestem Konrad.")},
            {"introduction",tr("Mam 18 lat i programuję od dziesiątego roku życia. Poza kodem najwięcej miejsca zajmują u mnie skoki narciarskie — dlatego powstało kilka moich gier o tej dyscyplinie.")},
            {"work",tr("Pracuję z aplikacjami serwerowymi w .NET i nowoczesnymi aplikacjami mobilnymi we Flutterze.")},
            {"availability",tr("Chętnie porozmawiam o pracy lub płatnym zadaniu.")},
            {"github",QStringLiteral("https://github.com/KonradKrol")},
            {"linkedin",QStringLiteral("https://www.linkedin.com/in/konrad-kr%C3%B3l-bb51a72a7/")},
            {"credits",credits},
            {"projects",QVariantList{
                QVariantMap{{"name",QStringLiteral("Sj.Sim Predazzo Edition")},{"description",tr("Odświeżona wersja klasycznego symulatora stworzona w kilka dób, specjalnie na Zimowe Igrzyska w 2026 roku.")},{"url",QStringLiteral("https://github.com/KonradKrol/sjsim-2026")}},
                QVariantMap{{"name",QStringLiteral("Ski Jump Draft")},{"description",tr("Gra online o skokach narciarskich oparta na rywalizacji. Gracze kompletują własną drużynę i konkurują z innymi. W 2025 roku odbyły się testy beta.")},{"url",QStringLiteral("https://github.com/Ski-Jump-Draft")}}
            }}};
}

void AuthorWindow::updateText()
{
    ThanksWindow thanks;
    credits = thanks.findChild<QTextEdit *>()->toPlainText();
    const auto info = details();
    QString html = "<h1>"+info.value("heading").toString().toHtmlEscaped()+"</h1><p>"+info.value("introduction").toString().toHtmlEscaped()+"</p>";
    for(const auto &value : info.value("projects").toList()) {
        const auto project=value.toMap();
        html += "<h3><a href=\""+project.value("url").toString()+"\">"+project.value("name").toString()+"</a></h3><p>"+project.value("description").toString().toHtmlEscaped()+"</p>";
    }
    html += "<p>"+info.value("work").toString().toHtmlEscaped()+"</p><hr><p><b>"+info.value("availability").toString().toHtmlEscaped()+"</b></p>";
    html += "<p><a href=\""+info.value("github").toString()+"\">GitHub</a> · <a href=\""+info.value("linkedin").toString()+"\">LinkedIn</a></p>";
    biography->setHtml(html);
}

void AuthorWindow::changeEvent(QEvent *event)
{
    QDialog::changeEvent(event);
    if(event->type()==QEvent::LanguageChange) {
        setWindowTitle(tr("O autorze"));
        updateText();
    }
}

void AuthorWindow::showCredits()
{
    QMessageBox box(QMessageBox::NoIcon,tr("Podziękowania"),credits,QMessageBox::Close,this);
    box.setTextFormat(Qt::PlainText);
    box.exec();
}
