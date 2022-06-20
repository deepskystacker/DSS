#ifndef RECOMMENDEDSETTINGS_H
#define RECOMMENDEDSETTINGS_H
#include <memory>
#include <vector>

class CWorkspace;
class QAbstractButton;
class QUrl;

#include <QString>
#include <QDialog>

#include "DSSCommon.h"
#include "StackingTasks.h"
#include "Workspace.h"


class RecommendationItem
{
public:
	WORKSPACESETTINGVECTOR				vSettings;
	int								linkID;
	QString								recommendation;


public:
	RecommendationItem() :
		linkID(0)
	{
	};

	~RecommendationItem()
	{
	};

	RecommendationItem(const RecommendationItem & rhs) :
		vSettings(rhs.vSettings),
		linkID(rhs.linkID),
		recommendation(rhs.recommendation)
	{}

	RecommendationItem & operator = (const RecommendationItem & rhs)
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

	bool	differsFromWorkspace()
	{
		bool					bResult = false;
		CWorkspace				workspace;

		// Check that the current values are (or not)
		for (const auto setting : vSettings)
		{
			QString				keyName;
			QVariant			value;
			QVariant			currentValue;


			keyName = setting.key();

			currentValue = workspace.value(keyName);
			value = setting.value();

			switch (value.type())
			{
			case QMetaType::Bool:
				bResult = value.toBool() != currentValue.toBool();
				break;
			case QMetaType::Double:
				bResult = value.toDouble() != currentValue.toDouble();
				break;
			default:
				bResult = value.toString() != currentValue.toString();
			}

			//
			// If different, no need to check any more
			//
			if (bResult) break;
		};
		return bResult;
	};

	void	applySettings()
	{
		CWorkspace				workspace;

		for (size_t i = 0; i < vSettings.size(); i++)
		{
			QString				keyName;
			QVariant			value;

			keyName = vSettings[i].key();
			value = vSettings[i].value();

			workspace.setValue(keyName, value);
		};
	};

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

	Recommendation(const Recommendation & rhs) :
		vRecommendations(rhs.vRecommendations),
		isApplicable(rhs.isApplicable),
		isImportant(rhs.isImportant),
		breakBefore(rhs.breakBefore),
		text(rhs.text)
	{}

	Recommendation & operator = (const Recommendation & rhs)
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

	void	addItem(const RecommendationItem & ri)
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
	explicit RecommendedSettings(QWidget *parent = nullptr);
	~RecommendedSettings();

	inline void setStackingTasks(CAllStackingTasks * stackingTasks) noexcept
	{
		pStackingTasks = stackingTasks;
	};


private slots:
	void accept() override;
	void reject() override;
	void on_textBrowser_anchorClicked(const QUrl &);

private:
	Ui::RecommendedSettings *ui;
	std::unique_ptr<CWorkspace> workspace;
	RECOMMENDATIONVECTOR vRecommendations;
	CAllStackingTasks stackingTasks;
	CAllStackingTasks *pStackingTasks;
	bool	initialised;

	void	clearText();

	void	insertHeader();
	void	insertHTML(const QString& html, const QColor& colour = QColor(Qt::black), bool bBold = false, bool bItalic = false, int lLinkID = -1);
	void	fillWithRecommendedSettings();

	void	setSetting(int lID = 0);


	void showEvent(QShowEvent *event) override;
	void onInitDialog();
};
/* ------------------------------------------------------------------- */
#endif