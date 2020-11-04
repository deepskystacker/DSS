#pragma once
#include <QToolBar>
#include <QGraphicsOpacityEffect>

class DSSToolBar : public QToolBar
{
    Q_OBJECT

public:
    explicit DSSToolBar(QWidget* parent) :
        QToolBar(parent),
        opacityEffect(this)
    {
        setOpacity(0.6, false);
    }

    ~DSSToolBar() {};

    inline void setOpacity(qreal opacity, bool upd)
    {
        opacityEffect.setOpacity(opacity);
        setGraphicsEffect(&opacityEffect);
        
        if (upd) update();
    };

protected:
    inline void enterEvent([[maybe_unused]] QEvent* e) override
    {
        setOpacity(1.0, true);
    };

    inline void leaveEvent([[maybe_unused]] QEvent* e) override
    {
        setOpacity(0.6, true);
    };

    QGraphicsOpacityEffect opacityEffect;
};
