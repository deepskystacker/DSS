#pragma once

#include "WndImage.h"
#include "Stars.h"
#include "BilinearParameters.h"
#include "MatchingStars.h"
#include "GrayBitmap.h"
#include "DSSCommon.h"

/* ------------------------------------------------------------------- */
extern bool     g_bShowRefStars;

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
	bool			m_bInSelecting;
	double			m_fXStart, m_fYStart;
	double			m_fXEnd, m_fYEnd;
	bool			m_bShowDrizzle;

private :
	void	UpdateSelectRect();

	HCURSOR			GetCursorFromMode(SELECTRECTMODE Mode);
	SELECTRECTMODE	GetModeFromPosition(long lX, long lY);
	void			GetDrizzleRectangles(CRect & rc2xDrizzle, CRect & rc3xDrizzle);

public :
	CSelectRectSink()
	{
		m_rcSelect.SetRectEmpty();
		m_bInSelecting	= false;
		m_Mode			= SRM_NONE;
		m_bShowDrizzle	= false;
        m_fXStart       = 0;
        m_fYStart       = 0;
        m_fXEnd         = 0;
        m_fYEnd         = 0;
	};
	virtual ~CSelectRectSink() {};

	void	ShowDrizzleRectangles(bool bShow = true)
	{
		m_bShowDrizzle = bShow;
	};

	virtual bool	Image_OnMouseMove(long lX, long lY) override;
	virtual bool	Image_OnLButtonDown(long lX, long lY) override;
	virtual bool	Image_OnLButtonUp(long lX, long lY) override;

	virtual Image *	GetOverlayImage(CRect & rcClient) override;

	bool	GetSelectRect(CRect & rcSelect)
	{
		rcSelect = m_rcSelect;

		return !rcSelect.IsRectEmpty();
	};
};

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

	void	InitGrid(STARVECTOR & vStars);
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

class CMemoryBitmap;

class CEditStarsSink : public CWndImageSink
{
private:
	CString						m_strFileName;
	STARVECTOR					m_vStars;
	STARVECTOR					m_vRefStars;
	CBilinearParameters			m_Transformation;
	VOTINGPAIRVECTOR			m_vVotedPairs;
	std::shared_ptr<CMemoryBitmap> m_pBitmap;
	QPointF					m_ptCursor;
	CGrayBitmap					m_GrayBitmap; // CGrayBitmapT<double>
	EDITSTARACTION				m_Action;
	CStar						m_AddedStar;
	int							m_lRemovedIndice;
	bool						m_bRemoveComet;
	bool						m_bCometMode;
	double						m_fXComet, m_fYComet;
	bool						m_bComet;
	double						m_fLightBkgd;
	bool						m_bDirty;
	double						m_fScore;
	int							m_lNrStars;
	double						m_fFWHM;
	double						m_fBackground;
	CQualityGrid				m_QualityGrid;

private:
	template <bool Refstar>
	bool isStarVoted(const int star)
	{
		bool bResult = false;
		if (g_bShowRefStars)
		{
			if (!m_vVotedPairs.empty())
			{
				//for (size_t i = 0; i < m_vVotedPairs.size() && !bResult; i++)
#pragma warning (suppress:4189)
				for (const auto& votedPair : m_vVotedPairs)
				{
					if constexpr (Refstar)
						if (star == votedPair.m_RefStar)
						{
							bResult = true;
							break;
						}
					else
						if (star == votedPair.m_TgtStar)
						{
							bResult = true;
							break;
						}
				}
			}
			else
				bResult = true;
		}
		else
			bResult = true;

		return bResult;
	}

	bool IsRefStarVoted(const int lStar)
	{
		return this->isStarVoted<true>(lStar);
/*		bool bResult = false;

		if (g_bShowRefStars)
		{
			if (!m_vVotedPairs.empty())
			{
				for (size_t i = 0; i < m_vVotedPairs.size() && !bResult; i++)
				{
					if (lStar == m_vVotedPairs[i].m_RefStar)
						bResult = true;
				}
			}
			else
				bResult = true;
		}
		else
			bResult = true;

		return bResult;*/
	}

	bool IsTgtStarVoted(const int lStar)
	{
		return this->isStarVoted<false>(lStar);
/*		bool bResult = false;

		if (g_bShowRefStars)
		{
			if (!m_vVotedPairs.empty())
			{
				for (size_t i = 0; i < m_vVotedPairs.size() && !bResult; i++)
				{
					if (lStar == m_vVotedPairs[i].m_TgtStar)
						bResult = true;
				}
			}
			else
				bResult = true;
		}
		else
			bResult = true;

		return bResult;*/
	}

	void	InitGrayBitmap(CRect & rc);
	void	DetectStars(const QPointF & pt, CRect & rc, STARVECTOR & vStars);

	void	ComputeOverallQuality()
	{
		m_fScore	= 0.0;
		m_lNrStars	= 0;
		m_fFWHM		= 0;
		for (size_t i = 0; i < m_vStars.size(); i++)
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
		m_bDirty		= false;
		m_bCometMode	= false;
		m_bComet		= false;
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

	void SetBitmap(std::shared_ptr<CMemoryBitmap> pBitmap)
	{
		m_pBitmap = pBitmap;
		m_GrayBitmap.Init(RCCHECKSIZE+1, RCCHECKSIZE+1);
		m_bDirty = false;
		m_fBackground = 0;
		if (static_cast<bool>(m_pBitmap))
			ComputeBackgroundValue();
	}

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

	void	SetCometMode(bool bCometMode)
	{
		m_bCometMode = bCometMode;
	};

	bool	IsDirty()
	{
		return m_bDirty;
	};

	// Message handling
	virtual bool	Image_OnMouseLeave() override;
	virtual bool	Image_OnMouseMove(long lX, long lY) override;
	virtual bool	Image_OnLButtonDown(long lX, long lY) override;
	virtual bool	Image_OnLButtonUp(long lX, long lY) override;

	virtual Image *	GetOverlayImage(CRect & rcClient);
};
