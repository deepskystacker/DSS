#ifndef ALIGNMENTPARAMETERS_H
#define ALIGNMENTPARAMETERS_H

#include <QWidget>

class CWorkspace;

namespace Ui {
class AlignmentParameters;
}

class AlignmentParameters : public QWidget
{
    Q_OBJECT

public:
    explicit AlignmentParameters(QWidget *parent = 0);
    ~AlignmentParameters();

private:
	Ui::AlignmentParameters *ui;
	std::unique_ptr<CWorkspace> workspace;
	uint m_Alignment;

	void updateText();

	void setAlignment(uint wAlignment);

private slots:
    void on_alignAuto_clicked();
    void on_alignBilinear_clicked();
    void on_alignBisquared_clicked();
    void on_alignBicubic_clicked();
    void on_alignNone_clicked();


private:
};

#endif // ALIGNMENTPARAMETERS_H
