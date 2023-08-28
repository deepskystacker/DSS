#pragma once
#include "commonresource.h"

namespace Ui {
	class AskRegistering;
}

class AskRegistering : public QDialog
{
	Q_OBJECT
		Ui::AskRegistering* ui;
public:
	explicit AskRegistering(QWidget* parent = nullptr);
	~AskRegistering();

	enum class Answer {
		ARA_ONE = 1,
		ARA_ALL = 2,
		ARA_CONTINUE = 3
	};

	//! represent the choice at any point even if the window was aborted or closed
	Answer desiredAction() const;
protected:
	void reject() override;
};