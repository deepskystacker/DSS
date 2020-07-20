#ifndef __IMAGESINKS_H__
#define __IMAGESINKS_H__

/* ------------------------------------------------------------------- */

typedef enum tagSELECTRECTMODE
{
	SRM_NONE			= 0,
	SRM_CREATE			= 1,
	SRM_MOVE			= 2,
	SRM_MOVETOP			= 3,
	SRM_MOVEBOTTOM		= 4,
	SRM_MOVELEFT		= 5,
	SRM_MOVERIGHT		= 6,
	SRM_MOVETOPLEFT		= 7,
	SRM_MOVETOPRIGHT	= 8,
	SRM_MOVEBOTTOMLEFT	= 9,
	SRM_MOVEBOTTOMRIGHT = 10
}SELECTRECTMODE;

class CSelectRectSink : public CWndImageSink
{
private :
	SELECTRECTMODE	m_Mode;
	CRect			m_rcSelect;
	CRect			m_rcStart;
	BOOL			m_bInSelecting;
	double			m_fXStart, m_fYStart;
	double			m_fXEnd, m_fYEnd;
	BOOL			m_bShowDrizzle;

private :
	void	UpdateSelectRect();

	HCURSOR			GetCursorFromMode(SELECTRECTMODE Mode);
	SELECTRECTMODE	GetModeFromPosition(LONG lX, LONG lY);
	void			GetDrizzleRectangles(CRect & rc2xDrizzle, CRect & rc3xDrizzle);

public :
	CSelectRectSink()
	{
		m_rcSelect.SetRectEmpty();
		m_bInSelecting	= FALSE;
		m_Mode			= SRM_NONE;
		m_bShowDrizzle	= FALSE;
        m_fXStart       = 0;
        m_fYStart       = 0;
        m_fXEnd         = 0;
        m_fYEnd         = 0;
	};
	virtual ~CSelectRectSink() {};

	void	ShowDrizzleRectangles(BOOL bShow = TRUE)
	{
		m_bShowDrizzle = bShow;
	};

	virtual BOOL	Image_OnMouseMove(LONG lX, LONG lY);
	virtual BOOL	Image_OnLButtonDown(LONG lX, LONG lY);
	virtual BOOL	Image_OnLButtonUp(LONG lX, LONG lY);

	virtual Image *	GetOverlayImage(CRect & rcClient);

	BOOL	GetSelectRect(CRect & rcSelect)
	{
		rcSelect = m_rcSelect;

		return !rcSelect.IsRectEmpty();
	};
};

/* ------------------------------------------------------------------- */

#include "Stars.h"
#include "MatchingStars.h"

/* ------------------------------------------------------------------- */

class CDelaunayTriangle
{
public :
	PointF			pt1, pt2, pt3;
	Color			cr1, cr2, cr3;

private:
	void	CopyFrom(const CDelaunayTriangle & right)
	{
		pt1 = right.pt1;
		pt2 = right.pt2;
		pt3 = right.pt3;
		cr1 = right.cr1;
		cr2 = right.cr2;
		cr3 = right.cr3;
	};

public:
	CDelaunayTriangle()
	{
	};
	~CDelaunayTriangle()
	{
	};

	CDelaunayTriangle(const CDelaunayTriangle & right)
	{
		CopyFrom(right);
	};

	CDelaunayTriangle & operator = (const CDelaunayTriangle & right)
	{
		CopyFrom(right);
		return (*this);
	};
};

typedef std::vector<CDelaunayTriangle>	DELAUNAYTRIANGLEVECTOR;


class CQualityGrid
{
public :
	DELAUNAYTRIANGLEVECTOR	m_vTriangles;
	double					m_fMean;
	double					m_fStdDev;

private:
public :
	CQualityGrid()
	{
		m_fMean		= 0.0;
		m_fStdDev	= 0.0;
	};

	void	InitGrid(STARVECTOR & vStars, LONG lWidth, LONG lHeight);
	void	Clear()
	{
		m_vTriangles.clear();
	};

	bool	Empty()
	{
		return !m_vTriangles.size();
	};
};

/* ------------------------------------------------------------------- */

typedef enum tagEDITSTARACTION
{
	ESA_NONE			= 0,
	ESA_ADDSTAR			= 1,
	ESA_REMOVESTAR		= 2,
	ESA_SETCOMET		= 3,
	ESA_RESETCOMET		= 4
}EDITSTARACTION;

