#pragma once

class Workspace;
class QAbstractButton;
class QUrl;
class CAllStackingTasks;

namespace DSS
{
	namespace Ui {
		class StackRecap;
	}

	class StackRecap : public QDialog
	{
		Q_OBJECT

			typedef QDialog
			Inherited;
	public:
		explicit StackRecap(QWidget* parent = nullptr);
		~StackRecap();

		inline void setStackingTasks(CAllStackingTasks* stackingTasks) noexcept
		{
			pStackingTasks = stackingTasks;
		};


	private slots:
		void accept() override;
		void reject() override;
		void on_recommended_clicked();
		void on_stackSettings_clicked();
		void on_textBrowser_anchorClicked(const QUrl&);

	private:
		Ui::StackRecap* ui;
		std::unique_ptr<Workspace> workspace;
		CAllStackingTasks* pStackingTasks;
		bool	initialised;
		QColor windowTextColour;
		QColor blueColour;

		void CallStackingSettings(int tab = 0);

		//void	clearText();

		void	insertHeader(QString& strHTML);
		void	insertHTML(QString& strHTML, const QString& szText, QColor colour = QColorConstants::Black, bool bBold = false, bool bItalic = false, int lLinkID = -1);
		void	fillWithAllTasks();

		void showEvent(QShowEvent* event) override;
		void onInitDialog();
	};
}