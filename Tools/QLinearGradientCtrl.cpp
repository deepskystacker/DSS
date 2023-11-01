// QLinearGradientCtrl.cpp 
//
#include "stdafx.h"
#include "QLinearGradientCtrl.h"

#include "Ztrace.h"
#include "ZExcBase.h"

/////////////////////////////////////////////////////////////////////////////
// QLinearGradientCtrl

enum SPECIALPEGS : short
{
	BACKGROUND = -4,
	STARTPEG = -3,
	ENDPEG = -2,
	NONE = -1
};

QLinearGradientCtrl::QLinearGradientCtrl(QWidget * parent, QColor start, QColor end) :
	QWidget(parent),
	m_Gradient(0, 0, 1, 0),
	m_Width(GCW_AUTO),
	selectedPeg(NONE),
	lastSelectedPeg(NONE),
	m_LastPos(0),
	m_showToolTips(true),
	m_Orientation(Orientation::Auto),
	startPegStop(0),
	m_LeftDownSide(true),
	m_RightUpSide(false)
{
	m_ToolTipFormat = "&SELPOS\nPosition: &SELPOS Colour: R &R G &G B &B\nColour: R &R G &G B &B\nColour: R &R G &G B &B\nDouble Click to Add a New Peg";
	//m_Impl = new QLinearGradientCtrlImpl(this);
	m_Gradient.setColorAt(0, start);
	m_Gradient.setColorAt(0.001, start);
	m_Gradient.setColorAt(0.999, end);
	m_Gradient.setColorAt(1, end);
	setFocusPolicy(Qt::StrongFocus);		// Make sure we get key events.

	stops = m_Gradient.stops();				// Grab the gradient stops
	endPegStop = stops.size() - 1;
}

QLinearGradientCtrl & QLinearGradientCtrl::setGradient(QLinearGradient const& src)
{
	m_Gradient = src;
	stops = m_Gradient.stops();
	endPegStop = stops.size() - 1;
	return *this;
}

QLinearGradientCtrl::~QLinearGradientCtrl()
{
	//delete m_Impl;
}

void QLinearGradientCtrl::setColorAt(double pos, QColor colour)
{
	m_Gradient.setColorAt(pos, colour);
	stops = m_Gradient.stops();				// Grab the gradient stops
	endPegStop = stops.size() - 1;

}

int QLinearGradientCtrl::getDrawWidth() const
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

QRect QLinearGradientCtrl::gradientRect() const
{
	QRect result;
	bool vertical = isVertical();
	int drawwidth = getDrawWidth();
	int l = (vertical ? clientRect.bottom() + 1 : clientRect.right() + 1) - 10;
	int w = drawwidth - (m_RightUpSide ? 24 : 5) - (m_LeftDownSide ? 24 : 5);

	if (clientRect.bottom() + 1 < 11) return result;	// Null rect

	if (vertical)
		result = QRect((m_LeftDownSide ? 24 : 5), 5, w, l);
	else
		result = QRect(5, clientRect.bottom() + 1 - drawwidth + (m_RightUpSide ? 24 : 5), l, w);

	return result;
}

void QLinearGradientCtrl::drawGradient(QPainter & painter)
{
	QRect rect = gradientRect();
	if (rect.isNull()) return;

	if (isVertical())
	{
		m_Gradient.setStart(rect.top(), rect.right());
		m_Gradient.setFinalStop(rect.bottom(), rect.right());
	}
	else
	{
		m_Gradient.setStart(rect.left(), rect.top());
		m_Gradient.setFinalStop(rect.right(), rect.top());
	}
	painter.save();
	painter.setBrush(m_Gradient);
	painter.drawRect(rect);
	painter.restore();
}

