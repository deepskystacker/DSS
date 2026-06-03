/****************************************************************************
**
** Copyright (C) 2026 Amila Sanjaya Karunarathna
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
#include "pch.h"
#include "QMTFSlider.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPolygon>
#include <QLinearGradient>
#include <QWheelEvent>

namespace DSS
{
	QMTFSlider::QMTFSlider(QWidget* parent)
		: QWidget(parent), m_Shadows(0.0), m_Midtones(0.5), m_Highlights(1.0), m_Color(Qt::white), m_ActiveHandle(ActiveHandle::None), m_SelectedHandle(ActiveHandle::None)
	{
		setMinimumHeight(24);
		setCursor(Qt::ArrowCursor);
	}

	void QMTFSlider::setShadows(double val)
	{
		m_Shadows = std::clamp(val, 0.0, m_Midtones);
		update();
		emit shadowsChanged(m_Shadows);
	}

	void QMTFSlider::setMidtones(double val)
	{
		m_Midtones = std::clamp(val, m_Shadows, m_Highlights);
		update();
		emit midtonesChanged(m_Midtones);
	}

	void QMTFSlider::setHighlights(double val)
	{
		m_Highlights = std::clamp(val, m_Midtones, 1.0);
		update();
		emit highlightsChanged(m_Highlights);
	}

	void QMTFSlider::setColor(const QColor& c)
	{
		m_Color = c;
		update();
	}

	void QMTFSlider::setValues(double s, double m, double h)
	{
		m_Shadows = std::clamp(s, 0.0, 1.0);
		m_Highlights = std::clamp(h, m_Shadows, 1.0);
		m_Midtones = std::clamp(m, m_Shadows, m_Highlights);
		update();
	}

	void QMTFSlider::clearSelectedHandle()
	{
		if (m_SelectedHandle != ActiveHandle::None)
		{
			m_SelectedHandle = ActiveHandle::None;
			update();
		}
	}

		int QMTFSlider::posFromValue(double val) const
	{
		const int margin = 8;
		int w = width() - 2 * margin;
		return margin + static_cast<int>(val * w);
	}

	double QMTFSlider::valueFromPos(int pos) const
	{
		const int margin = 8;
		int w = width() - 2 * margin;
		if (w <= 0) return 0.0;
		double val = static_cast<double>(pos - margin) / w;
		return std::clamp(val, 0.0, 1.0);
	}

	void QMTFSlider::paintEvent(QPaintEvent*)
	{
		QPainter p(this);
		p.setRenderHint(QPainter::Antialiasing);

		QRect r = rect();
		const int margin = 8;
		QRect barRect(margin, 2, r.width() - 2 * margin, r.height() - 4);

		// Draw background gradient
		QLinearGradient grad(barRect.left(), 0, barRect.right(), 0);
		grad.setColorAt(0.0, Qt::black);
		grad.setColorAt(1.0, m_Color);

		p.setPen(Qt::NoPen);
		p.setBrush(grad);
		p.drawRect(barRect);

		// Draw border
		p.setPen(QPen(Qt::darkGray, 1));
		p.setBrush(Qt::NoBrush);
		p.drawRect(barRect);

		// Draw handles (Shadows and Highlights first, Midtones on top so it's easier to grab)
		drawHandle(p, posFromValue(m_Shadows), ActiveHandle::Shadows);
		drawHandle(p, posFromValue(m_Highlights), ActiveHandle::Highlights);
		drawHandle(p, posFromValue(m_Midtones), ActiveHandle::Midtones);
	}

	void QMTFSlider::drawHandle(QPainter& p, int x, ActiveHandle type)
	{
		int yBottom = height() - 2;
		int size = 6;

		int midtoneHeight = static_cast<int>(size * 2 * 0.5); // 50% shorter height
		const int apexY = yBottom - midtoneHeight;

		// Draw marker line only above the handle, starting from the triangle apex.
		p.setPen(QPen(Qt::gray, 1));
		p.drawLine(x, 2, x, apexY);

		QPolygon poly;
		if (type == ActiveHandle::Shadows)
		{
			poly << QPoint(x, yBottom) << QPoint(x + size, yBottom) << QPoint(x, apexY);
		}
		else if (type == ActiveHandle::Highlights)
		{
			poly << QPoint(x, yBottom) << QPoint(x - size, yBottom) << QPoint(x, apexY);
		}
		else // Midtones
		{
			poly << QPoint(x, apexY) << QPoint(x - size, yBottom) << QPoint(x + size, yBottom);
		}

		// All handles are transparent with gray borders by default.
		// Fill only the selected handle (the one affected by mouse wheel).
		p.setPen(QPen(Qt::gray, 1));
		if (type == m_SelectedHandle)
			p.setBrush(Qt::gray);
		else
			p.setBrush(Qt::NoBrush);

		p.drawPolygon(poly);
	}

	QMTFSlider::ActiveHandle QMTFSlider::hitTest(int x, int /*y*/) const
	{
		int xS = posFromValue(m_Shadows);
		int xM = posFromValue(m_Midtones);
		int xH = posFromValue(m_Highlights);

		// Give priority to Midtones if close
		if (std::abs(x - xM) < 8) return ActiveHandle::Midtones;
		if (std::abs(x - xS) < 8) return ActiveHandle::Shadows;
		if (std::abs(x - xH) < 8) return ActiveHandle::Highlights;

		return ActiveHandle::None;
	}

	void QMTFSlider::mousePressEvent(QMouseEvent* event)
	{
		if (event->button() == Qt::LeftButton)
		{
			emit sliderClicked();
			m_ActiveHandle = hitTest(event->pos().x(), event->pos().y());
			if (m_ActiveHandle != ActiveHandle::None)
			{
				m_SelectedHandle = m_ActiveHandle;
			}
		}
	}

	void QMTFSlider::mouseMoveEvent(QMouseEvent* event)
	{
		if (m_ActiveHandle != ActiveHandle::None)
		{
			double val = valueFromPos(event->pos().x());

			if (m_ActiveHandle == ActiveHandle::Shadows)
			{
				val = std::clamp(val, 0.0, m_Midtones); // Cannot pass midtones
				m_Shadows = val;
			}
			else if (m_ActiveHandle == ActiveHandle::Highlights)
			{
				val = std::clamp(val, m_Midtones, 1.0); // Cannot pass midtones
				m_Highlights = val;
			}
			else if (m_ActiveHandle == ActiveHandle::Midtones)
			{
				val = std::clamp(val, m_Shadows, m_Highlights);
				m_Midtones = val;
			}

			update();
			emit sliderMoved();
		}
	}

	void QMTFSlider::mouseReleaseEvent(QMouseEvent* event)
	{
		if (event->button() == Qt::LeftButton && m_ActiveHandle != ActiveHandle::None)
		{
			m_ActiveHandle = ActiveHandle::None;
			emit valuesChanged(m_Shadows, m_Midtones, m_Highlights);
		}
	}

	void QMTFSlider::wheelEvent(QWheelEvent* event)
	{
		if (m_SelectedHandle != ActiveHandle::None)
		{
			int delta = event->angleDelta().y();
			if (delta != 0)
			{
				double step = (delta > 0) ? 0.005 : -0.005;

				if (m_SelectedHandle == ActiveHandle::Shadows)
				{
					m_Shadows = std::clamp(m_Shadows + step, 0.0, m_Midtones);
					emit shadowsChanged(m_Shadows);
				}
				else if (m_SelectedHandle == ActiveHandle::Highlights)
				{
					m_Highlights = std::clamp(m_Highlights + step, m_Midtones, 1.0);
					emit highlightsChanged(m_Highlights);
				}
				else if (m_SelectedHandle == ActiveHandle::Midtones)
				{
					m_Midtones = std::clamp(m_Midtones + step, m_Shadows, m_Highlights);
					emit midtonesChanged(m_Midtones);
				}

				update();
				emit sliderMoved();
				emit valuesChanged(m_Shadows, m_Midtones, m_Highlights);
				event->accept();
				return;
			}
		}
		Inherited::wheelEvent(event);
	}



} // namespace DSS
