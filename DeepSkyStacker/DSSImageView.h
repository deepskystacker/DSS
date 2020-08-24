#pragma once
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
class QKeyEvent;
class QMouseEvent;
class QResizeEvent;
class QWheelEvent;

#include <QtWidgets/QWidget>

class DSSImageView : public QWidget
{
    Q_OBJECT

typedef QWidget
        Inherited;
public:
    DSSImageView(QPixmap& p, QWidget* parent = Q_NULLPTR);
    QSize sizeHint() const noexcept override { return QSize(500, 500); };

public slots:
    void setPixmap(const QPixmap&);

signals:
    void Image_mousePressEvent(QMouseEvent* e);

protected:

    void keyPressEvent(QKeyEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;
#if QT_CONFIG(wheelevent)
    void wheelEvent(QWheelEvent*) override;
#endif

private:
    qreal m_scale, m_zoom;
    QPointF m_origin;
    QPixmap& m_pixmap;
    QPixmap m_drawingPixmap;
    QPointF m_pointInPixmap;
    void zoom(QPointF mouseLocation, qreal steps);
    bool m_fourCorners;
    void paintFourCorners(QPainter& painter);

    inline bool mouseOverImage(QPointF loc)
    {
        const qreal x = loc.x(), y = loc.y(), ox = m_origin.x(), oy = m_origin.y();
        return (
            (x >= ox) &&
            (x <= ox + (m_pixmap.width() * m_scale)) &&
            (y >= oy) &&
            (y <= oy + (m_pixmap.height() * m_scale)));
    };

};

