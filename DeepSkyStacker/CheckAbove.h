#pragma once

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

	class CheckAboveValidator : public QValidator
	{
		Q_OBJECT

	public:
		CheckAboveValidator(QObject* parent = nullptr);

		// By default, the pos parameter is not used by this validator - @see QDoubleValidator
		QValidator::State validate(QString& input, int&) const override;
		void fixup(QString& input) const override;

	private:
		QDoubleValidator* m_doubleValueValidator{ nullptr };
		QDoubleValidator* m_percentValueValidator{ nullptr };

	private:
		Q_DISABLE_COPY(CheckAboveValidator)
	};
}
