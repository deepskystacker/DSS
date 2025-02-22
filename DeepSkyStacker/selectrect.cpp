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
#include "stdafx.h"
#include "selectrect.h"
#include "imageview.h"
#include "ProcessingDlg.h"
#include "StackingDlg.h"
#include "DeepSkyStacker.h"

enum class SelectionMode : quint8
{
    None = 0,
    Create = 1,
    Move = 2,
    MoveTop = 3,
    MoveBottom = 4,
    MoveLeft = 5,
    MoveRight = 6,
    MoveTopLeft = 7,
    MoveTopRight = 8,
    MoveBottomLeft = 9,
    MoveBottomRight = 10
};

namespace DSS
{
    SelectRect::SelectRect(QWidget* parent) :
        QWidget(parent),
        mode(SelectionMode::None),
        selecting(false),
        showDrizzle_(true)
    {
        imageView = dynamic_cast<ImageView*>(parent);
        ZASSERT(nullptr != imageView);
        setAttribute(Qt::WA_TransparentForMouseEvents);
        setAttribute(Qt::WA_NoSystemBackground);
        setAttribute(Qt::WA_WState_ExplicitShowHide);
     }

    /*!
        \reimp
    */
    void SelectRect::paintEvent(QPaintEvent*)
    {
        QPainter painter(this);
        QPen pen(Qt::red, 1.0);
        QPen drizzlePen(QColor(255, 0, 0, 128), 1.0);
        QRectF rect(imageView->imageToScreen(selectRect.normalized()));
        QFont font("Helvetica", 10, QFont::Bold);

        painter.setPen(pen);
        painter.setFont(font);
        //
        // If there's actually a selected area
        //
        if (!rect.isEmpty())
        {
            painter.drawRect(rect
                .adjusted(0, 0, -1, -1));
            QString text(QString("%1 x %2").
                arg(std::round(selectRect.width()), 0, 'f', 0).
                arg(std::round(selectRect.height()), 0, 'f', 0));

            if (selecting) painter.drawText(rect.left() + 2, rect.top() - 2, text);

            if (rect.height() > 30 && rect.width() > 30)
            {
                //
                // add a cross hair at the middle.
                //
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
        }

        //
        // If a selection is in progress and the operation is not a move (in which case the drizzle rectangles are empty)
        // then we need to draw the 2x and 3x Drizzle rectangles.
        //
        QRectF rect2x, rect3x;
        getDrizzleRectangles(rect2x, rect3x);
        painter.setPen(drizzlePen);
        QFontMetrics fontMetrics(font);
        QSize size;

        if (!rect2x.isEmpty() && selecting)
        {
            rect = imageView->imageToScreen(rect2x.normalized());
            painter.drawRect(rect
                .adjusted(0, 0, -1, -1));
            size = fontMetrics.size(0, x2Text);
            painter.drawText(rect.right() - ((qreal)size.width() + 2), rect.bottom() - 2, x2Text);
        }

        if (!rect3x.isEmpty() && selecting)
        {
            rect = imageView->imageToScreen(rect3x.normalized());
            painter.drawRect(rect
                .adjusted(0, 0, -1, -1));
            size = fontMetrics.size(0, x3Text);
            painter.drawText(rect.right() - ((qreal)size.width() + 2), rect.bottom() - 2, x3Text);
        }

    }

    /*!
        \reimp
    */
    void SelectRect::changeEvent(QEvent* e)
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

    /*!
        \reimp
    */
    void SelectRect::showEvent(QShowEvent* e)
    {
        resize(imageView->size());
        raise();
        Inherited::showEvent(e);
    }

    void SelectRect::mousePressEvent(QMouseEvent* e)
    {
        if (Qt::LeftButton == e->button())
        {
            //
            // Determine what we're doing based on pointer location
            //
            mode = modeFromPosition(e->pos());
            if (mode == SelectionMode::None) mode = SelectionMode::Create;

            //
            // Remember the location at which the mouse button was pressed.
            //
            startPos = endPos = imageView->screenToImage(e->pos());

            //
            // Save the initial selection
            // 
            startRect = selectRect;

            updateSelection();
            selecting = true;
            update();

        }
    }

    void SelectRect::mouseMoveEvent(QMouseEvent* e)
    {
        Qt::CursorShape cursorShape(Qt::ArrowCursor);

        if (selecting)
        {
            cursorShape = cursorFromMode(mode);
            if (imageView->displayRect.contains(e->pos()) && Qt::LeftButton == (e->buttons() & Qt::LeftButton))
            {
                endPos = imageView->screenToImage(e->pos());
                updateSelection();
            }
        }
        else
        {
            SelectionMode _mode(modeFromPosition(e->pos()));
            cursorShape = cursorFromMode(_mode);
        }

        if (QCursor* cursor = QGuiApplication::overrideCursor();
            cursor != nullptr)
        {
            if (cursorShape == Qt::ArrowCursor)
            {
                QGuiApplication::restoreOverrideCursor();
            }
            else if (cursor->shape() != cursorShape)
            {
                QGuiApplication::changeOverrideCursor(cursorShape);
            }
        }
        else if (cursorShape != Qt::ArrowCursor)
        {
            QGuiApplication::setOverrideCursor(cursorShape);
        }
        update();
    }

    void SelectRect::mouseReleaseEvent(QMouseEvent* e)
    {
        if (Qt::LeftButton == e->button())
        {
            if (imageView->displayRect.contains(e->pos()))
            {
                endPos = imageView->screenToImage(e->pos());
                updateSelection();
            }

            if (selectRect.width() <= 2 || selectRect.height() <= 2)
                selectRect.setSize(QSizeF(0.0, 0.0));

            selectRect = selectRect.normalized();
            if (!selectRect.isEmpty())
            {
                QRectF screenRect(imageView->imageToScreen(selectRect));
            }
            if (QGuiApplication::overrideCursor())
            {
                QGuiApplication::restoreOverrideCursor();
            }
            selecting = false;
            mode = SelectionMode::None;
        }
        update();
    }

    void SelectRect::resizeMe(QResizeEvent* e)
    {
        resize(e->size());
    }

    void SelectRect::rectButtonPressed()
    {
        constexpr auto connectionType = static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection);

        connect(imageView, &ImageView::Image_mousePressEvent, this, &SelectRect::mousePressEvent, connectionType);
        connect(imageView, &ImageView::Image_mouseMoveEvent, this, &SelectRect::mouseMoveEvent, connectionType);
        connect(imageView, &ImageView::Image_mouseReleaseEvent, this, &SelectRect::mouseReleaseEvent, connectionType);
        connect(imageView, &ImageView::Image_resizeEvent, this, &SelectRect::resizeMe, connectionType);
        show();
        raise();
    }