void QLinearGradientCtrl::drawEndPegs(QPainter & painter)
{
	//
	// Get foreground colour (window text) and create a pen to draw a boundary
	// in that colour
	//
	QColor	foreground(QPalette().color(QPalette::WindowText));	// get foreground colour 
	QPen	pen(foreground);
	painter.save();
	painter.setPen(pen);

	//
	// Get the colour of the starting and ending stops and create brushes 
	//
	QBrush  startBrush(stops[startPegStop].second);
	QBrush	endBrush(stops[endPegStop].second);
	QRect pegrect;
	
	//----- Draw the first marker -----//
	painter.setBrush(startBrush);
	getPegRect(STARTPEG, &pegrect, m_RightUpSide);
	painter.drawRect(pegrect);

	//----- Draw the last one -----//
	painter.setBrush(endBrush);
	getPegRect(ENDPEG, &pegrect, m_RightUpSide);
	painter.drawRect(pegrect);
	painter.restore();
}

void QLinearGradientCtrl::drawPegs(QPainter& painter)
{
	// No stupid selection
	if (selectedPeg > stops.size())
		selectedPeg = -1;

	painter.save();
	//
	// Get foreground colour (window text) and create a pen to draw a one pixel boundary
	// in that colour
	//
	QColor	foreground(QPalette().color(QPalette::WindowText));	// get foreground colour 
	QPen	pen(foreground);
	painter.setPen(pen);
	for (int i = 1; i < stops.size() - 1; i++)	// ignore starting and ending stops
	{
		QPolygon poly{ getPegPoly(i) };
		QBrush brush{ stops[i].second };
		painter.setBrush(brush);
		painter.drawPolygon(poly);
	}
	painter.restore();
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
	QPolygon poly{ 3 };

	painter.save();
	//
	// Get foreground colour (window text) and create a pen to draw a one pixel boundary
	// in that colour
	//
	QColor	foreground(QPalette().color(QPalette::WindowText));	// get foreground colour 
	QPen	pen(foreground);
	painter.setPen(pen);

	QBrush brush;			// create a null brush so background isn't filled
	painter.setBrush(brush);

	//
	// Set composition mode to QPainter::RasterOp_NotDestination
	//
	painter.setCompositionMode(QPainter::RasterOp_NotDestination);

	//Prepare the coodrdinates
	switch (direction)
	{
	case 0:
		poly[0].rx() = 8 + point.x();
		poly[0].ry() = point.y() - 3;
		poly[1].rx() = 2 + point.x();
		poly[1].ry() = point.y();
		poly[2].rx() = 8 + point.x();
		poly[2].ry() = point.y() + 3;
		break;
	case 1:
		poly[0].rx() = point.x() - 3;
		poly[0].ry() = point.y() - 8;
		poly[1].rx() = point.x();
		poly[1].ry() = point.y() - 2;
		poly[2].rx() = point.x() + 3;
		poly[2].ry() = point.y() - 8;
		break;
	case 2:
		poly[0].rx() = point.x() - 9, poly[0].ry() = point.y() - 3;
		poly[1].rx() = point.x() - 3, poly[1].ry() = point.y();
		poly[2].rx() = point.x() - 9, poly[2].ry() = point.y() + 3;
		break;
	default:
		poly[0].rx() = point.x() - 3;
		poly[0].ry() = point.y() + 8;
		poly[1].rx() = point.x();
		poly[1].ry() = point.y() + 2;
		poly[2].rx() = point.x() + 3;
		poly[2].ry() = point.y() + 8;
		break;
	}
	painter.drawPolygon(poly);

	//Restore the composition mode etc ...
	painter.restore();
}

