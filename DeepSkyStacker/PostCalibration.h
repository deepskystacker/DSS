#ifndef POSTCALIBRATION_H
#define POSTCALIBRATION_H

#include <QWidget>

namespace Ui {
class PostCalibration;
}

class PostCalibration : public QWidget
{
    Q_OBJECT

public:
    explicit PostCalibration(QWidget *parent = nullptr);
    ~PostCalibration();

private:
    Ui::PostCalibration *ui;
};

#endif // POSTCALIBRATION_H
