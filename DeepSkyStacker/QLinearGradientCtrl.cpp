// QLinearGradientCtrl.cpp 
//
#include <algorithm>
using std::min;
using std::max;

#include <QColor>
#include <QLinearGradient>
#include <QMouseEvent>
#include <QPainter>
#include <QPoint>
#include <QRect>
#include <QRegion>
#include <QWidget>

#include <Ztrace.h>

#include "QLinearGradientCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// QLinearGradientCtrl

enum SPECIALPEGS : short
{
	BACKGROUND = -4,
	STARTPEG = -3,
	ENDPEG = -2,
	NONE = -1
};

QLinearGradientCtrl::QLinearGradientCtrl(QWidget * parent) :
	QWidget(parent),
	m_Width(GCW_AUTO),
	selectedPeg(NONE),
	m_LastPos(0),
	m_showToolTips(true),
	m_Orientation(Auto),
	startPegStop(0),
	m_LeftDownSide(true),
	m_RightUpSide(false)
{
	m_ToolTipFormat = "&SELPOS\nPosition: &SELPOS Colour: R &R G &G B &B\nColour: R &R G &G B &B\nColour: R &R G &G B &B\nDouble Click to Add a New Peg";
	//m_Impl = new QLinearGradientCtrlImpl(this);

	setFocusPolicy(Qt::StrongFocus);		// Make sure we get key events.

	stops = m_Gradient.stops();				// Grab the gradient stops
	addAnchorStops();						// Add Anchor stops if not already present
}

void QLinearGradientCtrl::addAnchorStops()
{
	//
	// Insert additional stops at the start and end of the gradient that match the
	// existing start and end stops (unless of course they are already present).
	//
	// The new first and last will be used as the fixed anchor points (square pegs), while the 
	// second and first from last will be movable (triangular) pegs.
	// 
	// This only done at construction time or when the gradient is replaced by setGradient
	//
	if (stops[0] != stops[1])
		stops.push_front(stops[0]);
	auto last = stops.size() - 1;
	if (stops[last] != stops[last - 1])
		stops.push_back(stops[last]);

	m_Gradient.setStops(stops);
	endPegStop = last;
}

QLinearGradientCtrl & QLinearGradientCtrl::setGradient(QLinearGradient const& src)
{
	m_Gradient = src;
	stops = m_Gradient.stops();
	return *this;
}

QLinearGradientCtrl::~QLinearGradientCtrl()
{
	//delete m_Impl;
}

int QLinearGradientCtrl::getDrawWidth()
{
	return (m_Width == GCW_AUTO) ? (isVertical() ? clientRect.right()+1 :
		clientRect.bottom()+1) : m_Width;
}

QColor QLinearGradientCtrl::colourFromPoint(const QPoint & point)
{
	bool vertical = isVertical();
	int drawwidth = getDrawWidth();
	int w = drawwidth - (m_RightUpSide ? 24 : 5) - (m_LeftDownSide ? 24 : 5);

	ZASSERT((clientRect.bottom()+1) >= 11);

	int l = (vertical ? clientRect.bottom() + 1 : clientRect.right() + 1) - 10;

	QRect gradientRect;
	if (vertical)
		gradientRect = QRect((m_LeftDownSide ? 24 : 5), 5, w, l);
	else
		gradientRect = QRect(5, clientRect.bottom() + 1 - drawwidth + (m_RightUpSide ? 24 : 5), l, w);

	QPoint testPoint;
	if (vertical)
	{
		int yPos = point.y();
		if (yPos < (gradientRect.top() + 1)) yPos = gradientRect.top()+1;
		if (yPos > (gradientRect.bottom())) yPos = gradientRect.bottom();
		testPoint = QPoint(gradientRect.left() + 1, yPos);
	}
	else
	{
		int xPos = point.x();
		if (xPos < (gradientRect.left() + 1)) xPos = gradientRect.left()+1;
		if (xPos > (gradientRect.right())) xPos = gradientRect.right();
		testPoint = QPoint(gradientRect.top() + 1, xPos);
	}
	ZTRACE_RUNTIME("colourFromPoint testPoint x=%ld y=&ld", testPoint.x(), testPoint.y());
	QPixmap qPix = grab();
	QImage image(qPix.toImage());
	return(image.pixel(testPoint));
}

void QLinearGradientCtrl::drawGradient(QPainter & painter)
{
	bool vertical = isVertical();
	int drawwidth = getDrawWidth();
	int w = drawwidth - (m_RightUpSide ? 24 : 5) - (m_LeftDownSide ? 24 : 5);

	if (clientRect.bottom()+1 < 11) return;

	QBrush oldBrush = painter.brush();
	painter.setBrush(m_Gradient);

	int l = (vertical ? clientRect.bottom()+1 : clientRect.right()+1) - 10;

	QRect gradientRect;
	if (vertical)
		gradientRect = QRect((m_LeftDownSide ? 24 : 5), 5, w, l);
	else
		gradientRect = QRect(5, clientRect.bottom() + 1 - drawwidth + (m_RightUpSide ? 24 : 5), l, w);

	ZTRACE_RUNTIME("Gradient rectangle x=%ld, y=%ld, w=%ld, h=%ld",
		gradientRect.left(), gradientRect.top(),
		gradientRect.width(), gradientRect.height());

	painter.drawRect(gradientRect);
	painter.setBrush(oldBrush);

	//
	// Now draw a line around the gradient (maybe it's already done?)
	//

}

