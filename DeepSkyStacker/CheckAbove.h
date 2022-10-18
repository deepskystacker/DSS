#pragma once

#include <QDialog>

namespace Ui {
	class CheckAbove;
}

namespace DSS
{
	class CheckAbove : public QDialog
	{
		Q_OBJECT

	typedef QDialog
			Inherited;

	public:
		CheckAbove(QWidget* parent = nullptr);
		virtual ~CheckAbove();


		double		threshold() const
		{
			return m_fThreshold;
		};

		bool		isPercent() const
		{
			return m_bPercent;
		};

	private slots:
		void accept() override;


	private:
		Ui::CheckAbove*		ui{ nullptr };
		double				m_fThreshold{ 0. };
		bool				m_bPercent{ false };

	private:
		Q_DISABLE_COPY(CheckAbove)
	};
}
