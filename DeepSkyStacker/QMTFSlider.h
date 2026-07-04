#pragma once
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
#include <QWidget>
#include <QColor>
class QKeyEvent;

namespace DSS
{
	class QMTFSlider : public QWidget
	{
		Q_OBJECT

		using Inherited = QWidget;

	public:
		explicit QMTFSlider(QWidget* parent = nullptr);

		float shadows() const { return static_cast<float>(m_Shadows); }
		float midtones() const { return static_cast<float>(m_Midtones); }
		float highlights() const { return static_cast<float>(m_Highlights); }
		QColor color() const { return m_Color; }

	public slots:
		void setShadows(double val);
		void setMidtones(double val);
		void setHighlights(double val);
		void setColor(const QColor& c);
		void setValues(double s, double m, double h);
		void clearSelectedHandle();

	signals:
		void valuesChanged(double s, double m, double h);
		void shadowsChanged(double s);
		void midtonesChanged(double m);
		void highlightsChanged(double h);
		void sliderMoved();
		void sliderClicked();

	protected:
		void paintEvent(QPaintEvent* event) override;
		void keyPressEvent(QKeyEvent* event) override;
		void mousePressEvent(QMouseEvent* event) override;
		void mouseMoveEvent(QMouseEvent* event) override;
		void mouseReleaseEvent(QMouseEvent* event) override;
		void wheelEvent(QWheelEvent* event) override;

	private:
		double m_Shadows;
		double m_Midtones;
		double m_Highlights;
		QColor m_Color;

		enum class ActiveHandle { None, Shadows, Midtones, Highlights };
		ActiveHandle m_ActiveHandle;
		ActiveHandle m_SelectedHandle;

		int posFromValue(double val) const;
		double valueFromPos(int pos) const;
		void drawHandle(QPainter& p, int x, ActiveHandle type);
		ActiveHandle hitTest(int x, int y) const;

		constexpr static double arrowIncrement = 0.00001;
		constexpr static double pageIncrement = 0.0001;
		constexpr static double wheelDivisor = 3.0;
	};

} // namespace DSS