void QLinearGradientCtrl::drawEndPegs(QPainter & painter)
{
	//
	// Get foreground colour (window text) and create a pen to draw a one pixel boundary
	// in that colour
	//
	QColor	foreground(QPalette().color(QPalette::WindowText));	// get foreground colour 
	QPen	pen(foreground);
	painter.setPen(pen);

	//
	// Get the colour of the starting and ending stops and create brushes 
	//
	QBrush  startBrush(stops[startPegStop].second);
	QBrush	endBrush(stops[endPegStop].second);

	bool vertical = isVertical();
	int drawwidth = getDrawWidth();

	int x1 = 0, x2 = 0, y1 = 0, y2 = 0;

	//----- Draw the first marker -----//
	painter.setBrush(startBrush);
	if (m_RightUpSide)
	{
		if (vertical)
		{
			x1 = drawwidth - 15;
			y1 = 4;
			x2 = drawwidth - 8;
			y2 = 11;
		}
		else
		{
			x1 = 4;
			y1 = clientRect.bottom()+1 - drawwidth + 15;
			x2 = 11;
			y2 = clientRect.bottom()+1 - drawwidth + 8;
		}
		painter.drawRect(x1, y1, x2, y2);
	}
	if (m_LeftDownSide)
	{
		if (vertical)
		{
			x1 = 8;
			y1 = 4;
			x2 = 15;
			y2 = 11;
		}
		else
		{
			x1 = 4;
			y1 = clientRect.bottom()+1 - 8;
			x2 = 11;
			y2 = clientRect.bottom()+1 - 15;
		}
		painter.drawRect(x1, y1, x2, y2);
	}


	//----- Draw the last one -----//
	painter.setBrush(endBrush);

	if (m_RightUpSide)
	{
		if (vertical)
		{
			x1 = drawwidth - 15;
			y1 = clientRect.bottom()+1 - 4;
			x2 = drawwidth - 8;
			y2 = clientRect.bottom()+1 - 11;
		}
		else
		{
			x1 = clientRect.right()+1 - 4;
			y1 = clientRect.bottom()+1 - drawwidth + 8;
			x2 = clientRect.right()+1 - 11;
			y2 = clientRect.bottom()+1 - drawwidth + 15;
		}
		painter.drawRect(x1, y1, x2, y2);
	}
	if (m_LeftDownSide)
	{
		if (vertical)
		{
			x1 = 8;
			y1 = clientRect.bottom()+1 - 4;
			x2 = 15;
			y2 = clientRect.bottom()+1 - 11;
		}
		else
		{
			x1 = clientRect.right()+1 - 4;
			y1 = clientRect.bottom()+1 - 15;
			x2 = clientRect.right()+1 - 11;
			y2 = clientRect.bottom()+1 - 8;
		}
		painter.drawRect(x1, y1, x2, y2);
	}
}

void QLinearGradientCtrl::drawPegs(QPainter& painter)
{
	// No stupid selection
	if (selectedPeg > stops.size())
		selectedPeg = -1;

	int pegindent = 0;

	for (int i = 1; i < stops.size()-1; i++)	// ignore starting and ending stops
	{
		if (m_RightUpSide)
		{
			//Indent if close
			pegindent = getPegIndent(i) * 11 + getDrawWidth() - 23;

			//Obvious really
			if (isVertical())
				drawPeg(painter, QPoint(pegindent, pointFromPos(stops[i].first)), stops[i].second, 0);
			else
				drawPeg(painter, QPoint(pointFromPos(stops[i].first), clientRect.bottom()+1 - pegindent - 1), stops[i].second, 1);
		}

		if (m_LeftDownSide)
		{
			//Indent if close
			pegindent = 23 - getPegIndent(i) * 11;

			//Obvious really
			if (isVertical())
				drawPeg(painter, QPoint(pegindent, pointFromPos(stops[i].first)), stops[i].second, 2);
			else
				drawPeg(painter, QPoint(pointFromPos(stops[i].first), clientRect.bottom()+1 - pegindent - 1), stops[i].second, 3);
		}
	}
}

void QLinearGradientCtrl::drawPeg(QPainter & painter, QPoint point, QColor colour, int direction)
{
	QBrush brush(colour);     // create a brush in the requested colour
	QPoint points[3];

	painter.setBrush(brush);

	//
	// Get foreground colour (window text) and create a pen to draw a one pixel boundary
	// in that colour
	//
	QColor	foreground(QPalette().color(QPalette::WindowText));	// get foreground colour 
	QPen	pen(foreground);
	painter.setPen(pen);

	//Prepare the coodrdinates
	switch (direction)
	{
	case 0:
		points[0].rx() = point.x();
		points[0].ry() = point.y() + 1;
		points[1].rx() = point.x() + 9;
		points[1].ry() = point.y() - 3;
		points[2].rx() = point.x() + 9;
		points[2].ry() = point.y() + 5;
		break;
	case 1:
		points[0].rx() = point.x() + 1;
		points[0].ry() = point.y();
		points[1].rx() = point.x() - 3;
		points[1].ry() = point.y() - 9;
		points[2].rx() = point.x() + 5;
		points[2].ry() = point.y() - 9;
		break;
	case 2:
		points[0].rx() = point.x() - 1;
		points[0].ry() = point.y() + 1;
		points[1].rx() = point.x() - 10;
		points[1].ry() = point.y() - 3;
		points[2].rx() = point.x() - 10;
		points[2].ry() = point.y() + 5;
		break;
	default:
		points[0].rx() = point.x() + 1;
		points[0].ry() = point.y() + 1;
		points[1].rx() = point.x() - 3;
		points[1].ry() = point.y() + 10;
		points[2].rx() = point.x() + 5;
		points[2].ry() = point.y() + 10;
		break;
	}
	painter.drawPolygon(points, 3);

	//----- Draw lines manually in the right directions ------//
	//CPen outlinepen(PS_SOLID, 1, GetSysColor(COLOR_WINDOWTEXT));
	//oldpen = dc->SelectObject(&outlinepen);

	//dc->MoveTo(points[0]);
	//dc->LineTo(points[1]);
	//dc->LineTo(points[2]);
	//dc->LineTo(points[0]);

	//dc->SelectObject(oldpen);

	//brush.DeleteObject();
}

