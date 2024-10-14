#pragma once

class Workspace;

namespace DSS
{
    namespace Ui {
        class AlignmentParameters;
    }

    class AlignmentParameters : public QWidget
    {
        Q_OBJECT

            typedef QWidget
            Inherited;

    public:
        explicit AlignmentParameters(QWidget* parent = 0);
        ~AlignmentParameters();

    public slots:
        void onSetActive();

    private:
        Ui::AlignmentParameters* ui;
        std::unique_ptr<Workspace> workspace;
        uint m_Alignment;

        void updateControls();

        void setAlignment(uint wAlignment);

    private slots:
        void on_alignAuto_clicked();
        void on_alignBilinear_clicked();
        void on_alignBisquared_clicked();
        void on_alignBicubic_clicked();
        void on_alignNone_clicked();


    private:
    };
}