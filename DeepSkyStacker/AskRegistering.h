#pragma once
#include <QDialog>

namespace Ui {
	class AskRegistering;
}

class AskRegistering : public QDialog
{
	Q_OBJECT
	Ui::AskRegistering* ui;

public:
	explicit AskRegistering(QWidget* parent = nullptr);
	~AskRegistering() override;

	enum class Answer {
		ARA_ONE = 1, //!< register the current frame
		ARA_ALL = 2, //!< register this frame and all other checked ones
		ARA_CONTINUE = 3, //!< do nothing
	};

	//! represent the choice at any point even if the window was aborted or closed
	Answer desiredAction() const;

protected:
	void reject() override;
};