void QLinearGradientCtrl::drawSelPeg(QPainter & painter, QPoint point, int direction)
{
	QPoint points[3];

	//
	// Get foreground colour (window text) and create a pen to draw a one pixel boundary
	// in that colour
	//
	auto& oldPen = painter.pen();
	QColor	foreground(QPalette().color(QPalette::WindowText));	// get foreground colour 
	QPen	pen(foreground);
	painter.setPen(pen);

	auto& oldBrush = painter.brush();
	QBrush brush;			// create a null brush so background isn't filled
	painter.setBrush(brush);

	//
	// Preserve painter's composition mode  and set to QPainter::RasterOp_NotDestination
	//
	auto oldMode = painter.compositionMode();
	painter.setCompositionMode(QPainter::RasterOp_NotDestination);

	//Prepare the coodrdinates
	switch (direction)
	{
	case 0:
		points[0].rx() = 8 + point.x();
		points[0].ry() = point.y() - 2;
		points[1].rx() = 2 + point.x();
		points[1].ry() = point.y() + 1;
		points[2].rx() = 8 + point.x();
		points[2].ry() = point.y() + 4;
		break;
	case 1:
		points[0].rx() = point.x() - 2;
		points[0].ry() = point.y() - 8;
		points[1].rx() = point.x() + 1;
		points[1].ry() = point.y() - 2;
		points[2].rx() = point.x() + 4;
		points[2].ry() = point.y() - 8;
		break;
	case 2:
		points[0].rx() = point.x() - 9, points[0].ry() = point.y() - 2;
		points[1].rx() = point.x() - 3, points[1].ry() = point.y() + 1;
		points[2].rx() = point.x() - 9, points[2].ry() = point.y() + 4;
		break;
	default:
		points[0].rx() = point.x() - 2;
		points[0].ry() = point.y() + 8;
		points[1].rx() = point.x() + 1;
		points[1].ry() = point.y() + 2;
		points[2].rx() = point.x() + 4;
		points[2].ry() = point.y() + 8;
		break;
	}
	painter.drawPolygon(points, 3);

	//Restore the composition mode etc ...
	painter.setCompositionMode(oldMode);
	painter.setBrush(oldBrush);
	painter.setPen(oldPen);
}

void QLinearGradientCtrl::drawSelPeg(QPainter & painter, int peg)
{
	int drawwidth = getDrawWidth() - 23;
	bool vertical = isVertical();

	//
	// Get foreground colour (window text) and create a pen to draw a one pixel boundary
	// in that colour
	//
	auto& oldPen = painter.pen();
	QColor	foreground(QPalette().color(QPalette::WindowText));	// get foreground colour 
	QPen	pen(foreground);
	painter.setPen(pen);

	auto& oldBrush = painter.brush();
	QBrush brush;			// create a null brush so background isn't filled
	painter.setBrush(brush);

	//
	// Preserve painter's composition mode  and set to QPainter::RasterOp_NotDestination
	//
	auto oldMode = painter.compositionMode();
	painter.setCompositionMode(QPainter::RasterOp_NotDestination);

	//"Select objects"//
	if (peg == STARTPEG)
	{
		if (m_RightUpSide)
			if (vertical) 
				painter.drawRect(drawwidth + 9, 5, drawwidth + 14, 10);
			else 
				painter.drawRect(5, clientRect.bottom()+1 - drawwidth - 9,
				10, clientRect.bottom()+1 - drawwidth - 14);

		if (m_LeftDownSide)
			if (vertical)
				painter.drawRect(9, 5, 14, 10);
			else
				painter.drawRect(5, clientRect.bottom()+1 - 9, 10, clientRect.bottom()+1 - 14);

		return;
	}

	if (peg == ENDPEG)
	{
		if (m_RightUpSide)
			if (vertical)
				painter.drawRect(drawwidth + 9, clientRect.bottom()+1 - 10, drawwidth + 14, clientRect.bottom()+1 - 5);
			else
				painter.drawRect(clientRect.right()+1 - 10, clientRect.bottom()+1 - drawwidth - 9,
				clientRect.right()+1 - 5, clientRect.bottom()+1 - drawwidth - 14);

		if (m_LeftDownSide)
			if (vertical)
				painter.drawRect(9, clientRect.bottom()+1 - 5, 14, clientRect.bottom()+1 - 10);
			else
				painter.drawRect(clientRect.right()+1 - 5, clientRect.bottom()+1 - 9,
				clientRect.right()+1 - 10, clientRect.bottom()+1 - 14);
		return;
	}

	//Restore the composition mode etc ...
	painter.setCompositionMode(oldMode);
	painter.setBrush(oldBrush);
	painter.setPen(oldPen);

	if (peg > 0 && stops.size())
	{
		int pegindent = getPegIndent(peg) * 11;

		if (isVertical())
		{
			if (m_RightUpSide)
				drawSelPeg(painter, QPoint(pegindent + drawwidth, pointFromPos(stops[peg].first)), 0);
			if (m_LeftDownSide)
				drawSelPeg(painter, QPoint(23 - pegindent, pointFromPos(stops[peg].first)), 2);
		}
		else
		{
			if (m_RightUpSide)
				drawSelPeg(painter, QPoint(pointFromPos(stops[peg].first), clientRect.bottom()+1 - pegindent - drawwidth - 1), 1);
			if (m_LeftDownSide)
				drawSelPeg(painter, QPoint(pointFromPos(stops[peg].first), clientRect.bottom()+1 - 23 + pegindent), 3);
		}
	}

}

void QLinearGradientCtrl::resizeEvent(QResizeEvent *event)
{
	clientRect = contentsRect();
	Inherited::resizeEvent(event);
}

