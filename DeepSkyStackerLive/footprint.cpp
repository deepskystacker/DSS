#include "stdafx.h"
#include "footprint.h"
#include "imageview.h"

namespace DSS
{

    Footprint::Footprint(QWidget* parent) :
        QWidget(parent)
    {
        imageView = dynamic_cast<ImageView*>(parent);
        ZASSERT(nullptr != imageView);
        setAttribute(Qt::WA_TransparentForMouseEvents);
        setAttribute(Qt::WA_NoSystemBackground);
        setAttribute(Qt::WA_WState_ExplicitShowHide);

        connect(imageView, &ImageView::Image_resizeEvent,
            this, &Footprint::resizeMe);

    }

    Footprint::~Footprint()
    {}

    void Footprint::changeEvent(QEvent* e)
    {
        QWidget::changeEvent(e);
        switch (e->type())
        {
        case QEvent::ParentChange:
            if (parent())
            {
                setWindowFlags(windowFlags() & ~Qt::ToolTip);
            }
            else
            {
                setWindowFlags(windowFlags() | Qt::ToolTip);
            }
            break;
        default:
            break;
        }

        if (e->type() == QEvent::ZOrderChange)
            raise();
    }

    void Footprint::paintEvent(QPaintEvent*)
    {
        QPainter painter(this);
        QColor colour{ QColorConstants::Cyan };
        colour.setAlpha(128);
        painter.setPen(QPen(colour, 1.0));

        if (pt1_ != pt2_)
        {
            //
            // Convert image coordinates to screen coordinates
            //
            QPointF p1 = imageView->imageToScreen(pt1_);
            QPointF p2 = imageView->imageToScreen(pt2_);
            QPointF p3 = imageView->imageToScreen(pt3_);
            QPointF p4 = imageView->imageToScreen(pt4_);
            //
            // and draw the last image footprint
            //
            painter.drawLine(p1, p2);
            painter.drawLine(p2, p3);
            painter.drawLine(p3, p4);
            painter.drawLine(p4, p1);
        }
    }

    void Footprint::resizeMe(QResizeEvent* e)
    {
        resize(e->size());
    }

    void Footprint::showEvent(QShowEvent* e)
    {
        resize(imageView->size());
        raise();
        Inherited::showEvent(e);
    }
} // namespace DSS
