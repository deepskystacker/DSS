#ifndef COMETSTACKING_H
#define COMETSTACKING_H
#include <memory>
#include "DSSCommon.h"
#include <QWidget>

namespace Ui {
class CometStacking;
}

class CWorkspace;

class CometStacking : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(COMETSTACKINGMODE cometStackingMode READ cometStackingMode WRITE setCometStackingMode NOTIFY cometStackingModeChanged);

public:
    explicit CometStacking(QWidget *parent = nullptr);
    ~CometStacking();

    void setCometStackingMode(COMETSTACKINGMODE mode)
    {
        if (mode != m_CometStackingMode)
        {
            m_CometStackingMode = mode;
            updateImage();
            emit cometStackingModeChanged();
        }
    }

    COMETSTACKINGMODE cometStackingMode()
    {
        return m_CometStackingMode;
    }

signals:
    void cometStackingModeChanged();

private slots:
    void onBnClickedStandardStacking(bool);
    void onBnClickedCometStacking(bool);
    void onBnClickedAdvancedStacking(bool);

private:
    Ui::CometStacking *ui;
	std::unique_ptr<CWorkspace> workspace;
    COMETSTACKINGMODE m_CometStackingMode;
    void updateImage();
};

#endif // COMETSTACKING_H