void QLinearGradientCtrl::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);

	// Draw it all here ...

	//
	// Erase the area that's been invalidated
	// 
	painter.eraseRect(event->rect());

	//----- Draw the Palette -----//
	drawGradient(painter);

	//----- Draw the marker arrows -----//
	drawEndPegs(painter);
	drawPegs(painter); //The order is important - Explanation: The function DrawSelPeg must be called last as the peg has already been drawn in normally by DrawPegs
	drawSelPeg(painter, selectedPeg);

	//----- Draw a box around the palette -----//
	//CBrush blackbrush(GetSysColor(COLOR_WINDOWTEXT));
	//if (IsVertical())
	//{

	//	dc->FrameRect(CRect(m_LeftDownSide ? 23 : 4, 4,
	//		getDrawWidth() - (m_RightUpSide ? 23 : 4),
	//		clientRect.bottom()+1 - 4), &blackbrush);
	//}
	//else
	//{
	//	dc->FrameRect(CRect(4, clientRect.bottom()+1 - getDrawWidth() + (m_RightUpSide ? 23 : 4),
	//		clientRect.right()+1 - 4, clientRect.bottom()+1 - (m_LeftDownSide ? 23 : 4)),
	//		&blackbrush);
	//}

	//----- Refresh -----//
	if (selectedPeg > stops.size())
		selectedPeg = NONE;

	//if (m_bShowToolTip)
	//	m_Impl->SynchronizeTooltips();
}

void QLinearGradientCtrl::mousePressEvent(QMouseEvent *event)
{
	if (Qt::LeftButton == event->buttons())
	{
		QPoint point = event->pos();
		QRect pegrect;

		QPainter painter(this);

		bool nowselected = false;

		m_LastPos = -1;

		//----- Just in case the user starts dragging -----//
		setFocus();
		setMouseTracking(true);
		m_MouseDown = point;

		//----- Check if the user is selecting a marker peg -----//
		for (int i = 1; i < stops.size() -1 ; i++) // Only the movable ones please
		{
			if (m_RightUpSide)
			{
				getPegRect(i, &pegrect, true);
				if (pegrect.contains(point))
				{
					setSelectedIndex(i);
					nowselected = true;
					break;
				}
			}

			if (m_LeftDownSide)
			{
				getPegRect(i, &pegrect, false);
				if (pegrect.contains(point))
				{
					setSelectedIndex(i);
					nowselected = true;
					break;
				}
			}
		}

		//----- Check if the user is trying to select the first or last one -----//

		if (m_RightUpSide)
		{
			getPegRect(STARTPEG, &pegrect, true);
			if (pegrect.contains(point))
			{
				setSelectedIndex(STARTPEG);
				nowselected = true;
			}

			getPegRect(ENDPEG, &pegrect, true);
			if (pegrect.contains(point))
			{
				setSelectedIndex(ENDPEG);
				nowselected = true;
			}
		}

		if (m_LeftDownSide)
		{
			getPegRect(STARTPEG, &pegrect, false);
			if (pegrect.contains(point))
			{
				setSelectedIndex(STARTPEG);
				nowselected = true;
			}

			getPegRect(ENDPEG, &pegrect, false);
			if (pegrect.contains(point))
			{
				setSelectedIndex(ENDPEG);
				nowselected = true;
			}
		}


		if (!nowselected)
		{
			drawSelPeg(painter, selectedPeg);
			selectedPeg = NONE;
			m_LastPos = -1;
		}

		emit pegSelChanged(selectedPeg);

		setFocus();
	}
	Inherited::mousePressEvent(event);
}

void QLinearGradientCtrl::mouseMoveEvent(QMouseEvent *event)
{
	if (Qt::LeftButton == event->buttons() && selectedPeg > -1)
	{
		QPoint point(event->pos());
		QPoint tippoint;
		QRegion oldRegion, newRegion, eraseRegion;
		//QString tiptext;
		bool vertical = isVertical();
		int selpegpos;

		//----- Prepare -----//
		float pos;

		//----- Checks to see if the mouse is far enough away to "drop out" -----//
		if (vertical)
		{
			if (max(point.x() - m_MouseDown.x(), m_MouseDown.x() - point.x()) <= 200)
				selpegpos = point.y();
			else
				selpegpos = m_MouseDown.y();
		}
		else
		{
			if (max(point.y() - m_MouseDown.y(), m_MouseDown.y() - point.y()) <= 200)
				selpegpos = point.x();
			else
				selpegpos = m_MouseDown.x();
		}
		// Get old region
		oldRegion = getPegRegion();

		//----- Continue -----//
		pos = posFromPoint(selpegpos);
		//"The id of the selection may change"//
		selectedPeg = setPeg(selectedPeg, stops[selectedPeg].second, pos);
		m_Gradient.setStops(stops);

		// Get new region
		newRegion = getPegRegion();

		//----- Get the region for the changed pegs and erase them -----//
		eraseRegion = oldRegion - newRegion;
		update(eraseRegion);			// Erase the old pegs.

		m_LastPos = selpegpos;

		//----- Show tooltip -----//
		//if (m_bShowToolTip)
		//{
		//	tiptext = m_Impl->ExtractLine(m_ToolTipFormat, 0);
		//	m_Impl->ParseToolTipLine(tiptext, peg);

		//	if (m_Impl->m_wndToolTip == nullptr)
		//	{
		//		if (m_Impl->IsVertical())
		//		{
		//			tippoint.y = point.y - 8;
		//			tippoint.x = m_Impl->getDrawWidth() + m_Impl->GetPegIndent(GetSelIndex()) * 11 - 7;
		//		}
		//		else
		//		{
		//			CRect clientrect;
		//			GetClientRect(&clientrect);
		//			tippoint.y = clientRect.bottom() + 1 - m_Impl->getDrawWidth() - m_Impl->GetPegIndent(GetSelIndex()) * 11 - 7;
		//			tippoint.x = point.x - 8;
		//		}

		//		ClientToScreen(&tippoint);

		//		m_Impl->ShowTooltip(tippoint, tiptext);
		//	}
		//	else
		//		m_Impl->SetTooltipText(tiptext);
		//}

		//----- Tell the world that the peg is moving -----//
		if (STARTPEG != selectedPeg && ENDPEG != selectedPeg)
			emit pegMove(selectedPeg);
	}

	Inherited::mouseMoveEvent(event);
}

