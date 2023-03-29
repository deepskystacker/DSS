#ifndef __STACKEDSINK_H__
#define __STACKEDSINK_H__

/* ------------------------------------------------------------------- */

#include "WndImage.h"

class CStackedSink : public CWndImageSink
{
private :
	QPointF		m_pt1,
				m_pt2,
				m_pt3,
				m_pt4;

private :

public :
	CStackedSink()
	{
		m_pt1 = m_pt2 = m_pt3 = m_pt4;
	};
	virtual ~CStackedSink() {};

	virtual Image *	GetOverlayImage(CRect & rcClient);

	void			SetFootprint(QPointF const& pt1, QPointF const& pt2, QPointF const& pt3, QPointF const& pt4);
	void			ClearFootprint();
};

/* ------------------------------------------------------------------- */

#endif // __STACKEDSINK_H__