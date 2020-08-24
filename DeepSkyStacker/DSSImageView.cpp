
/****************************************************************************
**
** Copyright (C) 2020 David C. Partridge
* **
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
**   * Neither the name of The Qt Company Ltd nor the names of its
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
#include <algorithm>
#include <cmath>
#include <iostream>

#include "DSSImageView.h"

#include <QCursor>
#include <QDebug>
#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#if QT_CONFIG(wheelevent)
#include <QWheelEvent>
#endif

constexpr int alpha = 155;

DSSImageView::DSSImageView(QPixmap& p, QWidget* parent)
    : QWidget(parent),
    m_scale(1.0),
    m_zoom(1.0),
    m_origin(0.0, 0.0),
    m_pixmap(p),
    m_pointInPixmap((m_pixmap.width() / 2), (m_pixmap.height() / 2)),
    m_fourCorners(false)
{
    setBackgroundRole(QPalette::Dark);      // A darker background please
    setAutoFillBackground(true);
    setAttribute(Qt::WA_MouseTracking);
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void DSSImageView::mousePressEvent(QMouseEvent* e)
{
    emit Image_mousePressEvent(e);
}

void DSSImageView::resizeEvent(QResizeEvent* e)
{
    const QSize sz = e->size();
    m_drawingPixmap = QPixmap(sz);  // this is what we draw onto

    const qreal pixWidth = m_pixmap.width();
    const qreal pixHeight = m_pixmap.height();
    const qreal hScale = static_cast<qreal>(sz.width()) / pixWidth;
    const qreal vScale = static_cast<qreal>(sz.height()) / pixHeight;
    m_scale = std::min(hScale, vScale);

    qreal xoffset = 0.0, yoffset = 0.0;
    if ((pixWidth * m_scale) < sz.width())
    {
        xoffset = (sz.width() - (pixWidth * m_scale)) / 2.0;

    }
    if ((pixHeight * m_scale) < sz.height())
    {
        yoffset = (sz.height() - (pixHeight * m_scale)) / 2.0;
    }
    m_origin = QPointF(xoffset, yoffset);

    update();
    Inherited::resizeEvent(e);
}

void DSSImageView::paintEvent(QPaintEvent* event)
{
    QPainter pixPainter(&m_drawingPixmap);
    pixPainter.begin(this);
    pixPainter.setRenderHint(QPainter::Antialiasing);
    pixPainter.setRenderHint(QPainter::SmoothPixmapTransform);
    pixPainter.eraseRect(rect());

    //
    // Calculate the rectangle we're interested in that is centred on the point in the pixmap
    // that is under the mouse pointer.  Need to adjust the origin position depending on the 
    // zoom factor
    //
    const qreal width = m_pixmap.width();
    const qreal height = m_pixmap.height();
    const qreal x = m_pointInPixmap.x();
    const qreal y = m_pointInPixmap.y();
    QRectF  sourceRect(
        x - (x / m_zoom),
        y - (y / m_zoom),
        width / m_zoom,
        height / m_zoom
    );

    //
    // Now take the intersection of the rectangle of interest and the pixmap's rectangle.
    //
    sourceRect &= m_pixmap.rect();

    pixPainter.save();
    pixPainter.translate(m_origin);
    pixPainter.scale(m_zoom * m_scale, m_zoom * m_scale);
    pixPainter.translate(-m_origin);

    //
    // Finally draw the clipped rectangle of interest at the origin location
    //
    pixPainter.drawPixmap(m_origin, m_pixmap, sourceRect);
    pixPainter.restore();

    //
    // Has the user enabled four corners mode?
    // 
    if (m_fourCorners)
    {
        paintFourCorners(pixPainter);
    }
    else
    {
        // do the overlay image
    }

    pixPainter.end();

    QPainter painter(this);
    painter.drawPixmap(0, 0, m_drawingPixmap);
    painter.end();
}

void DSSImageView::paintFourCorners(QPainter& painter)
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

    constexpr qreal extraZoom = 2.0;

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
    painter.drawPixmap(where, m_pixmap, sourceRect);
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
        m_pixmap.width() - (width / (m_scale * extraZoom * m_zoom)),
        0.0,
        width / (m_scale * extraZoom * m_zoom),
        height / (m_scale * extraZoom * m_zoom)
    );
    painter.drawPixmap(where, m_pixmap, sourceRect);
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
        m_pixmap.height() - (height / (m_scale * extraZoom * m_zoom)),
        width / (m_scale * extraZoom * m_zoom),
        height / (m_scale * extraZoom * m_zoom)
    );
    painter.drawPixmap(where, m_pixmap, sourceRect);
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
        m_pixmap.width() - (width / (m_scale * extraZoom * m_zoom)),
        m_pixmap.height() - (height / (m_scale * extraZoom * m_zoom)),
        width / (m_scale * extraZoom * m_zoom),
        height / (m_scale * extraZoom * m_zoom)
    );
    painter.drawPixmap(where, m_pixmap, sourceRect);
    painter.restore();

    QPen pen(Qt::white, 2.0);
    painter.setPen(pen);
    painter.drawRect(rcTopLeft);
    painter.drawRect(rcTopRight);
    painter.drawRect(rcBottomLeft);
    painter.drawRect(rcBottomRight);
}

void DSSImageView::setPixmap(const QPixmap& p)
{
    m_pixmap = p;
}

#if QT_CONFIG(wheelevent)
void DSSImageView::wheelEvent(QWheelEvent* e)
{
    const qreal degrees = -e->angleDelta().y() / 8.0;
    const qreal steps = degrees / 60.0;

    //
    // Zoom in on the pixmap
    //
    zoom(e->position(), steps);

    Inherited::wheelEvent(e);
}
#endif

void DSSImageView::zoom(QPointF mouseLocation, qreal steps)
{
    //
    // If zooming in and zoom factor is currently 1.0
    // then remember mouse location
    //
    if ((steps > 0) && (m_zoom == 1.0))
    {
        if (mouseOverImage(mouseLocation))
        {
            m_pointInPixmap = QPointF((mouseLocation - m_origin) / m_scale);
        }
        else
        {
            m_pointInPixmap = QPointF((m_pixmap.width() / 2), (m_pixmap.height() / 2));
        }
    }

    const qreal factor = m_zoom * std::pow(1.125, steps);
    m_zoom = std::clamp(factor, 1.0, 5.0);

    //
    // If zooming out and we're back to non-zoomed, set default location
    // 
    if (steps < 0 && m_zoom == 1.0)
    {

        m_pointInPixmap = QPointF((m_pixmap.width() / 2), (m_pixmap.height() / 2));
    }
    update();
}

void DSSImageView::keyPressEvent(QKeyEvent* e)
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
            if (m_fourCorners)
                m_fourCorners = false;
            else
                m_fourCorners = true;
            update();
            handled = true;
        }
        break;
    default:
        break;
    }

    if (!handled)
        Inherited::keyPressEvent(e);
}