void QLinearGradientCtrl::mouseReleaseEvent(QMouseEvent *event)
{
	if (Qt::LeftButton == event->buttons() && selectedPeg > -1)
	{
		if (selectedPeg >= STARTPEG && selectedPeg != NONE)
		{
			bool vertical = isVertical();
			//int selpegpos = vertical ? point.y-5 : point.x-5;

			if (vertical)
				update(QRect(getDrawWidth(), 0, clientRect.right()+1,
					clientRect.bottom()+1)); //Erase the old ones
			else
				update(QRect(0, 0, clientRect.right()+1,
					clientRect.bottom()+1 - getDrawWidth())); //Erase the old ones

			//m_Impl->DestroyTooltip();

			emit pegMoved(selectedPeg);		// emit the specific peg moved signal
			emit pegChanged(selectedPeg);	// emit the generic something changed signal
		}

		setMouseTracking(false);
	}
	Inherited::mouseReleaseEvent(event);
}

void QLinearGradientCtrl::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (Qt::LeftButton == event->buttons())
	{
		float pos;
		QPoint point(event->pos());
		QRect pegrect;
		bool edit = false;
		int drawwidth = getDrawWidth();

		if (isVertical())
			pos = posFromPoint(point.y());
		else
			pos = posFromPoint(point.x());

		//----- Check if the user is selecting a marker peg -----//
		for (int i = 1; i < stops.size()-1; i++)		// Only the movable pegs
		{
			if (m_RightUpSide)
			{
				getPegRect(i, &pegrect, true);
				if (pegrect.contains(point))
				{
					update(pegrect);
					setSelectedIndex(i);
					edit = true;
					break;
				}
			}

			if (m_LeftDownSide)
			{
				getPegRect(i, &pegrect, false);
				if (pegrect.contains(point))
				{
					update(pegrect);
					setSelectedIndex(i);
					edit = true;
					break;
				}
			}
		}


		//----- Check if the user is trying to select the first or last one -----//

		pegrect.setLeft(drawwidth + 8), pegrect.setTop(4);
		pegrect.setRight(drawwidth + 15), pegrect.setBottom(11);
		if (pegrect.contains(point))
		{
			update(pegrect);
			selectedPeg = STARTPEG;
			edit = true;
		}

		pegrect.setLeft(drawwidth + 8), pegrect.setTop(clientRect.bottom() + 1 - 11);
		pegrect.setRight(drawwidth + 15), pegrect.setBottom(clientRect.bottom() + 1 - 4);
		if (pegrect.contains(point))
		{
			update(pegrect);
			selectedPeg = ENDPEG;
			edit = true;
		}

		if (edit)
		{
			emit editPeg(selectedPeg);
		}
		else
		{
			QColor colour(colourFromPoint(point));
			// Should we add it here?
			emit createPeg(pos, colour);
		}
	}
	else
	{
		//
		// Only call Inherited::mouseDoubleClickEvent if we've not handled the event
		// in here as otherwise it would just call mousePressEvent(), which we
		// don't want.
		//
		Inherited::mouseDoubleClickEvent(event);
	}
}