void QLinearGradientCtrl::drawSelPeg(QPainter & painter, int peg)
{
	int drawwidth = getDrawWidth() - 23;
	//bool vertical = isVertical();

	painter.save();
	//
	// Get foreground colour (window text) and create a pen to draw a one pixel boundary
	// in that colour
	//
	QColor	foreground(QPalette().color(QPalette::WindowText));	// get foreground colour 
	QPen	pen(foreground);
	painter.setPen(pen);

	QBrush brush;			// create a null brush so background isn't filled
	painter.setBrush(brush);

	//
	// Set painter's composition to QPainter::RasterOp_NotDestination
	//
	painter.setCompositionMode(QPainter::RasterOp_NotDestination);

	//"Select objects"//
	if (STARTPEG == peg || ENDPEG == peg)
	{
		QRect rect;
		getPegRect(peg, &rect, m_RightUpSide);
		rect.adjust(+1, +1, -1, -1);
		painter.drawRect(rect);
		painter.restore();
		return;
	}

	//Restore the composition mode etc ...
	painter.restore();

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
	QRect theRect{ event->rect() };
	//
	// Erase the area that's been invalidated
	// 
	painter.eraseRect(theRect);

	//----- Draw the Palette -----//
	if (theRect.intersects(gradientRect()))
		drawGradient(painter);


	//----- Draw the marker arrows -----//
	drawEndPegs(painter);
	drawPegs(painter);
	//
	// The order is important!
	//    The function DrawSelPeg must be called last as the peg has 
	// already been drawn in normally by DrawPegs
	//
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

		bool nowselected = false;
		m_LastPos = -1;

		//----- Just in case the user starts dragging -----//
		setFocus();
		setMouseTracking(true);
		m_MouseDown = point;

		//----- Check if the user is selecting a marker peg -----//
		for (int i = 1; i < stops.size() -1 ; i++) // Only the movable ones please
		{
			getPegRect(i, &pegrect, m_RightUpSide);
			if (pegrect.contains(point))
			{
				setSelected(i);
				nowselected = true;
				break;
			}
		}

		//----- Check if the user is trying to select the first or last one -----//
		if (!nowselected)
		{
			getPegRect(STARTPEG, &pegrect, m_RightUpSide);
			if (pegrect.contains(point))
			{
				setSelected(STARTPEG);
				nowselected = true;
			}

			getPegRect(ENDPEG, &pegrect, m_RightUpSide);
			if (pegrect.contains(point))
			{
				setSelected(ENDPEG);
				nowselected = true;
			}
		}

		if (!nowselected)
		{
			lastSelectedPeg = selectedPeg;
			selectedPeg = NONE;
			m_LastPos = -1;
		}
		update();

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
		QRegion region{ getPegRegion() };

		//----- Continue -----//
		pos = posFromPoint(selpegpos);
		//"The id of the selection may change"//
		selectedPeg = setPeg(selectedPeg, stops[selectedPeg].second, pos);
		m_Gradient.setStops(stops);
		region = region.united(getPegRegion());;
		//----- Get the region for the pegs and erase them -----//
		//
		// Note that this uses repaint() not update()
		// Qt docs say:
		//
		// We suggest only using repaint() if you need an immediate repaint, for example during animation.
		//
		repaint(region.boundingRect());			// Erase the old pegs using repaint instead of update.

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
		{
			emit pegMove(selectedPeg);
			emit pegChanged(selectedPeg);
		}
	}

	Inherited::mouseMoveEvent(event);
}

