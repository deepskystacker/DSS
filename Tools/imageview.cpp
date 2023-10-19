/****************************************************************************
**
** Copyright (C) 2020 David C. Partridge
**
** BSD License Usage
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of DeepSkyStacker nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
**
****************************************************************************/
#include "stdafx.h"
#include "imageview.h"
#include "Ztrace.h"

namespace DSS
{
    constexpr int alpha = 155;
    constexpr qreal MAX_ZOOM = 7.25;

    ImageView::ImageView(QWidget* parent)
        : QWidget(parent),
        m_scale(1.0),
        m_zoom(1.0),
        m_origin(0.0, 0.0),
        m_pToolBar(nullptr),
        m_fourCorners(false),
        m_enableZoomImage(true),
        m_tipShowCount(0)
    {
        setAttribute(Qt::WA_MouseTracking);
        setFocusPolicy(Qt::StrongFocus);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        setToolTip(tr(
            "Ctrl++ or Ctrl+= to zoom in\n"
            "Ctrl+- to zoom out\n"
            " or use the mouse wheel to zoom\n"
            "Ctrl+4 to toggle 4-Corners mode"
        ));
     }

    void ImageView::changeEvent(QEvent* event)
    {
        if (event->type() == QEvent::LanguageChange)
        {
            retranslateUi();		// translate some of our stuff.
        }

        Inherited::changeEvent(event);
    }

    void ImageView::retranslateUi()
    {
        setToolTip(tr(
            "Ctrl++ or Ctrl+= to zoom in\n"
            "Ctrl+- to zoom out\n"
            " or use the mouse wheel to zoom\n"
            "Ctrl+4 to toggle 4-Corners mode"
        ));
    }

    void ImageView::leaveEvent(QEvent* e)
    {
        update();                   // Redraw the image without magnifying glass.
        emit Image_leaveEvent(e);
        Inherited::leaveEvent(e);
    }

    void ImageView::mousePressEvent(QMouseEvent* e)
    {
        m_enableZoomImage = false;
        emit Image_mousePressEvent(e);
    }

    void ImageView::mouseMoveEvent(QMouseEvent* e)
    {
        emit Image_mouseMoveEvent(e);
        update();
    }

    void ImageView::mouseReleaseEvent(QMouseEvent* e)
    {
        emit Image_mouseReleaseEvent(e);
        m_enableZoomImage = true;
    }

    void ImageView::resizeEvent(QResizeEvent* e)
    {
        QSize size{ e->size() };
        qreal ratio{ devicePixelRatio() };
        size *= ratio;
        m_drawingPixmap = QPixmap(size);
        m_drawingPixmap.setDevicePixelRatio(ratio);
        drawOnPixmap();
        update();
        emit Image_resizeEvent(e);
        Inherited::resizeEvent(e);
    }

    void ImageView::drawOnPixmap()
    {
        if (m_drawingPixmap.isNull()) return;
        QPainter painter(&m_drawingPixmap);
        QPalette palette{ QGuiApplication::palette() };
        QBrush brush{ palette.dark() };

        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.fillRect(rect(), brush);

        if (nullptr != pPixmap)
        {
            const QSize sz = size();
            const qreal pixWidth = pPixmap->width();
            const qreal pixHeight = pPixmap->height();
            const qreal hScale = static_cast<qreal>(sz.width()) / pixWidth;
            const qreal vScale = static_cast<qreal>(sz.height()) / pixHeight;
            m_scale = std::min(hScale, vScale);

            qreal xoffset = 0.0, yoffset = 0.0;
            if ((pixWidth * m_scale) < sz.width())
            {
                xoffset = floor((sz.width() - (pixWidth * m_scale)) / 2.0);
            }
            if ((pixHeight * m_scale) < sz.height())
            {
                yoffset = floor((sz.height() - (pixHeight * m_scale)) / 2.0);
            }
            m_origin = QPointF(xoffset, yoffset);

            displayRect.setTopLeft(m_origin);
            displayRect.setSize(QSizeF(pixWidth * m_scale, pixHeight * m_scale));

            if (m_pToolBar)
            {
                qreal width(m_pToolBar->width());
                qreal height(m_pToolBar->height());
                QPoint point(sz.width() - width, sz.height() - height);
                m_pToolBar->move(point);
            }

            //painter.save();
            //painter.translate(m_origin);
            //painter.scale(m_zoom * m_scale, m_zoom * m_scale);
            //painter.translate(-m_origin);

            //
            // Draw the rectangle of interest at the origin location
            //
            //painter.drawPixmap(m_origin, *pPixmap, rectOfInterest);
            //painter.restore();
            painter.drawPixmap(displayRect, *pPixmap, rectOfInterest);

            //
            // Has the user enabled four corners mode?
            // 
            if (m_fourCorners)
            {
                paintFourCorners(painter);
            }
            else if (nullptr != pOverlayPixmap)
            {
                //
                // Draw the overlay image in the same way that we drew the main image
                //
                //painter.save();
                //painter.translate(m_origin);
                //painter.scale(m_zoom * m_scale, m_zoom * m_scale);
                //painter.translate(-m_origin);

                //
                // Finally draw the rectangle of interest at the origin location
                //
                //painter.drawPixmap(m_origin, *pOverlayPixmap, rectOfInterest);
                //painter.restore();
                painter.setBackgroundMode(Qt::TransparentMode);
                painter.drawPixmap(0, 0, *pOverlayPixmap);
            }

        }
        painter.end();
    }

