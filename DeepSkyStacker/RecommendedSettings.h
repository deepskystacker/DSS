#pragma once
#include "Workspace.h"
#include "StackingTasks.h"

class Workspace;
class QAbstractButton;
class QUrl;

namespace DSS
{
	class RecommendationItem
	{
	public:
		WORKSPACESETTINGVECTOR vSettings;
		int linkID;
		QString recommendation;


	public:
		RecommendationItem() :
			linkID(0)
		{
		};

		~RecommendationItem()
		{
		};

		RecommendationItem(const RecommendationItem& rhs) :
			vSettings(rhs.vSettings),
			linkID(rhs.linkID),
			recommendation(rhs.recommendation)
		{}

		RecommendationItem& operator = (const RecommendationItem& rhs)
		{
			vSettings = rhs.vSettings;
			linkID = rhs.linkID;
			recommendation = rhs.recommendation;
			return *this;
		}

		void	clear()
		{
			vSettings.clear();
			linkID = 0;
			recommendation.clear();
		};

		bool differsFromWorkspace();
		void applySettings();

		void	setRecommendation(QString text)
		{
			recommendation = text;
		};

		void	addSetting(QString keyName, QVariant value)
		{
			vSettings.emplace_back(keyName, value);
		};
	};

	typedef std::vector<RecommendationItem>			RECOMMENDATIONITEMVECTOR;

	/* ------------------------------------------------------------------- */

	class Recommendation
	{
	public:
		RECOMMENDATIONITEMVECTOR			vRecommendations;
		bool								isApplicable;
		bool								isImportant;
		bool								breakBefore;
		QString								text;

	public:
		Recommendation() :
			isApplicable(true),
			isImportant(true),
			breakBefore(false)
		{
		};

		~Recommendation()
		{
		};

		Recommendation(const Recommendation& rhs) :
			vRecommendations(rhs.vRecommendations),
			isApplicable(rhs.isApplicable),
			isImportant(rhs.isImportant),
			breakBefore(rhs.breakBefore),
			text(rhs.text)
		{}

		Recommendation& operator = (const Recommendation& rhs)
		{
			vRecommendations = rhs.vRecommendations;
			isApplicable = rhs.isApplicable;
			isImportant = rhs.isImportant;
			breakBefore = rhs.breakBefore;
			text = rhs.text;
			return *this;
		}

		void	setText(const QString& t)
		{
			text = t;
		};

		void	addItem(const RecommendationItem& ri)
		{
			vRecommendations.push_back(ri);
		};
	};

	typedef std::vector<Recommendation>	RECOMMENDATIONVECTOR;
	typedef RECOMMENDATIONVECTOR::iterator	RECOMMENDATIONITERATOR;

	// RecommendedSettings dialog

	namespace Ui {
		class RecommendedSettings;
	}

	class RecommendedSettings : public QDialog
	{
		Q_OBJECT

			typedef QDialog
			Inherited;
	public:
		explicit RecommendedSettings(QWidget* parent = nullptr);
		~RecommendedSettings();

		inline void setStackingTasks(CAllStackingTasks* tasks) noexcept
		{
			pStackingTasks = tasks;
		};


	private slots:
		void accept() override;
		void reject() override;
		void on_textBrowser_anchorClicked(const QUrl&);

	private:
		Ui::RecommendedSettings* ui;
		std::unique_ptr<Workspace> workspace;
		RECOMMENDATIONVECTOR vRecommendations;
		CAllStackingTasks stackingTasks;
		CAllStackingTasks* pStackingTasks;
		bool	initialised;
		bool	darkTheme;
		QColor blueColour;

		void	insertHeader();
		void	insertHTML(const QString& html, const QColor& colour = QColorConstants::Black, bool bBold = false, bool bItalic = false, int lLinkID = -1);
		void	fillWithRecommendedSettings();

		void	setSetting(int lID = 0);


		void showEvent(QShowEvent* event) override;
		void onInitDialog();
	};
}