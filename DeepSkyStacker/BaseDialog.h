#pragma once

namespace DSS
{
	inline const QString DIALOG_GEOMETRY_SETTING{ "Dialogs/%1/geometry" };
	class BaseDialog : public QDialog
	{
		Q_OBJECT

		typedef QDialog
			Inherited;

	public:
		enum class Behaviour
		{
			None = 0x00,
			PersistGeometry = 0x01
		};

		Q_DECLARE_FLAGS(Behaviours, Behaviour);

		BaseDialog(const Behaviours& behaviours = Behaviour::None, QWidget* parent = nullptr);

	protected:
		void showEvent(QShowEvent* event) override;

	private slots:
		void saveState() const;

	private:
		virtual void onInitDialog();

		inline bool hasPersistentGeometry() const {
			return m_behaviours.testFlag(Behaviour::PersistGeometry);
		}

	private:
		Behaviours		m_behaviours{ Behaviour::None };
		bool			m_initialised{ false };

	private:
		Q_DISABLE_COPY(BaseDialog)
	};

	Q_DECLARE_OPERATORS_FOR_FLAGS(BaseDialog::Behaviours)
}