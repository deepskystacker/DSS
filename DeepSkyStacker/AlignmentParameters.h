#ifndef ALIGNMENTPARAMETERS_H
#define ALIGNMENTPARAMETERS_H

#include <QWidget>

namespace Ui {
class AlignmentParameters;
}

class AlignmentParameters : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(uint16_t alignment READ alignment WRITE setAlignment NOTIFY alignmentChanged)

public:
    explicit AlignmentParameters(QWidget *parent = 0);
    ~AlignmentParameters();

    uint16_t alignment()
    {
        return m_Alignment;
    };

    void setAlignment(uint16_t wAlignment)
    {
        if (m_Alignment != wAlignment)
        {
            m_Alignment = wAlignment;
            updateText();
            emit alignmentChanged();
        }
    };

signals:
    void alignmentChanged();

private slots:
    void onAutomaticClicked();
    void onBilinearClicked();
    void onBisquaredClicked();
    void onBicubicClicked();
    void onNoAlignemtnClicked();
    void updateText();

private:
    uint16_t m_Alignment;
    Ui::AlignmentParameters *ui;
};

#endif // ALIGNMENTPARAMETERS_H
