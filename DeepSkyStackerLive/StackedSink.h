#ifndef __STACKEDSINK_H__
#define __STACKEDSINK_H__

/* ------------------------------------------------------------------- */

#include "DSSProgress.h"
#include "DSSTools.h"

class CStackedSink : public CWndImageSink
{
private :
	CPointExt		m_pt1,
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

	void			SetFootprint(CPointExt pt1, CPointExt pt2, CPointExt pt3, CPointExt pt4);
	void			ClearFootprint();
};

/* ------------------------------------------------------------------- */

#endif // __STACKEDSINK_H__