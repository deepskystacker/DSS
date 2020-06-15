#ifndef STACKRECAP_H
#define STACKRECAP_H
#include <memory>

class CWorkspace;
class QAbstractButton;
class QUrl;

#include "DSSCommon.h"
#include "StackingTasks.h"
#include <QDialog>

namespace Ui {
	class StackRecap;
}

class StackRecap : public QDialog
{
	Q_OBJECT

typedef QDialog
		Inherited;
public:
	explicit StackRecap(QWidget *parent = nullptr);
	~StackRecap();

	inline void setStackingTasks(CAllStackingTasks * stackingTasks) noexcept
	{
		pStackingTasks = stackingTasks;
	};


private slots:
	void accept() override;
	void reject() override;
	void on_recommended_clicked();
	void on_stackSettings_clicked();
	void on_textBrowser_anchorClicked(const QUrl &);

private:
	Ui::StackRecap *ui;
	std::unique_ptr<CWorkspace> workspace;
	CAllStackingTasks *pStackingTasks;
	bool	initialised;

	inline void	SpaceToQString(__int64 ulSpace, QString & strSpace)
	{
		double fKb, fMb, fGb;

		fKb = ulSpace / 1024.0;
		fMb = fKb / 1024.0;
		fGb = fMb / 1024.0;


		if (fKb < 900)
			strSpace = QString(tr("%L1 kB", "IDS_RECAP_KILOBYTES"))
				.arg(fKb, 0, 'f', 1);
		else if (fMb < 900)
			strSpace = QString(tr("%L1 MB", "IDS_RECAP_MEGABYTES"))
				.arg(fMb, 0, 'f', 1);
		else
			strSpace = QString(tr("%L1 GB", "IDS_RECAP_GIGABYTES"))
				.arg(fGb, 0, 'f', 1);
	};

	void CallStackingSettings(int tab = 0);

	//void	clearText();

	void	insertHeader(QString & strHTML);
	void	insertHTML(QString & strHTML, const QString& szText, QColor colour = QColor(Qt::black), bool bBold = false, bool bItalic = false, LONG lLinkID = -1);
	void	fillWithAllTasks();

	void showEvent(QShowEvent *event) override;
	void onInitDialog();
};

/* ------------------------------------------------------------------- */
#endif
