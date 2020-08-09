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