void QLinearGradientCtrl::mouseReleaseEvent(QMouseEvent *event)
{
	if (Qt::LeftButton == event->button() )
	{
		if (selectedPeg >= STARTPEG && selectedPeg != NONE)
		{
#if (0)
			bool vertical = isVertical();
			//int selpegpos = vertical ? point.y-5 : point.x-5;

			if (vertical)
				update(QRect(getDrawWidth(), 0, clientRect.right()+1,
					clientRect.bottom()+1)); //Erase the old ones
			else
				update(QRect(0, 0, clientRect.right()+1,
					clientRect.bottom()+1 - getDrawWidth())); //Erase the old ones

			//m_Impl->DestroyTooltip();
#endif
			QRect pegrect;
			getPegRect(selectedPeg, &pegrect, m_RightUpSide);
			update(pegrect);

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
		//int drawwidth = getDrawWidth();

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
					lastSelectedPeg = setSelected(i);
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
					lastSelectedPeg = setSelected(i);
					edit = true;
					break;
				}
			}
		}


		//----- Check if the user is trying to select the first or last one -----//
		getPegRect(STARTPEG, &pegrect, m_RightUpSide);
		if (pegrect.contains(point))
		{
			update(pegrect);
			lastSelectedPeg = setSelected(STARTPEG);
			edit = true;
		}

		getPegRect(ENDPEG, &pegrect, m_RightUpSide);
		if (pegrect.contains(point))
		{
			update(pegrect);
			lastSelectedPeg = setSelected(ENDPEG);
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
			if (selectedPeg + 1 >= static_cast<int>(stops.size()))
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
			stop.first = 0.005;
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
			stop.first = 0.995;
			moveSelected(stop.first, true);
			//Send parent messages
			emit pegMoved(selectedPeg);
			emit pegChanged(selectedPeg);

		}

		break;

	case Qt::Key_Left:
	case Qt::Key_Down:
		if (selectedPeg > startPegStop && selectedPeg < endPegStop)
		{
			QGradientStop stop = selectedStop();
			stop.first -= 0.005f;
			//Make sure that the position does not stray below 0.005
			stop.first = (stop.first <= 0.005f) ? stop.first : 0.005f;
			moveSelected(stop.first, true);

			//Send parent messages
			emit pegMoved(selectedPeg);
			emit pegChanged(selectedPeg);
		}
		break;

	case Qt::Key_Right:
	case Qt::Key_Up:
		if (selectedPeg > startPegStop && selectedPeg < endPegStop)
		{
			QGradientStop stop = selectedStop();
			stop.first += 0.005f;
			//Make sure that the position does not stray above 0.995
			stop.first = (stop.first <= 0.995f) ? stop.first : 0.995f;

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

	case Qt::Key_PageDown:
		if (selectedPeg > startPegStop && selectedPeg < endPegStop)
		{
			QGradientStop stop = selectedStop();
			stop.first -= 0.01f;
			//Make sure that the position does not stray below 0.005
			stop.first = (stop.first >= 0.005f) ? stop.first : 0.005f;
			moveSelected(stop.first, true);

			//Send parent messages
			emit pegMoved(selectedPeg);
			emit pegChanged(selectedPeg);
		}
		break;

	case Qt::Key_PageUp: // Shift the peg up a big jump
		if (selectedPeg > startPegStop && selectedPeg < endPegStop)
		{
			QGradientStop stop = selectedStop();
			stop.first += 0.01f;
			//Make sure that the position does not stray above 0.995
			stop.first = (stop.first <= 0.995f) ? stop.first : 0.995f;
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
			//
			// Create a peg at about the same position as the currently selected peg
			// but not the identical location otherwise QGradient::setStops() will
			// throw it away
			//
			if (stops[selectedPeg].first < 0.990f)
			{
				emit createPeg(stops[selectedPeg].first + 0.005f, stops[selectedPeg].second);
			}
			else
			{
				emit createPeg(stops[selectedPeg].first - 0.005f, stops[selectedPeg].second);
			}
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
	if (!STARTPEG == selectedPeg)
		selectedPeg--;
	if (selectedPeg < 1) selectedPeg = STARTPEG;

	if (bUpdate)
		update(clientRect);		// Repaint time

	emit pegRemoved(selectedPeg);
	emit pegSelChanged(selectedPeg);
	emit pegChanged(selectedPeg);
}

int QLinearGradientCtrl::setPeg(int index, QColor colour, qreal position)
{
	if (index == NONE)
		return -1;
	if (index == STARTPEG)
	{
		stops[startPegStop].second = colour;
	}
	else if (index == ENDPEG)
	{
		stops[endPegStop].second = colour;
	}
	else
	{
		position = std::clamp(position, 0.001, 0.999);
		auto it = std::find_if(stops.begin(), stops.end(), [position](const auto& v) { return v.first == position; });
		if (it != stops.end()) // moved position already exists, 'it' points to element with the identical position.
		{
			const qreal old_position = stops[index].first;
			const bool direction_up = position > old_position;
			const qreal new_position = direction_up ? position + 0.001 : position - 0.001;
			if (std::find_if(stops.cbegin(), stops.cend(), [new_position](const auto& v) { return v.first == new_position; }) != stops.cend()) // corrected position already exists
			{
				position = direction_up ? (0.5 * (position + (it + 1)->first)) : (0.5 * (position + (it - 1)->first));
			}
			else
			{
				position = new_position;
			}
		}
		stops[index].first = position;
		stops[index].second = colour;
		std::stable_sort(stops.begin(), stops.end(), [](const QGradientStop& lhs, const QGradientStop& rhs) noexcept {
			return lhs.first < rhs.first;
		});
		for (int i = 0; i < stops.size(); i++)
			if (stops[i].first == position)
				index = i;
	}

	return index;
}

int QLinearGradientCtrl::setSelected(int iSel)
{
	if (selectedPeg != iSel)
	{
		QRect lastRect, selRect;
		ZASSERT(iSel >= STARTPEG); //Nothing smaller than -3 ok?
		ZASSERT(iSel < endPegStop); //Make sure things are in range
		lastSelectedPeg = selectedPeg;
		selectedPeg = iSel;
		emit pegSelChanged(selectedPeg);
		update(getPegRegion());		
	}

	return lastSelectedPeg;
}

int QLinearGradientCtrl::moveSelected(qreal newpos, bool)
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

QColor QLinearGradientCtrl::setSelectedPegColour(QColor newColour, bool)
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

void QLinearGradientCtrl::setShowToolTips(bool)
{
	//m_bShowToolTip = bShow;
	//if (m_Impl->m_ToolTipCtrl.GetSafeHwnd() != nullptr)
	//	m_Impl->m_ToolTipCtrl.Activate(bShow);
	//if (m_bShowToolTip)
	//	m_Impl->SynchronizeTooltips();
}

QGradientStop QLinearGradientCtrl::selectedStop() const
{
	QGradientStop nullStop;   // QPair(0.0, QColor())  QColor() is not a valid colour!
	if (selectedPeg == STARTPEG)
		return stops[startPegStop];
	if (selectedPeg == ENDPEG)
		return stops[endPegStop];
	if (selectedPeg == NONE)
		return nullStop;
	ZASSERT(selectedPeg > startPegStop && selectedPeg < endPegStop);
	return stops[selectedPeg];
}

bool QLinearGradientCtrl::isVertical() const
{
	if (m_Orientation == Orientation::ForceVertical)
		return true;
	else if (m_Orientation == Orientation::ForceHorizontal)
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

	return (int)(pos*length) + 5;
}

qreal QLinearGradientCtrl::posFromPoint(int point)
{
	int length = isVertical() ? (clientRect.bottom() - 9) : (clientRect.right() - 9);
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

	constexpr int lmargin = 1;
	constexpr int rmargin = 1;
	constexpr int tmargin = 3;
	constexpr int bmargin = 3;
	constexpr int size = 8;
	constexpr QSize endsize{ size , size };


	if (index == STARTPEG)
	{
		if (right)
		{
			if (vertical)
			{
				rect->setTopLeft(QPoint(drawwidth - (rmargin + size),
					tmargin));
			}
			else
			{
				rect->setTopLeft(QPoint(lmargin,
					clientRect.bottom() + 1 - drawwidth + tmargin));
			}
		}
		else
		{
			if (vertical)
			{
				rect->setTopLeft(QPoint(lmargin,
					tmargin));
			}
			else
			{
				rect->setTopLeft(QPoint(lmargin,clientRect.bottom() + 1 - (bmargin + size)));
			}
		}
		rect->setSize(endsize);
		return;
	}

	if (index == ENDPEG)
	{
		if (right)
		{
			if (vertical)
			{
				rect->setTopLeft(QPoint(drawwidth - (rmargin + size), 
					clientRect.bottom() + 1 - (bmargin + size)));
			}
			else
			{
				rect->setTopLeft(QPoint(clientRect.right() + 1 - (rmargin + size), 
					clientRect.bottom() + 1 - drawwidth + (bmargin + size)));
			}
		}
		else
		{
			if (vertical)
			{
				rect->setTopLeft(QPoint(lmargin,
					clientRect.bottom() + 1 - (bmargin + size)));
			}
			else
			{
				rect->setTopLeft(QPoint(clientRect.right() + 1 - (rmargin + size),
					clientRect.bottom() + 1 - (bmargin + size)));
			}
		}
		rect->setSize(endsize);
		return;
	}

	// auto position = stops[index].first;
	// int p = pointFromPos(position);
	// int indent = getPegIndent(index) * 11;

	QRegion region{ getPegPoly(index) };
	*rect = region.boundingRect();
}

QPolygon QLinearGradientCtrl::getPegPoly(int i)
{
	QPoint pegpoint;
	int drawwidth = getDrawWidth();
	bool vertical = isVertical();

	ZASSERT(i > 0 && i < endPegStop);

	QVector<QPoint> points;

	if (vertical)
	{
		pegpoint.ry() = pointFromPos(stops[i].first);

		if (m_LeftDownSide)
		{
			pegpoint.rx() = 23 - getPegIndent(i) * 11;

			points.push_back(QPoint(pegpoint.x(), pegpoint.y()));
			points.push_back(QPoint(pegpoint.x() - 9, pegpoint.y() - 5));
			points.push_back(QPoint(pegpoint.x() - 9, pegpoint.y() + 5));
			return QPolygon(points);
		}

		if (m_RightUpSide)
		{
			pegpoint.rx() = getPegIndent(i) * 11 + drawwidth - 23;

			points.push_back(QPoint(pegpoint.x(), pegpoint.y()));
			points.push_back(QPoint(pegpoint.x() + 9, pegpoint.y() - 5));
			points.push_back(QPoint(pegpoint.x() + 9, pegpoint.y() + 5));
			return QPolygon(points);
		}
	}
	else
	{
		pegpoint.rx() = pointFromPos(stops[i].first);

		if (m_LeftDownSide)
		{
			pegpoint.ry() = clientRect.bottom() + 1 - 23 + getPegIndent(i) * 11;

			points.push_back(QPoint(pegpoint.x(), pegpoint.y() - 1));
			points.push_back(QPoint(pegpoint.x() - 5, pegpoint.y() + 9));
			points.push_back(QPoint(pegpoint.x() + 5, pegpoint.y() + 9));
			return QPolygon(points);
		}

		if (m_RightUpSide)
		{
			pegpoint.ry() = clientRect.bottom() + 1 - getPegIndent(i) * 11 - drawwidth + 22;

			points.push_back(QPoint(pegpoint.x(), pegpoint.y() + 1));
			points.push_back(QPoint(pegpoint.x() - 5, pegpoint.y() - 9));
			points.push_back(QPoint(pegpoint.x() + 5, pegpoint.y() - 9));
			return QPolygon(points);
		}
	}
	return QPolygon();		// return empty polygon
}

QRegion QLinearGradientCtrl::getPegRegion(short peg)
{
	QRect rect;

	ZASSERT(peg >= STARTPEG && peg <= endPegStop);
	if (STARTPEG == peg)
	{
		getPegRect(STARTPEG, &rect, m_RightUpSide);
		return QRegion(rect);
	}
	else if (ENDPEG == peg)
	{
		getPegRect(ENDPEG, &rect, m_RightUpSide);
		return QRegion(rect);
	}
	else
	{
		return QRegion(getPegPoly(peg));
	}
}


QRegion QLinearGradientCtrl::getPegRegion()
{
	QRegion result;
	QPoint pegpoint;
	// int drawwidth = getDrawWidth();
	// bool vertical = isVertical();
	int colcount = (int)m_LeftDownSide + (int)m_RightUpSide;

	if (colcount == 0)
	{
		return result;			// Return the empty region
	}

	//----- End pegs -----//
	QRect rect;
	getPegRect(STARTPEG, &rect, m_RightUpSide);
	result += QRegion(rect);
	getPegRect(ENDPEG, &rect, m_RightUpSide);
	result += QRegion(rect);

	//----- Main pegs -----//
	for (int i = 1; i < endPegStop-1; i++)
	{
		result += QRegion(getPegPoly(i));
	}

	return result;
}
//void QLinearGradientCtrl::showEvent(QShowEvent *event)
//{
//	if (!event->spontaneous())
//	{
//		setFocus();
//	}
//	// Invoke base class showEvent()
//	return Inherited::showEvent(event);
//}