void QLinearGradientCtrl::keyPressEvent(QKeyEvent * event)
{
	QPainter painter;
	event->setAccepted(true);		// Set that we've handled the event.  If not default will reset
	switch (event->key())
	{
	case Qt::Key_Tab:
		drawSelPeg(painter, selectedPeg);
		switch (selectedPeg)
		{
		case NONE:
			selectedPeg = STARTPEG;
			break;
		case STARTPEG:
			if (stops.size()-1 > 0)
				selectedPeg = 1;
			else selectedPeg = ENDPEG;
			break;
		case ENDPEG:
			selectedPeg = STARTPEG;
			break;
		default:
			if (selectedPeg + 1 >= stops.size())
				selectedPeg = ENDPEG;
			else selectedPeg++;
		}

		drawSelPeg(painter, selectedPeg);
		emit pegSelChanged(selectedPeg);
		emit pegChanged(selectedPeg);
		break;

	case Qt::Key_Home:
		if (selectedPeg > startPegStop && selectedPeg < endPegStop)
		{
			QGradientStop stop = selectedStop();
			stop.first = 0.0;
			moveSelected(stop.first, true);
			//Send parent messages
			emit pegMoved(selectedPeg);
			emit pegChanged(selectedPeg);
		}

		break;

	case Qt::Key_End:
		if (selectedPeg > startPegStop && selectedPeg < endPegStop)
		{
			QGradientStop stop = selectedStop();
			stop.first = 1.0;
			moveSelected(stop.first, true);
			//Send parent messages
			emit pegMoved(selectedPeg);
			emit pegChanged(selectedPeg);

		}

		break;

	case Qt::Key_Left:
	case Qt::Key_Up:
		if (selectedPeg > startPegStop && selectedPeg < endPegStop)
		{
			QGradientStop stop = selectedStop();
			stop.first -= 0.005f;
			//Make sure that the position does not stray below zero
			stop.first = (stop.first <= 1.0f) ? stop.first : 1.0f;
			moveSelected(stop.first, true);

			//Send parent messages
			emit pegMoved(selectedPeg);
			emit pegChanged(selectedPeg);
		}
		break;

	case Qt::Key_Right:
	case Qt::Key_Down:
		if (selectedPeg > startPegStop && selectedPeg < endPegStop)
		{
			QGradientStop stop = selectedStop();
			stop.first += 0.005f;
			//Make sure that the position does not stray above 1
			stop.first = (stop.first <= 1.0f) ? stop.first : 1.0f;
			moveSelected(stop.first, true);

			//Send parent messages
			emit pegMoved(selectedPeg);
			emit pegChanged(selectedPeg);
		}
		break;


	case Qt::Key_Delete:
	case Qt::Key_Back:
		if (selectedPeg > startPegStop && selectedPeg < endPegStop)
			deleteSelected(true);

		break;

	case Qt::Key_PageUp: // Shift the peg up a big jump
		if (selectedPeg > startPegStop && selectedPeg < endPegStop)
		{
			QGradientStop stop = selectedStop();
			stop.first -= 0.01f;
			//Make sure that the position does not stray bellow zero
			stop.first = (stop.first >= 0.0f) ? stop.first : 0.0f;
			moveSelected(stop.first, true);

			//Send parent messages
			emit pegMoved(selectedPeg);
			emit pegChanged(selectedPeg);
		}
		break;

	case Qt::Key_PageDown:
		if (selectedPeg > startPegStop && selectedPeg < endPegStop)
		{
			QGradientStop stop = selectedStop();
			stop.first += 0.01f;
			//Make sure that the position does not stray above 1
			stop.first = (stop.first <= 1.0f) ? stop.first : 1.0f;
			moveSelected(stop.first, true);

			//Send parent messages
			emit pegMoved(selectedPeg);
			emit pegChanged(selectedPeg);
		}
		break;

	case Qt::Key_Enter:
	case Qt::Key_Space:
		if (selectedPeg >= STARTPEG && selectedPeg < endPegStop
			&& selectedPeg != NONE)
		{
			emit editPeg(selectedPeg);
		}
		break;

	case Qt::Key_Insert:
		if (selectedPeg >= 0 && selectedPeg < stops.size())
		{
			// Create a peg at the same position as the currently selected peg
			emit createPeg(stops[selectedPeg].first, stops[selectedPeg].second);
		}
		break;
	default:
		event->setAccepted(false);
	}
	Inherited::keyPressEvent(event); 
}

void QLinearGradientCtrl::deleteSelected(bool bUpdate)
{
	int oldsel;
	float oldpos;

	if (selectedPeg == NONE)
		return;
	if (selectedPeg >= endPegStop)
		return;

	oldsel = selectedPeg;
	oldpos = stops[selectedPeg].first;

	stops.remove(selectedPeg);
	--endPegStop;				// There's now one fewer pegs
	m_Gradient.setStops(stops);

	//Select the previous peg
	if (selectedPeg == STARTPEG) selectedPeg = STARTPEG;
	else selectedPeg--;
	if (selectedPeg < 1) selectedPeg = STARTPEG;

	if (bUpdate)
		update(clientRect);		// Repaint time

	emit pegRemoved(selectedPeg);
	emit pegSelChanged(selectedPeg);
	emit pegChanged(selectedPeg);
}

int QLinearGradientCtrl::setPeg(int index, QColor colour, qreal position)
{
	if (position < 0) position = 0;
	else if (position > 1) position = 1;

	if (index == STARTPEG)
	{
		stops[startPegStop].second = colour;
	}
	else if (index == ENDPEG)
	{
		stops[endPegStop].second = colour;
	}
	else if (index != NONE)
	{
		stops[index].first = position;
		stops[index].second = colour;
		QGradientStop temp(stops[index]);
		std::stable_sort(stops.begin(), stops.end(),
			[](const QGradientStop &lhs, const QGradientStop &rhs) noexcept
		{
			return lhs.first < rhs.first;
		}
			);
		return stops.indexOf(temp);
	}
	return -1;
}

int QLinearGradientCtrl::setSelectedIndex(int iSel)
{
	int oldsel = selectedPeg;
	assert(iSel >= STARTPEG); //Nothing smaller than -3 ok?
	assert(iSel < endPegStop); //Make sure things are in range

	selectedPeg = iSel;

	return oldsel;
}

int QLinearGradientCtrl::moveSelected(qreal newpos, bool bUpdate)
{
	if (selectedPeg < 0)
		return -1;

	if (selectedPeg > startPegStop && selectedPeg < endPegStop)
	{
		QGradientStop newStop(newpos, selectedStop().second);
		stops[selectedPeg].first = newpos;
		std::stable_sort(stops.begin(), stops.end(),
			[](const QGradientStop &lhs, const QGradientStop &rhs) noexcept
		{
			return lhs.first < rhs.first;
		}
			);	
		selectedPeg = stops.indexOf(newStop);		// Make sure the selectedPeg is correct.
		m_Gradient.setStops(stops);
	}

	// if (bUpdate) Invalidate();

	return selectedPeg;
}

QColor QLinearGradientCtrl::setSelectedPegColour(QColor newColour, bool bUpdate)
{
	QColor result(Qt::black);
	if (selectedPeg < 0)
		return result;

	if (selectedPeg > startPegStop && selectedPeg < endPegStop)
	{
		result = selectedStop().second;
		stops[selectedPeg].second = newColour;
		m_Gradient.setStops(stops);
	}

	//if (bUpdate) Invalidate();

	return result;
}

void QLinearGradientCtrl::setShowToolTips(bool bShow)
{
	//m_bShowToolTip = bShow;
	//if (m_Impl->m_ToolTipCtrl.GetSafeHwnd() != nullptr)
	//	m_Impl->m_ToolTipCtrl.Activate(bShow);
	//if (m_bShowToolTip)
	//	m_Impl->SynchronizeTooltips();
}

