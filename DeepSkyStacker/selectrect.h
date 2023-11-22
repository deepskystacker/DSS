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
class QMouseEvent;

enum class SelectionMode : quint8;

namespace DSS
{
    class ImageView;

    class SelectRect :
        public QWidget
    {
        Q_OBJECT

            typedef QWidget
            Inherited;

    public:
        explicit SelectRect(QWidget* parent);

        void setGeometry(const QRect& r);
        inline void reset()
        {
            selectRect = QRectF();
            emit selectRectChanged(selectRect);
        }
        inline void setShowDrizzle(bool show = true)
        {
            showDrizzle_ = show;
        };


    protected:
        void paintEvent(QPaintEvent*) override;
        void changeEvent(QEvent*) override;
        void showEvent(QShowEvent*) override;

    public slots:
        void mousePressEvent(QMouseEvent* e);
        void mouseMoveEvent(QMouseEvent* e);
        void mouseReleaseEvent(QMouseEvent* e);
        void resizeMe(QResizeEvent* e);

        void rectButtonPressed();
        void starsButtonPressed();
        void cometButtonPressed();
        void saveButtonPressed();
 
    signals:
        void selectRectChanged(QRectF rect);

    private:
        SelectionMode mode;
        ImageView* imageView;
        QRectF  selectRect;             // In image coordinates
        QRectF  startRect;              // In image coordinates
        bool    selecting;
        bool    showDrizzle_;            // To display the drizzle rectangles or not
        QPointF startPos;
        QPointF endPos;
        QRegion clipping;
        static inline QString x2Text{ "x2" };
        static inline QString x3Text{ "x3" };

        SelectionMode modeFromPosition(const QPointF&);
        Qt::CursorShape cursorFromMode(SelectionMode);

        void updateSelection();
        void getDrizzleRectangles(QRectF& rect2xDrizzle, QRectF& rect3xDrizzle) noexcept;
    };
}
