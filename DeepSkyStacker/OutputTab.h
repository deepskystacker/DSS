#ifndef OUTPUTTAB_H
#define OUTPUTTAB_H

#include <QWidget>

namespace Ui {
class OutputTab;
}

class OutputTab : public QWidget
{
    Q_OBJECT

typedef QWidget
		Inherited;

public:
    explicit OutputTab(QWidget *parent = nullptr);
    ~OutputTab();

private:
    Ui::OutputTab *ui;
};

#endif // OUTPUTTAB_H
