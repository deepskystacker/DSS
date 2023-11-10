#pragma once
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
class QKeyEvent;
class QMouseEvent;
class QResizeEvent;
class QRubberBand;
class QToolBar;
class QWheelEvent;

namespace DSS
{
    class EditStars;
    class SelectRect;

    class ImageView : public QWidget
    {
        friend class SelectRect;
        friend class EditStars;
        Q_OBJECT

            typedef QWidget
            Inherited;
    public:
        ImageView(QWidget* parent = Q_NULLPTR);
        QSize sizeHint() const noexcept override { return QSize(900, 600); };
        inline void setToolBar(QToolBar* p) noexcept { m_pToolBar = p; };
        inline qreal scale() { return m_scale; }
        inline qreal zoom() { return m_zoom; }

        inline QPointF imageToScreen(const QPointF& pt) noexcept
        {
            return (pt * m_zoom * m_scale) + m_origin - (rectOfInterest.topLeft() * m_zoom * m_scale);
        };

        inline void	imageToScreen(qreal& fX, qreal& fY) noexcept
        {
            fX = (fX * m_zoom * m_scale) + m_origin.x() - (rectOfInterest.topLeft().x() * m_zoom * m_scale);
            fY = (fY * m_zoom * m_scale) + m_origin.y() - (rectOfInterest.topLeft().y() * m_zoom * m_scale);
        };

        inline QRectF imageToScreen(const QRectF& rc) noexcept
        {
            return QRectF(
                (rc.topLeft() * m_zoom * m_scale) + m_origin - (rectOfInterest.topLeft() * m_zoom * m_scale),
                rc.size() * m_zoom * m_scale
            );
        };

        inline qreal imageHeight() noexcept
        {
            return (nullptr == pPixmap) ? 0.0 : pPixmap->height();
        }

        inline qreal imageWidth() noexcept
        {
            return (nullptr == pPixmap) ? 0.0 : pPixmap->width();
        }

        inline QPointF screenToImage(const QPointF& pt) noexcept
        {
            return ((pt - m_origin) / (m_zoom * m_scale)) + rectOfInterest.topLeft();
        };

        inline void	screenToImage(qreal& fX, qreal& fY) noexcept
        {
            fX = (fX - m_origin.x()) / (m_zoom * m_scale) + rectOfInterest.topLeft().x();
            fY = (fY - m_origin.y()) / (m_zoom * m_scale) + rectOfInterest.topLeft().y();
        };

        inline QRectF screenToImage(const QRectF& rc)
        {
            return QRectF(
                (rc.topLeft() - m_origin) / (m_zoom * m_scale) + rectOfInterest.topLeft(),
                rc.size() / (m_zoom * m_scale)
            );
        };

        void  clearOverlay();

        void clear();

    public slots:
        void setPixmap(const QPixmap&);
        // void setOverlayPixmap(const QPixmap&);
        void on_fourCorners_clicked(bool)
        {
            m_fourCorners = !m_fourCorners;
            drawOnPixmap();
            update();
        }

    signals:
        void Image_leaveEvent(QEvent* e);
        void Image_mousePressEvent(QMouseEvent* e);
        void Image_mouseMoveEvent(QMouseEvent* e);
        void Image_mouseReleaseEvent(QMouseEvent* e);
        void Image_resizeEvent(QResizeEvent* e);

    protected:
        bool event(QEvent* event) override;
        void changeEvent(QEvent* e) override;
        void keyPressEvent(QKeyEvent* event) override;
        void leaveEvent(QEvent* event) override;
        void paintEvent(QPaintEvent* event) override;
        void mousePressEvent(QMouseEvent* e) override;
        void mouseMoveEvent(QMouseEvent* e) override;
        void mouseReleaseEvent(QMouseEvent* e) override;
        void resizeEvent(QResizeEvent* e) override;
#if QT_CONFIG(wheelevent)
        void wheelEvent(QWheelEvent*) override;
#endif

    private:
        qreal m_scale, m_zoom;
        QPointF m_origin;
        std::unique_ptr<QPixmap> pPixmap;
        std::unique_ptr<QPixmap> pOverlayPixmap;
        QPixmap m_drawingPixmap;
        QRectF rectOfInterest;
        QToolBar* m_pToolBar;
        QRectF displayRect;
        bool m_fourCorners;
        bool m_enableZoomImage;
        uint m_tipShowCount;
        void zoom(const QPointF& mouseLocation, qreal steps);
        void drawOnPixmap();
        void paintFourCorners(QPainter& painter);
        void paintZoomImage(QPainter& painter);
        void retranslateUi();
    };
}