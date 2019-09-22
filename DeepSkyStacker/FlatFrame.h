#ifndef __FLATFRAME_H__
#define __FLATFRAME_H__

#include "BitmapExt.h"

/* ------------------------------------------------------------------- */

class CFlatNormalization
{
private :
	double						m_fMeanGray;
	double						m_fMeanRed;
	double						m_fMeanGreen;
	double						m_fMeanBlue;
	double						m_fMeanCyan;
	double						m_fMeanMagenta;
	double						m_fMeanGreen2;
	double						m_fMeanYellow;
	BOOL						m_bUseGray;

public :
	CFlatNormalization()
	{
		m_fMeanGray = 0;
		m_fMeanRed  = 0;
		m_fMeanGreen= 0;
		m_fMeanBlue = 0;
		m_fMeanCyan = 0;
		m_fMeanMagenta = 0;
		m_fMeanGreen2  = 0;
		m_fMeanYellow  = 0;

		m_bUseGray	= FALSE;
	};

	virtual ~CFlatNormalization()
	{
	};

	void	SetParameters(double fMeanGray)
	{
		m_fMeanGray = fMeanGray;
		m_bUseGray  = TRUE;
	};

	void	SetParameters(double fMeanRed, double fMeanGreen, double fMeanBlue)
	{
		m_fMeanRed   = fMeanRed;
		m_fMeanGreen = fMeanGreen;
		m_fMeanBlue  = fMeanBlue;
		m_bUseGray   = FALSE;
	};

	void	SetParameters(double fMeanCyan, double fMeanMagenta, double fMeanYellow, double fMeanGreen2)
	{
		m_fMeanCyan		= fMeanCyan;
		m_fMeanYellow	= fMeanYellow;
		m_fMeanGreen2	= fMeanGreen2;
		m_fMeanMagenta  = fMeanMagenta;
		m_bUseGray   = FALSE;
	};

	BOOL	UseGray()
	{
		return m_bUseGray;
	};

	void	Normalize(double & fAdjustGray, double fFlatGray, BAYERCOLOR BayerColor = BAYER_UNKNOWN)
	{
		switch (BayerColor)
		{
		case BAYER_UNKNOWN :
			fAdjustGray *= m_fMeanGray / max(1.0, fFlatGray);
			break;
		case BAYER_RED :
			fAdjustGray *= m_fMeanRed / max(1.0, fFlatGray);
			break;
		case BAYER_GREEN :
			fAdjustGray *= m_fMeanGreen / max(1.0, fFlatGray);
			break;
		case BAYER_BLUE	:
			fAdjustGray *= m_fMeanBlue / max(1.0, fFlatGray);
			break;
		case BAYER_CYAN :
			fAdjustGray *= m_fMeanCyan / max(1.0, fFlatGray);
			break;
		case BAYER_YELLOW :
			fAdjustGray *= m_fMeanYellow / max(1.0, fFlatGray);
			break;
		case BAYER_MAGENTA :
			fAdjustGray *= m_fMeanMagenta / max(1.0, fFlatGray);
			break;
		case BAYER_GREEN2 :
			fAdjustGray *= m_fMeanGreen2 / max(1.0, fFlatGray);
			break;
		};
		fAdjustGray = min(fAdjustGray, 255.0);
	};

	void	Normalize(double & fAdjustRed, double & fAdjustGreen, double & fAdjustBlue, double fFlatRed, double fFlatGreen, double fFlatBlue)
	{
		fAdjustRed   *= m_fMeanRed / max(1.0, fFlatRed);
		fAdjustGreen *= m_fMeanGreen / max(1.0, fFlatGreen);
		fAdjustBlue  *= m_fMeanBlue / max(1.0, fFlatBlue);

		fAdjustRed = min(fAdjustRed, 255.0);
		fAdjustGreen = min(fAdjustGreen, 255.0);
		fAdjustBlue = min(fAdjustBlue, 255.0);
	};
};

/* ------------------------------------------------------------------- */

class CFlatFrame
{
public :
	CSmartPtr<CMemoryBitmap>	m_pFlatFrame;
	CFlatNormalization			m_FlatNormalization;
	BOOL						m_bComputed;

public :
	CFlatFrame()
	{
		m_bComputed = FALSE;
	};

	virtual  ~CFlatFrame()
	{
	};

	BOOL	IsOk()
	{
		return m_pFlatFrame && m_pFlatFrame->IsOk();
	};

	BOOL	IsCFA()
	{
		return ::IsCFA(m_pFlatFrame);
	};

	void	Clear()
	{
		m_bComputed = FALSE;
		m_pFlatFrame.Release();
	};

	void	ComputeFlatNormalization(CDSSProgress * pProgress = NULL);
	BOOL	ApplyFlat(CMemoryBitmap * pTarget, CDSSProgress * pProgress = NULL);
};

/* ------------------------------------------------------------------- */

#endif // __FLATFRAME_H__