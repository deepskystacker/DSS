#if !defined(QLINEARGRADIENTCTRL_HEADER)
#define QLINEARGRADIENTCTRL_HEADER

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// QLinearGradientCtrl : header file
//
#include <zexcept.h>

class QColor;
class QString;

#include <QWidget>
#include <QGradient>


#define GCW_AUTO -1

#define GC_SELCHANGE			1
#define GC_PEGMOVE				2
#define GC_PEGMOVED				3
#define GC_PEGREMOVED			4
#define GC_CREATEPEG			5
#define GC_EDITPEG				6
#define GC_CHANGE				7

class QLinearGradientCtrlImpl;

/////////////////////////////////////////////////////////////////////////////
// QLinearGradientCtrl window

class QLinearGradientCtrl : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QLinearGradient gradient READ gradient WRITE setGradient)
	Q_PROPERTY(int gradientWidth READ orientation WRITE setGradientWidth)
	Q_PROPERTY(QLinearGradientCtrl::Orientation orientation READ orientation WRITE setOrientation)
	Q_PROPERTY(int selectedIndex READ selectedIndex WRITE setSelectedIndex NOTIFY pegSelChanged)
	Q_PROPERTY(QGradientStop selectedStop READ selectedStop)
	Q_PROPERTY(bool pegsOnLeftOrBottom READ pegsOnLeftOrBottom WRITE setPegsOnLeftOrBottom)
	Q_PROPERTY(bool pegsOnRightOrTop READ pegsOnRightOrTop WRITE setPegsOnRightOrTop)
	Q_PROPERTY(bool showToolTips READ showToolTips WRITE setShowToolTips)
	Q_PROPERTY(QString toolTipFormat READ toolTipFormat WRITE setToolTipFormat)

typedef QWidget
		Inherited;

	// Construction
public:
	QLinearGradientCtrl(QWidget* parent= nullptr);
	// BOOL Create(const RECT& rect, CWnd* pParentWnd, UINT nID);

	virtual ~QLinearGradientCtrl();

	enum Orientation
	{
		ForceHorizontal,
		ForceVertical,
		Auto
	};

	// Attributes
public:
	QLinearGradient gradient() { return m_Gradient; };
	QLinearGradientCtrl &
		setGradient(QLinearGradient const& src);

	int		gradientWidth() const { return m_Width; };
	QLinearGradientCtrl &
		setGradientWidth(int iWidth) { assert(iWidth >= 1 || iWidth == -1); m_Width = iWidth; return *this; };

	Orientation orientation() const { return m_Orientation; };
	QLinearGradientCtrl &
		setOrientation(Orientation orientation) { m_Orientation = orientation; return *this; };

	int		selectedIndex() const { return selectedPeg; };
	int		setSelectedIndex(int iSel);

	QGradientStop selectedStop() const;

	bool	pegsOnLeftOrBottom() const { return m_LeftDownSide; };
	QLinearGradientCtrl &
		setPegsOnLeftOrBottom(bool value) { m_LeftDownSide = value; return *this; };

	bool	pegsOnRightOrTop() const {return m_RightUpSide; };
	QLinearGradientCtrl &
		setPegsOnRightOrTop(bool value) { m_RightUpSide = value; return *this; };

	QString toolTipFormat() const { return m_ToolTipFormat; };
	void	setToolTipFormat(const QString format) { m_ToolTipFormat = format; };

	bool	showToolTips() { return m_showToolTips; };
	void	setShowToolTips(bool Show = true);

	// Operations
public:
	void	deleteSelected(bool bUpdate);
	int		moveSelected(qreal newpos, bool bUpdate);
	QColor	setSelectedPegColour(QColor newColour, bool bUpdate);

	// Internals
protected:
	//void GetPegRgn(CRgn *rgn);
	//]void SendBasicNotification(UINT code, CPeg const& peg, int index);

	QLinearGradient	m_Gradient;
	int			m_Width;

	int			selectedPeg;
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

	void	addAnchorStops();
	QColor	colourFromPoint(const QPoint & point);
	void	drawGradient(QPainter & painter);
	void	drawEndPegs(QPainter & painter);
	void	drawPeg(QPainter & painter, QPoint point, QColor colour, int direction);
	void	drawPegs(QPainter & painter);
	void	drawSelPeg(QPainter & painter, int peg);
	void	drawSelPeg(QPainter & painter, QPoint point, int direction);
	bool	isVertical();
	int		pointFromPos(qreal pos);
	qreal	posFromPoint(int point);
	int		getDrawWidth();
	void	getPegRect(int index, QRect *rect, bool right);
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
	// void showEvent(QShowEvent *event) override;


};

#endif
