#ifndef INTERMEDIATEFILES_H
#define INTERMEDIATEFILES_H

#include <QWidget>

namespace Ui {
class IntermediateFiles;
}

class IntermediateFiles : public QWidget
{
    Q_OBJECT

public:
    explicit IntermediateFiles(QWidget *parent = nullptr);
    ~IntermediateFiles();

private:
    Ui::IntermediateFiles *ui;
};

#endif // INTERMEDIATEFILES_H
