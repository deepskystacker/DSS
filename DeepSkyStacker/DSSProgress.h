#ifndef _DSSPROGRESS_H__
#define _DSSPROGRESS_H__
#include <QElapsedTimer>

/* ------------------------------------------------------------------- */

namespace DSS
{
	class ProgressBase
	{
	protected:
		bool m_jointProgress;
		int m_total1;
		int m_total2;
		int m_lastTotal1;
		int m_lastTotal2;
		bool m_firstProgress;
		bool m_enableCancel;
		QElapsedTimer m_timer;

		enum eOutputType
		{
			OT_TITLE = 0,
			OT_TEXT1,
			OT_TEXT2,
			OT_PROGRESS1,
			OT_PROGRESS2,
			OT_MAX,
		};
		QString m_strLastOut[OT_MAX];

		static const QString m_strEmptyString;
		static constexpr float m_minProgressStep{ 5.0f };

	public:
		ProgressBase() :
			m_jointProgress{ false },
			m_total1{ 0 },
			m_total2{ 0 },
			m_lastTotal1{ 0 },
			m_lastTotal2{ 0 },
			m_firstProgress{ false },
			m_enableCancel{ false }
		{}

		virtual ~ProgressBase() = default;

		void Start1(const QString& szTitle, int lTotal1, bool bEnableCancel = true);
		bool Progress1(const QString& szText, int lAchieved1);
		void Start2(const QString& szText, int lTotal2);
		bool Progress2(const QString& szText, int lAchieved2);
		void End2();
		bool IsCanceled();
		void Close();
		bool Warning(const QString& szText);

		void SetJointProgress(bool bJointProgress) { m_jointProgress = bJointProgress; };
		const QString& GetStart1Text() const { return m_strLastOut[OT_TEXT1]; }
		const QString& GetStart2Text() const { return m_strLastOut[OT_TEXT2]; }
		const QString& GetTitleText() const { return m_strLastOut[OT_TITLE]; }
		const QString& GetProgress1Text() const { return m_strLastOut[OT_PROGRESS1]; }
		const QString& GetProgress2Text() const { return m_strLastOut[OT_PROGRESS2]; }

		// Helper functions - when you just want to update the progress and not the text.
		void Start1(int lTotal1, bool bEnableCancel = true) { Start1(m_strEmptyString, lTotal1, bEnableCancel); }
		void Start2(int lTotal2) { Start2(m_strEmptyString, lTotal2); }
		bool Progress1(int lAchieved1) { return Progress1(m_strEmptyString, lAchieved1); }
		bool Progress2(int lAchieved2) { return Progress2(m_strEmptyString, lAchieved2); }

	protected:
		void UpdateProcessorsUsed();

	protected: // Pure virtual - implementation of the progress bar will fill these in.
		virtual void applyStart1Text(const QString& strText) = 0;
		virtual void applyStart2Text(const QString& strText) = 0;
		virtual void applyProgress1(int lAchieved) = 0;
		virtual void applyProgress2(int lAchieved) = 0;
		virtual void applyTitleText(const QString& strText) = 0;
		virtual void initialise() = 0;
		virtual void endProgress2() = 0;
		virtual bool hasBeenCanceled() = 0;
		virtual void closeProgress() = 0;
		virtual bool doWarning(const QString& szText) = 0;
		virtual void applyProcessorsUsed(int nCount) = 0;
	};
}
/* ------------------------------------------------------------------- */

#endif // _DSSPROGRESS_H__