class CEditStarsSink : public CWndImageSink
{
private :
	CString						m_strFileName;
	STARVECTOR					m_vStars;
	STARVECTOR					m_vRefStars;
	CBilinearParameters			m_Transformation;
	VOTINGPAIRVECTOR			m_vVotedPairs;
	CSmartPtr<CMemoryBitmap>	m_pBitmap;
	CPointExt					m_ptCursor;
	CGrayBitmap					m_GrayBitmap;
	EDITSTARACTION				m_Action;
	CStar						m_AddedStar;
	LONG						m_lRemovedIndice;
	BOOL						m_bRemoveComet;
	BOOL						m_bCometMode;
	double						m_fXComet, m_fYComet;
	BOOL						m_bComet;
	double						m_fLightBkgd;
	BOOL						m_bDirty;
	double						m_fScore;
	LONG						m_lNrStars;
	double						m_fFWHM;
	double						m_fBackground;
	CQualityGrid				m_QualityGrid;

private :

	BOOL	IsRefStarVoted(LONG lStar)
	{
		BOOL			bResult = FALSE;

		if (g_bShowRefStars)
		{
			if (m_vVotedPairs.size())
			{
				for (LONG i = 0;i<m_vVotedPairs.size() && !bResult;i++)
				{
					if (lStar == m_vVotedPairs[i].m_RefStar)
						bResult = TRUE;
				};
			}
			else
				bResult = TRUE;
		}
		else
			bResult = TRUE;

		return bResult;
	};

	BOOL	IsTgtStarVoted(LONG lStar)
	{
		BOOL			bResult = FALSE;

		if (g_bShowRefStars)
		{
			if (m_vVotedPairs.size())
			{
				for (LONG i = 0;i<m_vVotedPairs.size() && !bResult;i++)
				{
					if (lStar == m_vVotedPairs[i].m_TgtStar)
						bResult = TRUE;
				};
			}
			else
				bResult = TRUE;
		}
		else
			bResult = TRUE;

		return bResult;
	};

	void	InitGrayBitmap(CRect & rc);
	void	DetectStars(const CPointExt & pt, CRect & rc, STARVECTOR & vStars);

	void	ComputeOverallQuality()
	{
		m_fScore	= 0.0;
		m_lNrStars	= 0;
		m_fFWHM		= 0;
		for (LONG i = 0;i<m_vStars.size();i++)
		{
			if (!m_vStars[i].m_bRemoved)
			{
				m_fScore += m_vStars[i].m_fQuality;
				m_lNrStars++;
				m_fFWHM += m_vStars[i].m_fMeanRadius* 2.35/1.5;
			};
		};
		if (m_lNrStars)
			m_fFWHM /= m_lNrStars;
	};

	void	ComputeBackgroundValue();

	void	DrawQualityGrid(Graphics * pGraphics, CRect & rcClient);
public :
	CEditStarsSink()
	{
		m_Action		= ESA_NONE;
		m_bDirty		= FALSE;
		m_bCometMode	= FALSE;
		m_bComet		= FALSE;
		m_fLightBkgd	= 0;
		m_fScore		= 0;
		m_lNrStars		= 0;
		m_fBackground	= 0;
        m_fXComet       = 0;
        m_fYComet       = 0;
        m_fFWHM         = 0;
        m_lRemovedIndice = 0;
        m_bRemoveComet = false;
	};
	virtual ~CEditStarsSink() {};

	void	SetBitmap(CMemoryBitmap * pBitmap)
	{
		m_pBitmap = pBitmap;
		m_GrayBitmap.Init(RCCHECKSIZE+1, RCCHECKSIZE+1);
		m_bDirty = FALSE;
		m_fBackground = 0;
		if (m_pBitmap)
			ComputeBackgroundValue();
	};

	void	SetRefStars(STARVECTOR const& Stars)
	{
		if (g_bShowRefStars)
		{
			m_vRefStars = Stars;
			std::sort(m_vRefStars.begin(), m_vRefStars.end(), CompareStarLuminancy);
		};
	};

	void	ClearRefStars()
	{
		m_vRefStars.clear();
	};

	void	SetLightFrame(LPCTSTR szFileName);

	void	SaveRegisterSettings();

	void	SetTransformation(const CBilinearParameters & Transformation, const VOTINGPAIRVECTOR & vVotedPairs)
	{
		if (g_bShowRefStars)
		{
			m_Transformation = Transformation;
			m_vVotedPairs    = vVotedPairs;
		};
	};

	void	SetCometMode(BOOL bCometMode)
	{
		m_bCometMode = bCometMode;
	};

	BOOL	IsDirty()
	{
		return m_bDirty;
	};

	// Message handling
	virtual BOOL	Image_OnMouseLeave();
	virtual BOOL	Image_OnMouseMove(LONG lX, LONG lY);
	virtual BOOL	Image_OnLButtonDown(LONG lX, LONG lY);
	virtual BOOL	Image_OnLButtonUp(LONG lX, LONG lY);

	virtual Image *	GetOverlayImage(CRect & rcClient);
};

/* ------------------------------------------------------------------- */

#endif // __IMAGESINKS_H__