    void SelectRect::starsButtonPressed()
    {
        //
        // No longer interested in signals from the imageView object
        //
        imageView->disconnect(this, nullptr);
        hide();
    }

    void SelectRect::cometButtonPressed()
    {
        //
        // No longer interested in signals from the imageView object
        //
        imageView->disconnect(this, nullptr);
        hide();
    }

    void SelectRect::saveButtonPressed()
    {
    }

    SelectionMode SelectRect::modeFromPosition(const QPointF& pos)
    {
        SelectionMode result(SelectionMode::None);

        QRectF  cornerRect;
        QRectF  screenRect(imageView->imageToScreen(selectRect));

        //
        // If we have a valid selection
        //
        if (!screenRect.isEmpty())
        {
            if (screenRect.contains(pos))
                result = SelectionMode::Move;

            //
            // Check the cursor position against each of the edges and corners of the selection rectangle in turn
            //

            // Top edge
            cornerRect.setLeft(screenRect.left());		cornerRect.setRight(screenRect.right());
            cornerRect.setTop(screenRect.top() - 1);	cornerRect.setBottom(screenRect.top() + 1);

            if (cornerRect.contains(pos))
                result = SelectionMode::MoveTop;

            // Bottom edge
            cornerRect.setLeft(screenRect.left());		cornerRect.setRight(screenRect.right());
            cornerRect.setTop(screenRect.bottom() - 1);	cornerRect.setBottom(screenRect.bottom() + 1);

            if (cornerRect.contains(pos))
                result = SelectionMode::MoveBottom;

            // Left edge
            cornerRect.setLeft(screenRect.left() - 1);	cornerRect.setRight(screenRect.left() + 1);
            cornerRect.setTop(screenRect.top());		cornerRect.setBottom(screenRect.bottom());

            if (cornerRect.contains(pos))
                result = SelectionMode::MoveLeft;

            // Right edge
            cornerRect.setLeft(screenRect.right() - 1);	cornerRect.setRight(screenRect.right() + 1);
            cornerRect.setTop(screenRect.top());		cornerRect.setBottom(screenRect.bottom());

            if (cornerRect.contains(pos))
                result = SelectionMode::MoveRight;

            // Top left corner
            cornerRect.setLeft(screenRect.left() - 1);	cornerRect.setRight(screenRect.left() + 3);
            cornerRect.setTop(screenRect.top() - 1);	cornerRect.setBottom(screenRect.top() + 3);

            if (cornerRect.contains(pos))
                result = SelectionMode::MoveTopLeft;

            // Top right corner
            cornerRect.setLeft(screenRect.right() - 3);	cornerRect.setRight(screenRect.right() + 1);
            cornerRect.setTop(screenRect.top() - 1);	cornerRect.setBottom(screenRect.top() + 3);

            if (cornerRect.contains(pos))
                result = SelectionMode::MoveTopRight;

            // Bottom left corner
            cornerRect.setLeft(screenRect.left() - 1);	cornerRect.setRight(screenRect.left() + 3);
            cornerRect.setTop(screenRect.bottom() - 3);	cornerRect.setBottom(screenRect.bottom() + 1);

            if (cornerRect.contains(pos))
                result = SelectionMode::MoveBottomLeft;

            // Bottom right corner
            cornerRect.setLeft(screenRect.right() - 3);	cornerRect.setRight(screenRect.right() + 1);
            cornerRect.setTop(screenRect.bottom() - 3);	cornerRect.setBottom(screenRect.bottom() + 1);

            if (cornerRect.contains(pos))
                result = SelectionMode::MoveBottomRight;
        };

        return result;
    }