#pragma warning(push)
#pragma warning(disable:4715)
QGradientStop QLinearGradientCtrl::selectedStop() const
{
	QGradientStop nullStop;   // QPair(0.0, QColor())  QColor() is not a valid colour!
	if (selectedPeg == STARTPEG)
		return stops[startPegStop];
	if (selectedPeg == ENDPEG)
		return stops[endPegStop];
	if (selectedPeg == NONE)
		return nullStop;
	if (selectedPeg > startPegStop && selectedPeg < endPegStop)
		return stops[selectedPeg];
	else assert(false); // Some kind of stupid error
}
#pragma warning(pop)

bool QLinearGradientCtrl::isVertical()
{
	if (m_Orientation == ForceVertical)
		return true;
	else if (m_Orientation == ForceHorizontal)
		return false;
	else
	{
		return (clientRect.right() <= clientRect.bottom());
	}
}

int QLinearGradientCtrl::pointFromPos(qreal pos)
{
	qreal length = isVertical() ? ((qreal)clientRect.height() - 10.0)
		: ((qreal)clientRect.width() - 10.0);

	return (int)(pos*length) + 4;
}

qreal QLinearGradientCtrl::posFromPoint(int point)
{
	int length = isVertical() ? (clientRect.bottom()+1 - 9) : (clientRect.right()+1 - 9);
	int x = point - 5;
	qreal val;

	val = (qreal)x / (qreal)length;

	if (val < 0) val = 0.0;
	else if (val > 1) val = 1.0;

	return val;
}


int QLinearGradientCtrl::getPegIndent(int index)
{
	int lastpegpos = -1, pegindent = 0;

	for (int i = 1; i <= index; i++)   // ignore initial stop
	{
		auto position = stops[i].first;
		if (lastpegpos != -1 && lastpegpos >= pointFromPos(position) - 10)
			pegindent += 1;
		else pegindent = 0;

		lastpegpos = pointFromPos(position);
	}

	return pegindent % 2;
}

void QLinearGradientCtrl::getPegRect(int index, QRect *rect, bool right)
{
	int drawwidth = getDrawWidth();
	bool vertical = isVertical();

	if (index == STARTPEG)
	{
		if (right)
		{
			if (vertical)
			{
				rect->setTopLeft(QPoint(drawwidth - 15, 4));
				rect->setBottomRight(QPoint(drawwidth - 8, 11));
			}
			else
			{
				rect->setTopLeft(QPoint(4, clientRect.bottom()+1 - drawwidth + 8));
				rect->setBottomRight(QPoint(11, clientRect.bottom()+1 - drawwidth + 15));
			}
		}
		else
		{
			if (vertical)
			{
				rect->setTopLeft(QPoint(8, 4));
				rect->setBottomRight(QPoint(15, 11));
			}
			else
			{
				rect->setTopLeft(QPoint(4, clientRect.bottom()+1 - 15));
				rect->setBottomRight(QPoint(11, clientRect.bottom() + 1 - 8));
			}
		}
		return;
	}

	if (index == ENDPEG)
	{
		if (right)
		{
			if (vertical)
			{
				rect->setTopLeft(QPoint(drawwidth - 15, clientRect.bottom()+1 - 11));
				rect->setBottomRight(QPoint(drawwidth - 8, clientRect.bottom() + 1 - 4));
			}
			else
			{
				rect->setTopLeft(QPoint(clientRect.right()+1 - 11, clientRect.bottom()+1 - drawwidth + 8));
				rect->setBottomRight(QPoint(clientRect.right() + 1 - 4, clientRect.bottom() + 1 - drawwidth + 15));
			}
		}
		else
		{
			if (vertical)
			{
				rect->setTopLeft(QPoint(8, clientRect.bottom()+1 - 11));
				rect->setBottomRight(QPoint(15, clientRect.bottom()+1 - 4));
			}
			else
			{
				rect->setTopLeft(QPoint(clientRect.right()+1 - 11, clientRect.bottom()+1 - 15));
				rect->setBottomRight(QPoint(clientRect.right()+1 - 4, clientRect.bottom()+1 - 8));
			}
		}

		return;
	}

	auto position = stops[index].first;
	int p = pointFromPos(position);
	int indent = getPegIndent(index) * 11;

	if (right)
	{
		if (vertical)
		{
			rect->setTopLeft(QPoint(drawwidth + indent - 23, p - 3));
			rect->setBottomRight(QPoint(drawwidth + indent - 13, p + 6));
		}
		else
		{
			rect->setTopLeft(QPoint(p - 3, clientRect.bottom()+1 - drawwidth - indent + 13));
			rect->setBottomRight(QPoint(p + 6, clientRect.bottom()+1 - drawwidth - indent + 23));
		}
	}
	else
	{
		if (vertical)
		{
			rect->setTopLeft(QPoint(13 - indent, p - 3));
			rect->setBottomRight(QPoint(23 - indent, p + 6));
		}
		else
		{
			rect->setTopLeft(QPoint(p - 3, clientRect.bottom()+1 + indent - 23));
			rect->setBottomRight(QPoint(p + 6, clientRect.bottom()+1 + indent - 13));
		}
	}
}

