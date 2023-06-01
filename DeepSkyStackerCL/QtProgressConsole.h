#pragma once
#include "DSSProgress.h"
#include "DSSCommon.h"

namespace DSS
{
	class ProgressConsole : public QObject, public ProgressBase
	{
		Q_OBJECT

	private:
		QTextStream m_out;
		TERMINAL_OUTPUT_MODE m_style;
		QString m_strLastSent[3];

	public:
		ProgressConsole(TERMINAL_OUTPUT_MODE mode) :
			ProgressBase(),
			m_out(stdout),
			m_style(mode)
		{
		}
		virtual ~ProgressConsole()
		{
			if(m_style == TERMINAL_OUTPUT_MODE::COLOURED)
				m_out << "\033[0m";
			Close();
		}

		//
		// These eight mfs implement the public interface defined in DSS::ProgressBase
		// They invoke the corresponding slots using QMetaObject::invokeMethod
		// so that they can be invoked from ANY thread in the application will run on
		// the GUI thread.
		//
		virtual void Start1(const QString& title, int total1, bool enableCancel = false) override
		{
			QMetaObject::invokeMethod(this, "slotStart1", Qt::AutoConnection,
				Q_ARG(const QString&, title),
				Q_ARG(int, total1),
				Q_ARG(bool, enableCancel));
		}

		virtual void Progress1(const QString& text, int achieved) override
		{
			QMetaObject::invokeMethod(this, "slotProgress1", Qt::AutoConnection,
				Q_ARG(const QString&, text),
				Q_ARG(int, achieved));
		}

		virtual void Start2(const QString& title, int total2) override
		{
			QMetaObject::invokeMethod(this, "slotStart2", Qt::AutoConnection,
				Q_ARG(const QString&, title),
				Q_ARG(int, total2));
		}

		virtual void Progress2(const QString& text, int achieved) override
		{
			QMetaObject::invokeMethod(this, "slotProgress2", Qt::AutoConnection,
				Q_ARG(const QString&, text),
				Q_ARG(int, achieved));
		}

		virtual void End2() override
		{
			QMetaObject::invokeMethod(this, "slotEnd2", Qt::AutoConnection);
		}

		virtual void Close() override
		{
			QMetaObject::invokeMethod(this, "slotClose", Qt::AutoConnection);
		}

		virtual bool IsCanceled() const override { return false; }

		virtual bool Warning(const QString& szText)
		{
			return doWarning(szText);
		}

		void initialise(){};
		void applyStart1Text(const QString& strText) { PrintText(strText, OT_TEXT1); }
		void applyStart2Text(const QString& strText) 
		{
			if(m_total2>0)
				PrintText(strText, OT_TEXT2); 
		}
		void applyProgress1([[maybe_unused]] int lAchieved)
		{
			PrintText(GetProgress1Text(), OT_PROGRESS1);
		}
		void applyProgress2([[maybe_unused]] int lAchieved)
		{
			PrintText(GetProgress2Text(), OT_PROGRESS2);
		}
		void applyTitleText(const QString& strText) { PrintText(strText, OT_TITLE); }

		void endProgress2(){}
		void closeProgress() { }
		bool doWarning([[maybe_unused]] const QString& szText) { return true; }
		virtual void applyProcessorsUsed([[maybe_unused]] int nCount) override {};

	protected slots:
		virtual void slotStart1(const QString& szTitle, int lTotal1, bool bEnableCancel = true);
		virtual void slotProgress1(const QString& szText, int lAchieved1);
		virtual void slotStart2(const QString& szText, int lTotal2);
		virtual void slotProgress2(const QString& szText, int lAchieved2);
		virtual void slotEnd2();
		virtual void slotClose();

	private:
		void PrintText(const QString& szText, eOutputType type)
		{
			QString singleLineText(szText);
			singleLineText.replace('\n', ' ');

			switch (m_style)
			{
			case TERMINAL_OUTPUT_MODE::BASIC:
				PrintBasic(singleLineText, type, false);
				break;
			case TERMINAL_OUTPUT_MODE::COLOURED:
				PrintBasic(singleLineText, type, true);
				break;
			}
		}
		void PrintBasic(const QString& szText, eOutputType type, bool bColour)
		{
			switch(type)
			{
			case OT_TITLE:
				if (m_strLastSent[0].compare(szText) != 0)
				{
					m_out << (bColour ? "\033[036m" : "") << szText << "\n";
					m_strLastSent[0] = szText;
				}
				break;
			case OT_TEXT1:
				if (m_strLastSent[1].compare(szText) != 0)
				{
					// Don't echo out if the same as the title text.
					if (m_strLastSent[1].compare(m_strLastSent[0]) != 0)
						m_out << (bColour ? "\033[1;33m" : "") << szText << "\n";
					m_strLastSent[1] = szText;
				}
				break;
			case OT_TEXT2:
				if (m_strLastSent[2].compare(szText) != 0)
				{
					// Don't echo out if the same as the detail text
					if(m_strLastSent[2].compare(m_strLastSent[1]) != 0)
						m_out << (bColour ? "\033[1;33m" : "") << szText << "\n";
					m_strLastSent[2] = szText;
				}
					
				break;
			case OT_PROGRESS1:
			case OT_PROGRESS2:
				m_out << (bColour ? "\033[32m" : "") << szText << "\r";
				break;
			}
			m_out.flush();
		}
	};
}