    Qt::CursorShape SelectRect::cursorFromMode(SelectionMode _mode)
    {
        Qt::CursorShape result = Qt::ArrowCursor;

        switch (_mode)
        {
        case SelectionMode::Move:
            result = Qt::SizeAllCursor;
            break;

        case SelectionMode::MoveTop:
        case SelectionMode::MoveBottom:
            result = Qt::SizeVerCursor;
            break;

        case SelectionMode::MoveLeft:
        case SelectionMode::MoveRight:
            result = Qt::SizeHorCursor;
            break;

        case SelectionMode::MoveTopLeft:
        case SelectionMode::MoveBottomRight:
            result = Qt::SizeFDiagCursor;
            break;

        case SelectionMode::MoveTopRight:
        case SelectionMode::MoveBottomLeft:
            result = Qt::SizeBDiagCursor;
            break;
        default:
            break;
        }
        return result;
    }

    void SelectRect::updateSelection()
    {
        qreal   deltaX = endPos.x() - startPos.x();
        qreal	deltaY = endPos.y() - startPos.y();
        qreal   temp = 0.0;

        //
        // Set back to start location 
        // 
        selectRect = startRect;

        switch (mode)
        {
        case SelectionMode::Create:
            selectRect.setLeft(std::max(std::min(startPos.x(), endPos.x()), 0.0));
            selectRect.setRight(std::min(std::max(startPos.x(), endPos.x()), imageView->imageWidth()));
            selectRect.setTop(std::max(std::min(startPos.y(), endPos.y()), 0.0));
            selectRect.setBottom(std::min(std::max(startPos.y(), endPos.y()), imageView->imageHeight()));
            break;
        case SelectionMode::Move:
            if (deltaX < 0) // Move to the left - check left first
            {
                if (-deltaX > selectRect.left() - 0.0)
                    deltaX = -(selectRect.left() - 0.0);
            }
            else // Move to the right - check right first
            {
                if (deltaX > imageView->imageWidth() - selectRect.right())
                    deltaX = imageView->imageWidth() - selectRect.right();
            };

            if (deltaY < 0) // Move to the top - check top first
            {
                if (-deltaY > selectRect.top() - 0.0)
                    deltaY = -(selectRect.top() - 0.0);
            }
            else // Move to the bottom - check bottom first
            {
                if (deltaY > imageView->imageHeight() - selectRect.bottom())
                    deltaY = imageView->imageHeight() - selectRect.bottom();
            };
            selectRect.translate(deltaX, deltaY);
            break;
        case SelectionMode::MoveTop:
            // Only deltaY is used
            if (deltaY > selectRect.height() - 1)
                deltaY = selectRect.height() - 1;
            temp = std::max(0.0, selectRect.top() + deltaY);
            selectRect.setTop(temp);
            break;
        case SelectionMode::MoveBottom:
            // Only deltaY is used
            if (-deltaY > selectRect.height() - 1)
                deltaY = -selectRect.height() + 1;
            temp = std::min(imageView->imageHeight(), selectRect.bottom() + deltaY);
            selectRect.setBottom(temp);
            break;
        case SelectionMode::MoveLeft:
            // Only deltaX is used
            if (deltaX > selectRect.width() - 1)
                deltaX = selectRect.width() - 1;
            temp = std::max(0.0, selectRect.left() + deltaX);
            selectRect.setLeft(temp);
            break;
        case SelectionMode::MoveRight:
            // Only deltaX is used
            if (-deltaX > selectRect.width() - 1)
                deltaX = -selectRect.width() + 1;
            temp = std::min(imageView->imageWidth(), selectRect.right() + deltaX);
            selectRect.setRight(temp);
            break;
        case SelectionMode::MoveTopLeft:
            // Both deltaX and deltaY are used
            if (deltaX > selectRect.width() - 1)
                deltaX = selectRect.width() - 1;
            temp = std::max(0.0, selectRect.left() + deltaX);
            selectRect.setLeft(temp);

            if (deltaY > selectRect.height() - 1)
                deltaY = selectRect.height() - 1;
            temp = std::max(0.0, selectRect.top() + deltaY);
            selectRect.setTop(temp);

            break;
        case SelectionMode::MoveTopRight:
            // Both deltaX and deltaY are used
            if (-deltaX > selectRect.width() - 1)
                deltaX = -selectRect.width() + 1;
            temp = std::min(imageView->imageWidth(), selectRect.right() + deltaX);
            selectRect.setRight(temp);

            if (deltaY > selectRect.height() - 1)
                deltaY = selectRect.height() - 1;
            temp = std::max(0.0, selectRect.top() + deltaY);
            selectRect.setTop(temp);

            break;
        case SelectionMode::MoveBottomLeft:
            // Both deltaX and deltaY are used
            if (deltaX > selectRect.width() - 1)
                deltaX = selectRect.width() - 1;
            temp = std::max(0.0, selectRect.left() + deltaX);
            selectRect.setLeft(temp);

            if (-deltaY > selectRect.height() - 1)
                deltaY = -selectRect.height() + 1;
            temp = std::min(imageView->imageHeight(), selectRect.bottom() + deltaY);
            selectRect.setBottom(temp);
            break;
        case SelectionMode::MoveBottomRight:
            // Both deltaX and deltaY are used
            if (-deltaX > selectRect.width() - 1)
                deltaX = -selectRect.width() + 1;
            temp = std::min(imageView->imageWidth(), selectRect.right() + deltaX);
            selectRect.setRight(temp);

            if (-deltaY > selectRect.height() - 1)
                deltaY = -selectRect.height() + 1;
            temp = std::min(imageView->imageHeight(), selectRect.bottom() + deltaY);
            selectRect.setBottom(temp);
            break;
        };

        QRectF	rect2xDrizzle, rect3xDrizzle;

        getDrizzleRectangles(rect2xDrizzle, rect3xDrizzle);

        if (!rect2xDrizzle.isEmpty())
        {
            // If the width/height is within 5 pixels - make it match
            if (labs(rect2xDrizzle.width() - selectRect.width()) < (5 / (imageView->zoom() * imageView->scale())))
            {
                selectRect.setLeft(rect2xDrizzle.left());
                selectRect.setRight(rect2xDrizzle.right());
            };
            if (labs(rect2xDrizzle.height() - selectRect.height()) < (5 / (imageView->zoom() * imageView->scale())))
            {
                selectRect.setTop(rect2xDrizzle.top());
                selectRect.setBottom(rect2xDrizzle.bottom());
            };
        };

        if (!rect3xDrizzle.isEmpty())
        {
            // If the width/height is within 5 pixels - make it match
            if (labs(rect3xDrizzle.width() - selectRect.width()) < (5 / (imageView->zoom() * imageView->scale())))
            {
                selectRect.setLeft(rect3xDrizzle.left());
                selectRect.setRight(rect3xDrizzle.right());
            };
            if (labs(rect3xDrizzle.height() - selectRect.height()) < (5 / (imageView->zoom() * imageView->scale())))
            {
                selectRect.setTop(rect3xDrizzle.top());
                selectRect.setBottom(rect3xDrizzle.bottom());
            };
        };
        emit selectRectChanged(selectRect);
    }