QRegion QLinearGradientCtrl::getPegRegion()
{
	QRegion result;
	QPoint pegpoint;
	int drawwidth = getDrawWidth();
	bool vertical = isVertical();
	int colcount = (int)m_LeftDownSide + (int)m_RightUpSide;

	if (colcount == 0)
	{
		return result;			// Return the empty region
	}


	QVector<QPoint> points(4);
	//----- End pegs -----//
	if (vertical)
	{
		if (m_RightUpSide)
		{
			points.push_back(QPoint(drawwidth - 15, 4));
			points.push_back(QPoint(drawwidth - 15, 11));
			points.push_back(QPoint(drawwidth - 8, 11));
			points.push_back(QPoint(drawwidth - 8, 4));
			result += QRegion(QPolygon(points));
			points.empty();

			points.push_back(QPoint(drawwidth - 15, clientRect.bottom()+1 - 4));
			points.push_back(QPoint(drawwidth - 15, clientRect.bottom()+1 - 11));
			points.push_back(QPoint(drawwidth - 8, clientRect.bottom()+1 - 11));
			points.push_back(QPoint(drawwidth - 8, clientRect.bottom()+1 - 4));
			result += QRegion(QPolygon(points));
			points.empty();
		}

		if (m_LeftDownSide)
		{
			points.push_back(QPoint(8, 4));
			points.push_back(QPoint(8, 11));
			points.push_back(QPoint(15, 11));
			points.push_back(QPoint(15, 4));
			result += QRegion(QPolygon(points));
			points.empty();

			points.push_back(QPoint(8, clientRect.bottom()+1 - 4));
			points.push_back(QPoint(8, clientRect.bottom()+1 - 11));
			points.push_back(QPoint(15, clientRect.bottom()+1 - 11));
			points.push_back(QPoint(15, clientRect.bottom()+1 - 4));
			result += QRegion(QPolygon(points));
			points.empty();
		}
	}
	else
	{
		if (m_RightUpSide)
		{
			points.push_back(QPoint(4, clientRect.bottom()+1 - drawwidth + 8));
			points.push_back(QPoint(11, clientRect.bottom()+1 - drawwidth + 8));
			points.push_back(QPoint(11, clientRect.bottom()+1 - drawwidth + 15));
			points.push_back(QPoint(4, clientRect.bottom()+1 - drawwidth + 15));
			result += QRegion(QPolygon(points));
			points.empty();

			points.push_back(QPoint(clientRect.right()+1 - 4, clientRect.bottom()+1 - drawwidth + 8));
			points.push_back(QPoint(clientRect.right()+1 - 11, clientRect.bottom()+1 - drawwidth + 8));
			points.push_back(QPoint(clientRect.right()+1 - 11, clientRect.bottom()+1 - drawwidth + 15));
			points.push_back(QPoint(clientRect.right()+1 - 4, clientRect.bottom()+1 - drawwidth + 15));
			result += QRegion(QPolygon(points));
			points.empty();
		}

		if (m_LeftDownSide)
		{
			points.push_back(QPoint(4, clientRect.bottom()+1 - 8));
			points.push_back(QPoint(11, clientRect.bottom()+1 - 8));
			points.push_back(QPoint(11, clientRect.bottom()+1 - 15));
			points.push_back(QPoint(4, clientRect.bottom()+1 - 15));
			result += QRegion(QPolygon(points));
			points.empty();

			points.push_back(QPoint(clientRect.right()+1 - 4, clientRect.bottom()+1 - 8));
			points.push_back(QPoint(clientRect.right()+1 - 11, clientRect.bottom()+1 - 8));
			points.push_back(QPoint(clientRect.right()+1 - 11, clientRect.bottom()+1 - 15));
			points.push_back(QPoint(clientRect.right()+1 - 4, clientRect.bottom()+1 - 15));
			result += QRegion(QPolygon(points));
			points.empty();
		}
	}

	//----- Main pegs -----//
	for (int i = 1; i < endPegStop-1; i++)
	{
		if (vertical)
		{
			pegpoint.ry() = pointFromPos(stops[i].first);

			if (m_LeftDownSide)
			{
				pegpoint.rx() = 23 - getPegIndent(i) * 11;

				points.push_back(QPoint(pegpoint.x(), pegpoint.y() + 1));
				points.push_back(QPoint(pegpoint.x() - 10, pegpoint.y() - 4));
				points.push_back(QPoint(pegpoint.x() - 10, pegpoint.y() + 6));
				result += QRegion(QPolygon(points));
				points.empty();
			}

			if (m_RightUpSide)
			{
				pegpoint.rx() = getPegIndent(i) * 11 + drawwidth - 23;

				points.push_back(QPoint(pegpoint.x(), pegpoint.y() + 1));
				points.push_back(QPoint(pegpoint.x() + 10, pegpoint.y() - 4));
				points.push_back(QPoint(pegpoint.x() + 10, pegpoint.y() + 6));
				result += QRegion(QPolygon(points));
				points.empty();
			}
		}
		else
		{
			pegpoint.rx() = pointFromPos(stops[i].first);

			if (m_LeftDownSide)
			{
				pegpoint.ry() = clientRect.bottom()+1 - 23 + getPegIndent(i) * 11;

				points.push_back(QPoint(pegpoint.x() + 1, pegpoint.y() - 1));
				points.push_back(QPoint(pegpoint.x() - 4, pegpoint.y() + 10));
				points.push_back(QPoint(pegpoint.x() + 7, pegpoint.y() + 10));
				result += QRegion(QPolygon(points));
				points.empty();
			}

			if (m_RightUpSide)
			{
				pegpoint.ry() = clientRect.bottom()+1 - getPegIndent(i) * 11 - drawwidth + 22;

				points.push_back(QPoint(pegpoint.x() + 1, pegpoint.y() + 1));
				points.push_back(QPoint(pegpoint.x() - 4, pegpoint.y() - 9));
				points.push_back(QPoint(pegpoint.x() + 6, pegpoint.y() - 9));
				result += QRegion(QPolygon(points));
				points.empty();
			}
		}
	}

	return result;
}
//void QLinearGradientCtrl::showEvent(QShowEvent *event)
//{
//	if (!event->spontaneous())
//	{
//	
//	}
//	// Invoke base class showEvent()
//	return Inherited::showEvent(event);
//}