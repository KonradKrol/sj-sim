#ifndef AUTHORWINDOW_H
#define AUTHORWINDOW_H

#include <QDialog>
#include <QVariantMap>

class QTextBrowser;

class AuthorWindow : public QDialog
{
    Q_OBJECT
public:
    explicit AuthorWindow(QWidget *parent = nullptr);
    QVariantMap details() const;
    void showCredits();
protected:
    void changeEvent(QEvent *event) override;
private:
    QString credits;
    QTextBrowser *biography;
    void updateText();
};

#endif