    void SelectRect::getDrizzleRectangles(QRectF& rect2xDrizzle, QRectF& rect3xDrizzle) noexcept
    {
        rect2xDrizzle.setLeft(0.0);  rect2xDrizzle.setTop(0.0);
        rect3xDrizzle.setLeft(0.0);  rect3xDrizzle.setTop(0.0);

        //
        // If the drizzle rectangles are required then calculate the necessary sizes
        // otherwise set the size to zero.
        //
        if (showDrizzle_)
        {
            rect2xDrizzle.setRight(imageView->imageWidth() / 2);
            rect2xDrizzle.setBottom(imageView->imageHeight() / 2);

            rect3xDrizzle.setRight(imageView->imageWidth() / 3);
            rect3xDrizzle.setBottom(imageView->imageHeight() / 3);

            switch (mode)
            {
            case SelectionMode::Create:
            case SelectionMode::MoveBottom:
            case SelectionMode::MoveRight:
            case SelectionMode::MoveBottomRight:
                // Upper left corner is used as anchor
                rect2xDrizzle.moveTo(selectRect.left(), selectRect.top());
                rect3xDrizzle.moveTo(selectRect.left(), selectRect.top());
                break;
            case SelectionMode::Move:
                // No need to show the drizzle rectangles
                rect2xDrizzle.setSize(QSizeF(0.0, 0.0));
                rect3xDrizzle.setSize(QSizeF(0.0, 0.0));
                break;
            case SelectionMode::MoveTop:
            case SelectionMode::MoveTopRight:
                // Lower left corner is used as anchor
                rect2xDrizzle.moveTo(selectRect.left(), selectRect.bottom() - rect2xDrizzle.height());
                rect3xDrizzle.moveTo(selectRect.left(), selectRect.bottom() - rect3xDrizzle.height());
                break;
            case SelectionMode::MoveLeft:
            case SelectionMode::MoveBottomLeft:
                // Upper right corner is used as anchor
                rect2xDrizzle.moveTo(selectRect.right() - rect2xDrizzle.width(), selectRect.top());
                rect3xDrizzle.moveTo(selectRect.right() - rect3xDrizzle.width(), selectRect.top());
                break;
            case SelectionMode::MoveTopLeft:
                // Lower right corner is used as anchor
                rect2xDrizzle.moveTo(selectRect.right() - rect2xDrizzle.width(), selectRect.bottom() - rect2xDrizzle.height());
                rect3xDrizzle.moveTo(selectRect.right() - rect3xDrizzle.width(), selectRect.bottom() - rect3xDrizzle.height());
                break;
            default:
                break;
            }
        }
        else
        {
            rect2xDrizzle.setBottom(0.0); rect2xDrizzle.setRight(0.0);
            rect3xDrizzle.setBottom(0.0); rect3xDrizzle.setRight(0.0);
        }

    }

    /*!
        \fn void SelectRect::setGeometry(const QRect &rect)

        Sets the geometry of the rubber band to \a rect, specified in the coordinate system
        of its parent widget.

        \sa QWidget::geometry
    */
    void SelectRect::setGeometry(const QRect& geom)
    {
        QWidget::setGeometry(geom);
    }

}