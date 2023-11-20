#pragma once
/****************************************************************************
**
** Copyright (C) 2020, 2022 David C. Partridge
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
// QLinearGradientCtrl : header file
//
#define GCW_AUTO -1

#define GC_SELCHANGE			1
#define GC_PEGMOVE				2
#define GC_PEGMOVED				3
#define GC_PEGREMOVED			4
#define GC_CREATEPEG			5
#define GC_EDITPEG				6
#define GC_CHANGE				7

//class QLinearGradientCtrlImpl;

/////////////////////////////////////////////////////////////////////////////
// QLinearGradientCtrl window

class QLinearGradientCtrl : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QLinearGradient gradient READ gradient WRITE setGradient)
	Q_PROPERTY(int gradientWidth READ gradientWidth WRITE setGradientWidth)
	Q_PROPERTY(Orientation orientation READ orientation WRITE setOrientation)
	Q_PROPERTY(int selectedPeg READ selected WRITE setSelected NOTIFY pegSelChanged)
	Q_PROPERTY(QGradientStop selectedStop READ selectedStop)
	Q_PROPERTY(bool pegsOnLeftOrBottom READ pegsOnLeftOrBottom WRITE setPegsOnLeftOrBottom)
	Q_PROPERTY(bool pegsOnRightOrTop READ pegsOnRightOrTop WRITE setPegsOnRightOrTop)
	Q_PROPERTY(bool showToolTips READ showToolTips WRITE setShowToolTips)

typedef QWidget
		Inherited;

	// Construction
public:
	QLinearGradientCtrl(QWidget* parent= nullptr, QColor start = Qt::black, QColor end = Qt::white) ;
	// BOOL Create(const RECT& rect, CWnd* pParentWnd, UINT nID);

	virtual ~QLinearGradientCtrl();

	enum class Orientation
	{
		ForceHorizontal,
		ForceVertical,
		Auto
	};

	// Attributes
public:
	inline QLinearGradient& gradient() { return m_Gradient; };
	QLinearGradientCtrl &
		setGradient(QLinearGradient const& src);

	inline int gradientWidth() const { return m_Width; };
	inline QLinearGradientCtrl &
		setGradientWidth(int iWidth)
	{
		assert(iWidth >= 1 || iWidth == -1);
		m_Width = iWidth;
		return *this;
	};

	inline Orientation orientation() const { return m_Orientation; };
	inline QLinearGradientCtrl &
		setOrientation(Orientation orientation) { m_Orientation = orientation; return *this; };

	inline int	selected() const { return selectedPeg; };
	int	setSelected(int iSel);

	QGradientStop selectedStop() const;

	inline bool	pegsOnLeftOrBottom() const { return m_LeftDownSide; };
	inline QLinearGradientCtrl &
		setPegsOnLeftOrBottom(bool value)
	{
		m_RightUpSide = !value;
		m_LeftDownSide = value;
		return *this;
	};

	inline bool	pegsOnRightOrTop() const {return m_RightUpSide; };
	inline QLinearGradientCtrl &
		setPegsOnRightOrTop(bool value)
	{ 
		m_RightUpSide = value;
		m_LeftDownSide = !value;
	  return *this;
	};

	QString toolTipFormat() const { return m_ToolTipFormat; };
	void	setToolTipFormat(const QString format) { m_ToolTipFormat = format; };

	bool	showToolTips() { return m_showToolTips; };
	void	setShowToolTips(bool Show = true);

	// Operations
public:
	void	deleteSelected(bool bUpdate);
	int		moveSelected(qreal newpos, bool bUpdate);
	QColor	setSelectedPegColour(QColor newColour, bool bUpdate);
	void	setColorAt(double pos, QColor colour);
	int		setPeg(int index, qreal position);

	// Internals
protected:
	//void GetPegRgn(CRgn *rgn);
	//]void SendBasicNotification(UINT code, CPeg const& peg, int index);

	QLinearGradient	m_Gradient;
	int			m_Width;

	int	selectedPeg;
	int lastSelectedPeg;
	int			m_LastPos;
	QPoint		m_MouseDown;

	bool		m_showToolTips;
	QString		m_ToolTipFormat;
	QRect		clientRect;

	enum Orientation m_Orientation;

	int		startPegStop;
	int		endPegStop;

	QGradientStops stops;

	bool m_LeftDownSide;
	bool m_RightUpSide;

	QColor	colourFromPoint(const QPoint & point);
	QRect	gradientRect() const;
	void	drawGradient(QPainter & painter);
	void	drawEndPegs(QPainter & painter);
	void	drawPeg(QPainter & painter, QPoint point, QColor colour, int direction);
	void	drawPegs(QPainter & painter);
	void	drawSelPeg(QPainter & painter, int peg);
	void	drawSelPeg(QPainter & painter, QPoint point, int direction);
	bool	isVertical() const;
	int		pointFromPos(qreal pos);
	qreal	posFromPoint(int point);
	int		getDrawWidth() const;
	QPolygon getPegPoly(int index);
	void	getPegRect(int index, QRect *rect, bool right);
	QRegion getPegRegion(short peg);
	QRegion getPegRegion();
	int		getPegIndent(int index);
	int		setPeg(int index, QColor colour, qreal position);

// Implementation
public:

signals:
	void pegMove(int peg);
	void pegMoved(int peg);
	void pegSelChanged(int peg);
	void pegRemoved(int peg);
	void createPeg(qreal pos, QColor colour);
	void editPeg(int peg);
	void pegChanged(int peg);


	// Event handler functions inherited from QWidget
protected:
	void keyPressEvent(QKeyEvent * event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void mouseDoubleClickEvent(QMouseEvent *event) override;
	void paintEvent(QPaintEvent *event) override;
	void resizeEvent(QResizeEvent *event) override;
	//void showEvent(QShowEvent *event) override;


};