    void ImageView::paintEvent(QPaintEvent*)
    {
        QPainter painter(this);
        QBrush brush{ QGuiApplication::palette().dark() };

        painter.fillRect(rect(), brush);

        //
        // Draw the stuff we drew onto the working pixmap onto the screen
        // 
        painter.drawPixmap(0, 0, m_drawingPixmap);

        //
        // If there's actaully an image to display and we're not in "Four Corners" mode
        // we can show the magnifying glass view
        //
        if (nullptr != pPixmap && !m_fourCorners && m_enableZoomImage)
        {
            paintZoomImage(painter);
        }
        painter.end();
    }

    void ImageView::paintFourCorners(QPainter& painter)
    {

        const QRectF rc(rect());
        const qreal width{ (rc.width()) / 2.7 };
        const qreal height{ (rc.height()) / 2.7 };

        // Too small - then don't bother.
        if (width < 100 || height < 70) return;

        const QRectF rcTopLeft(0.0, 0.0, width, height);
        const QRectF rcTopRight(rc.right() - width, 0.0, width, height);
        const QRectF rcBottomLeft(0.0, rc.bottom() - height, width, height);
        const QRectF rcBottomRight(rc.right() - width, rc.bottom() - height, width, height);

        constexpr qreal extraZoom = 2.5;

        //
        // Draw the corner sections of the image at an increased zoom factor
        //
        painter.save();
        painter.scale(extraZoom * m_zoom * m_scale, extraZoom * m_zoom * m_scale);

        QRectF sourceRect;
        QPointF where;
        //
        // Top left rectangle
        //
        sourceRect = QRectF(
            0.0,
            0.0,
            width / (m_scale * extraZoom * m_zoom),
            height / (m_scale * extraZoom * m_zoom)
        );
        where = QPointF(0.0, 0.0);
        painter.drawPixmap(where, *pPixmap, sourceRect);
        painter.restore();

        //
        // Top right rectangle
        //
        painter.save();
        where = QPointF((rc.right() - width), 0.0);
        painter.translate(where);
        painter.scale(extraZoom * m_zoom * m_scale, extraZoom * m_zoom * m_scale);
        painter.translate(-where);
        sourceRect = QRectF(
            pPixmap->width() - (width / (m_scale * extraZoom * m_zoom)),
            0.0,
            width / (m_scale * extraZoom * m_zoom),
            height / (m_scale * extraZoom * m_zoom)
        );
        painter.drawPixmap(where, *pPixmap, sourceRect);
        painter.restore();

        //
        // Bottom left rectangle
        //
        painter.save();
        where = QPointF(0.0, (rc.bottom() - height));
        painter.translate(where);
        painter.scale(extraZoom * m_zoom * m_scale, extraZoom * m_zoom * m_scale);
        painter.translate(-where);
        sourceRect = QRectF(
            0.0,
            pPixmap->height() - (height / (m_scale * extraZoom * m_zoom)),
            width / (m_scale * extraZoom * m_zoom),
            height / (m_scale * extraZoom * m_zoom)
        );
        painter.drawPixmap(where, *pPixmap, sourceRect);
        painter.restore();

        //
        // Bottom right rectangle
        //
        painter.save();
        where = QPointF((rc.right() - width), (rc.bottom() - height));
        painter.translate(where);
        painter.scale(extraZoom * m_zoom * m_scale, extraZoom * m_zoom * m_scale);
        painter.translate(-where);
        sourceRect = QRectF(
            pPixmap->width() - (width / (m_scale * extraZoom * m_zoom)),
            pPixmap->height() - (height / (m_scale * extraZoom * m_zoom)),
            width / (m_scale * extraZoom * m_zoom),
            height / (m_scale * extraZoom * m_zoom)
        );
        painter.drawPixmap(where, *pPixmap, sourceRect);
        painter.restore();

        QPen pen(Qt::white, 2.0);
        painter.setPen(pen);
        painter.drawRect(rcTopLeft);
        painter.drawRect(rcTopRight);
        painter.drawRect(rcBottomLeft);
        painter.drawRect(rcBottomRight);
    }

