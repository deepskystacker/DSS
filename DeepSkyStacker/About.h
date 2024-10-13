#pragma once
namespace DSS
{
    namespace Ui {
        class About;
    }

    class About : public QDialog
    {
        Q_OBJECT
            Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
            Q_PROPERTY(bool internetCheck READ internetCheck WRITE setInternetCheck NOTIFY internetCheckChanged)

            typedef QDialog
            Inherited;

    public:
        explicit About(QWidget* parent = 0);
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

    protected:
        void showEvent(QShowEvent* event) override;

    signals:
        void languageChanged(QString);
        void internetCheckChanged();

    private slots:
        void storeSettings();
        void selectLanguage(int);
        void setCheck(bool);
        void aboutQt();


    private:
        Ui::About* ui;

        bool initialised;
        QString m_Language;
        bool m_InternetCheck;

        void onInitDialog();
    };
}