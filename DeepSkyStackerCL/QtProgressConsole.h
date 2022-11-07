#pragma once
#include "DSSProgress.h"

namespace Ui {
	class ProgressDlg;
}

namespace DSS
{
	class QtProgressConsole : public CDSSProgress
	{
	private:
		LONG m_lTotal1;
		LONG m_lTotal2;
		DWORD m_dwStartTime;
		DWORD m_dwLastTime;
		LONG m_lLastTotal1;
		LONG m_lLastTotal2;
		bool m_bFirstProgress;
		QString m_strTitle;
		QString m_strStartText;
		QString m_strStart2Text;
		bool m_bTitleDone;
		bool m_bStartTextDone;
		bool m_bStart2TextDone;
		QString m_strLastOut;

	private:
		void	PrintText(const QString& szText)
		{
			if (!szText.isEmpty() && m_strLastOut.compare(szText, Qt::CaseInsensitive) != 0)
			{
				_tprintf(_T("%s\n"), szText.toStdWString().c_str());
				m_strLastOut = szText;
			}
		}

	public:
		QtProgressConsole()
		{
			m_bTitleDone = false;
			m_bStartTextDone = false;
			m_bStart2TextDone = false;
		}
		virtual ~QtProgressConsole()
		{
			Close();
		}

		virtual const QString GetStartText() const override
		{
			return m_strStartText;
		}

		virtual const QString GetStart2Text() const override
		{
			return m_strStart2Text;
		}

		virtual	void Start(const QString& szTitle, int lTotal1, bool bEnableCancel = true) override
		{
			m_lLastTotal1 = 0;
			m_lTotal1 = lTotal1;
			m_dwStartTime = GetTickCount();
			m_dwLastTime = m_dwStartTime;
			m_bFirstProgress = true;
			if (!szTitle.isEmpty())
			{
				if (m_strTitle.compare(szTitle, Qt::CaseInsensitive) != 0)
					m_bTitleDone = false;
				m_strTitle = szTitle;
				if (!m_bTitleDone)
				{
					PrintText(m_strTitle);
					m_bTitleDone = true;
				}
			}
		}
		virtual void Progress1(const QString& szText, int lAchieved1) override
		{
			DWORD			dwCurrentTime = GetTickCount();
			if (!szText.isEmpty())
			{
				if (m_strStartText.compare(szText, Qt::CaseInsensitive) != 0)
					m_bStartTextDone = false;
				m_strStartText = szText;
			}

			if (m_bFirstProgress || ((double)(lAchieved1 - m_lLastTotal1) > (m_lTotal1 / 100.0)) || ((dwCurrentTime - m_dwLastTime) > 1000))
			{
				m_bFirstProgress = false;
				m_lLastTotal1 = lAchieved1;
				m_dwLastTime = dwCurrentTime;

				double			fAchieved = 0.0;
				if (m_lTotal1)
					fAchieved = (double)lAchieved1 / (double)m_lTotal1 * 100.0;

				// Update progress
				if (!m_bStartTextDone && !m_strStartText.isEmpty())
				{
					PrintText(m_strStartText);
					m_bStartTextDone = true;
				}
				printf("%.1f %%   \r", fAchieved);
			}
		}

		virtual void Start2(const QString& szText, int lTotal2) override
		{
			m_lLastTotal2 = 0;
			if (!szText.isEmpty())
			{
				if (m_strStart2Text.compare(szText, Qt::CaseInsensitive) != 0)
					m_bStart2TextDone = false;

				m_strStart2Text = szText;
				if (!m_bStart2TextDone && !m_strStart2Text.isEmpty())
				{
					PrintText(m_strStart2Text);
					m_bStart2TextDone = true;
				};
			};

			m_lTotal2 = lTotal2;
		}

		virtual void Progress2(const QString& szText, int lAchieved2) override
		{
			if ((double)(lAchieved2 - m_lLastTotal2) > (m_lTotal2 / 100.0))
			{
				m_lLastTotal2 = lAchieved2;
				if (!szText.isEmpty())
				{
					if (m_strStart2Text.compare(szText, Qt::CaseInsensitive) != 0)
						m_bStart2TextDone = false;
					m_strStart2Text = szText;
				};

				double			fAchieved = 0.0;
				if (m_lTotal2)
					fAchieved = (double)lAchieved2 / (double)m_lTotal2 * 100.0;

				if (!m_bStart2TextDone && !m_strStart2Text.isEmpty())
				{
					PrintText(m_strStart2Text);
					m_bStart2TextDone = true;
				};
				printf("%.1f %%   \r", fAchieved);
			}
		}

		virtual void End2() override
		{
		}

		virtual bool IsCanceled() override
		{
			return false;
		}
		virtual bool Close() override
		{
			return true;
		}
	};
}
