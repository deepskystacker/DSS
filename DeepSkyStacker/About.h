#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>

namespace Ui {
class About;
}

class About : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged);
    Q_PROPERTY(bool internetCheck READ internetCheck WRITE setInternetCheck NOTIFY internetCheckChanged);

public:
    explicit About(QWidget *parent = 0);
    ~About();

    QString language()
    {
        return m_Language;
    }

    void setLanguage(QString lang);

    bool internetCheck()
    {
        return m_InternetCheck;
    }

    void setInternetCheck(bool);

signals:
    void languageChanged();
    void internetCheckChanged();

private slots:
    void storeSettings();
    void selectLanguage(int);
    void setCheck(bool);

private:
    Ui::About *ui;
    QString m_Language;
    bool m_InternetCheck;
};

#endif // ABOUT_H