    void ImageView::paintZoomImage(QPainter& painter)
    {
        constexpr qreal extraZoom = 5.0;
        const qreal diameter = 176.0;

        const QPoint globalMouseLocation(QCursor::pos());
        const QPointF mouseLocation(mapFromGlobal(globalMouseLocation));

        //
        // If the mouse isn't over theis control there's nothing to do, if it is over the control, then
        // if the mouse is not over the image or mouse is over the toolbar, then there's nothing to do
        //
        if (!underMouse() || !displayRect.contains(mouseLocation) || (m_pToolBar && m_pToolBar->underMouse())) return;

        //QPoint where{ static_cast<int>(mouseLocation.x() - diameter / 2), static_cast<int>(mouseLocation.y() - diameter / 2) };
        QPoint where(std::max(0.0, mouseLocation.x() - diameter), 0);
        int my = mouseLocation.y();
        if (my < 10 + diameter)
        {
            where.setY(my + 30);
        }
        else
        {
            where.setY(my - (10 + diameter));
        }

        const QRect rect(where, QSize(diameter, diameter));
        //
        // Create a circular clipping region on the rectangle
        //
        const QRegion region(rect, QRegion::Ellipse);
        painter.setClipRegion(region);

        // Fill this region with grey
        QPalette palette{ QGuiApplication::palette() };
        QBrush brush{ palette.dark() };
        painter.fillRect(rect, brush);

        auto pointOfInterest(screenToImage(mouseLocation));

        const qreal x = pointOfInterest.x();
        const qreal y = pointOfInterest.y();
        qreal xloc = x - (diameter / (m_scale * m_zoom * extraZoom) / 2.0);
        qreal yloc = y - (diameter / (m_scale * m_zoom * extraZoom) / 2.0);

        QRectF  sourceRect(
            xloc,
            yloc,
            diameter / (m_scale * m_zoom * extraZoom),
            diameter / (m_scale * m_zoom * extraZoom)
        );

        painter.save();
        painter.translate(where);
        painter.scale(m_scale * m_zoom * extraZoom, m_scale * m_zoom * extraZoom);
        painter.translate(-where);

        //
        // Finally draw the rectangle of interest at the origin location
        //
        painter.drawPixmap(where, *pPixmap, sourceRect);
        painter.restore();

        painter.setClipping(false);
        painter.setRenderHint(QPainter::RenderHint::Antialiasing);

        //
        // Draw a white circle around the zoomed area
        QPen pen(Qt::white, 2);
        painter.setPen(pen);
        painter.drawEllipse(rect);

        //
        // Draw cross-hairs in the centre
        // 
        pen.setColor(qRgba(255, 0, 0, 170));  // Red semi transparent
        pen.setWidth(2.0);
        painter.setPen(pen);
        painter.drawLine(
            rect.left() + rect.width() / 2 - 10,
            rect.top() + rect.height() / 2,
            rect.left() + rect.width() / 2 - 3,
            rect.top() + rect.height() / 2
        );
        painter.drawLine(
            rect.left() + rect.width() / 2 + 10,
            rect.top() + rect.height() / 2,
            rect.left() + rect.width() / 2 + 3,
            rect.top() + rect.height() / 2
        );
        painter.drawLine(
            rect.left() + rect.width() / 2,
            rect.top() + rect.height() / 2 - 10,
            rect.left() + rect.width() / 2,
            rect.top() + rect.height() / 2 - 3
        );
        painter.drawLine(
            rect.left() + rect.width() / 2,
            rect.top() + rect.height() / 2 + 10,
            rect.left() + rect.width() / 2,
            rect.top() + rect.height() / 2 + 3
        );

    }

    void  ImageView::clearOverlay()
    {
        pOverlayPixmap.reset(nullptr);
        drawOnPixmap();
        update();
    }

    void ImageView::clear()
    {
        //
        // delete the images
        //
        pPixmap.reset(nullptr);
        pOverlayPixmap.reset(nullptr);
        if (m_pToolBar)
        {
            m_pToolBar->setVisible(false);
            m_pToolBar->setEnabled(false);
        }
        drawOnPixmap();
        update();
    }

    void ImageView::setPixmap(const QPixmap& p)
    {
        pPixmap.reset(new QPixmap(p));
        drawOnPixmap();
        update();
    }

#if (0)
    void ImageView::setOverlayPixmap(const QPixmap& p)
    {
        pOverlayPixmap.reset(new QPixmap(p));
        drawOnPixmap();
        update();
    }
#endif


#if QT_CONFIG(wheelevent)
    void ImageView::wheelEvent(QWheelEvent* e)
    {
        //
        // This use to read degrees = -e->angleDelta().y() / 8.0; 
        // 
        // Discussion on the mailing list concluded the this resulted in mousewheel
        // zoom operated the "wrong way" compared to other applications
        // 
        const qreal degrees = e->angleDelta().y() / 8.0;
        const qreal steps = degrees / 60.0;

        //
        // Zoom in on the pixmap
        //
        zoom(e->position(), steps);

        Inherited::wheelEvent(e);
    }
#endif

    void ImageView::zoom(const QPointF& mouseLocation, qreal steps)
    {
        if (0.0 == steps || nullptr == pPixmap) return;

        //
        // Default the POI to the image centre
        //
        QPointF pointOfInterest(pPixmap->rect().center());

        //
        // If the mouse is over the image, determine where it is in the
        // zoomed image (the point of interest).
        //
        if (displayRect.contains(mouseLocation))
        {
            pointOfInterest = screenToImage(mouseLocation);
        }

        //
        // calculate the new zoom level
        //
        const qreal factor = m_zoom * std::pow(1.125, steps);
        m_zoom = std::clamp(factor, 1.0, MAX_ZOOM);

        //
        // Calculate the rectangle we're interested in that is centred on
        // the point of interest
        //
        const qreal width = pPixmap->width();
        const qreal height = pPixmap->height();
        const qreal x = pointOfInterest.x();
        const qreal y = pointOfInterest.y();
        rectOfInterest = QRectF(
            x - (x / m_zoom),
            y - (y / m_zoom),
            width / m_zoom,
            height / m_zoom
        );

        //
        // Now take the intersection of the rectangle of interest and the pixmap's rectangle.
        //
        rectOfInterest &= pPixmap->rect();

        //
        // if the user has updated the point of interest, we need to re-position the
        // mouse pointer to the position on the screen that matches the POI
        //
        QPointF newMouseLocation = imageToScreen(pointOfInterest);
        QCursor::setPos(mapToGlobal(newMouseLocation.toPoint()));

        drawOnPixmap();
        update();
    }

    bool ImageView::event(QEvent* event)
    {
        if (nullptr != pPixmap && QEvent::ToolTip == event->type())
        {
            //
            // If the mouse is over the image, but not over the toolbar,
            // get the tooltip text and if there is any, display it
            //
            const QPoint globalMouseLocation(QCursor::pos());
            const QPointF mouseLocation(mapFromGlobal(globalMouseLocation));
            if (displayRect.contains(mouseLocation) && (m_pToolBar && !m_pToolBar->underMouse()))
            {
                const QString tip = toolTip();
                if (!tip.isEmpty() && m_tipShowCount % 25 == 0)
                {
                    QToolTip::showText(globalMouseLocation, tip, this);
                }
                m_tipShowCount++;
            }
            return true;
        }
        // Make sure the rest of events are handled
        return Inherited::event(event);
    }

    void ImageView::keyPressEvent(QKeyEvent* e)
    {
        const QPointF mouseLocation(mapFromGlobal(QCursor::pos()));
        bool handled = false;
        switch (e->key())
        {
        case Qt::Key_Plus:
        case Qt::Key_Equal:
            // Handle Ctrl+= and Ctrl++ as Zoom in 
            if (e->modifiers() & Qt::ControlModifier)
            {
                zoom(mouseLocation, 1);
                handled = true;
            }
            break;
        case Qt::Key_Minus:
            // Handle Ctrl+- as Zoom out
            if (e->modifiers() & Qt::ControlModifier)
            {
                zoom(mouseLocation, -1);
                handled = true;
            }
            break;
        case Qt::Key_4:
            // Handle Ctrl+4 to toggle Four Corners mode
            if (e->modifiers() & Qt::ControlModifier)
            {
                on_fourCorners_clicked(true);
                 handled = true;
            }
            break;
        default:
            break;
        }

        if (!handled)
            Inherited::keyPressEvent(e);
    }
}