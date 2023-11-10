#pragma once

const		LONG				BUTTONTOOLBARSIZE = 48;

/* ------------------------------------------------------------------- */

typedef enum tagMASKEDBUTTONSTATUS
{
	MBS_UP				= 0,
	MBS_DOWN			= 1,
	MBS_HOT				= 2,
	MBS_DISABLED		= 3
}MASKEDBUTTONSTATUS;

typedef enum tagMASKEDBUTTONTYPE
{
	MBT_BUTTON			= 0,
	MBT_CHECKBOX		= 1
}MASKEDBUTTONTYPE;

class CMaskedButton
{
public :
	DWORD				m_dwID;
	MASKEDBUTTONTYPE	m_Type;
	MASKEDBUTTONSTATUS	m_Status;
	Bitmap *			m_pUp;
	Bitmap *			m_pDown;
	Bitmap *			m_pHot;
	Bitmap *			m_pDisabled;
	BOOL				m_bShowTooltip;
	CRect				m_rcButton;
	CString				m_strTooltip;

private :
	void	CopyFrom(const CMaskedButton & mb)
	{
		m_dwID		= mb.m_dwID;
		m_Status	= mb.m_Status;
		m_Type		= mb.m_Type;
		m_pUp		= mb.m_pUp;
		m_pDown		= mb.m_pDown;
		m_pHot		= mb.m_pHot;
		m_pDisabled = mb.m_pDisabled;
		m_rcButton	= mb.m_rcButton;
		m_strTooltip= mb.m_strTooltip;
		m_bShowTooltip = mb.m_bShowTooltip;
	};

public :
	CMaskedButton(DWORD dwID = 0)
	{
		m_dwID			= dwID;
		m_Status		= MBS_UP;
		m_Type			= MBT_BUTTON;
		m_pUp			= nullptr;
		m_pDown			= nullptr;
		m_pHot			= nullptr;
		m_pDisabled		= nullptr;
		m_bShowTooltip  = FALSE;
	};

	CMaskedButton(const CMaskedButton & mb)
	{
		CopyFrom(mb);
	};

	CMaskedButton & operator = (const CMaskedButton & mb)
	{
		CopyFrom(mb);
		return (*this);
	};

	virtual ~CMaskedButton()
	{
	};

	void	Free()
	{
		if (m_pUp)
			delete m_pUp;
		if (m_pDown)
			delete m_pDown;
		if (m_pHot)
			delete m_pHot;
		if (m_pDisabled)
			delete m_pDisabled;

		m_pUp			= nullptr;
		m_pDown			= nullptr;
		m_pHot			= nullptr;
		m_pDisabled		= nullptr;
	};

	void	Init(UINT nBitmapUp, UINT nBitmapDown, UINT nBitmapHot, UINT nBitmapDisabled, UINT nBitmapMask);
	void	SetTooltip(LPCTSTR szTooltip)
	{
		m_strTooltip = szTooltip;
	};
};

/* ------------------------------------------------------------------- */

typedef std::vector<CMaskedButton>		MASKEDBUTTONVECTOR;

#define MBI1(BaseName, Extension)	IDB_BUTTON##BaseName##Extension
#define MBI(BaseName) MBI1(BaseName, _UP), MBI1(BaseName, _DOWN), MBI1(BaseName, _HOT), MBI1(BaseName, _DISABLED)

class CButtonToolbar;

class CButtonToolbarSink
{
public :
	CButtonToolbarSink() {};
	virtual ~CButtonToolbarSink() {};

	virtual void ButtonToolbar_OnCheck([[maybe_unused]] DWORD dwID, [[maybe_unused]] CButtonToolbar * pButtonToolbar) {};
	virtual void ButtonToolbar_OnClick([[maybe_unused]] DWORD dwID, [[maybe_unused]] CButtonToolbar * pButtonToolbar) {};
	virtual void ButtonToolbar_OnRClick([[maybe_unused]] DWORD dwID, [[maybe_unused]] CButtonToolbar * pButtonToolbar) {};
};

class CButtonToolbar
{
protected :
	CButtonToolbarSink *	m_pSink;
	CRect					m_rcToolbar;
	CPoint					m_ptTopLeft;
	MASKEDBUTTONVECTOR		m_vButtons;
	BOOL					m_bTop;
	BOOL					m_bMouseIn;
	BOOL					m_bButtonDown;

protected :
	void		UpdateRect()
	{
		m_rcToolbar.top = m_rcToolbar.left = 0;
		m_rcToolbar.right = BUTTONTOOLBARSIZE+2;
		m_rcToolbar.bottom = (LONG)m_vButtons.size()*BUTTONTOOLBARSIZE+2;
	};

	void		ResetHot()
	{
		for  (LONG i = 0;i<m_vButtons.size();i++)
		{
			if (m_vButtons[i].m_Status == MBS_HOT)
				m_vButtons[i].m_Status		 = MBS_UP;
		};
	};

	void		ComputeTooltipSize(LPCTSTR szTooltip, CRect & rcTooltip);
	BOOL		GetTooltipRect(CRect & rcTooltip, CString & strTooltip);

	void		ResetTooltip()
	{
		for  (LONG i = 0;i<m_vButtons.size();i++)
			m_vButtons[i].m_bShowTooltip = FALSE;
	};

	BOOL		GetTooltipButton(LONG & lIndice)
	{
		BOOL		bResult = FALSE;

		for (LONG i = 0;i<m_vButtons.size() && !bResult;i++)
		{
			if (m_vButtons[i].m_bShowTooltip)
			{
				lIndice = i;
				bResult = TRUE;
			};
		};

		return bResult;
	};

	BOOL		GetHotButton(LONG & lIndice)
	{
		BOOL		bResult = FALSE;

		for (LONG i = 0;i<m_vButtons.size() && !bResult;i++)
		{
			if (m_vButtons[i].m_Status == MBS_HOT)
			{
				lIndice = i;
				bResult = TRUE;
			};
		};

		return bResult;
	};

	BOOL		GetButton(LONG lX, LONG lY, LONG & lIndice)
	{
		BOOL		bResult = FALSE;
		CPoint		pt(lX, lY);

		for (LONG i = 0;i<m_vButtons.size() && !bResult;i++)
		{
			if (m_vButtons[i].m_rcButton.PtInRect(pt))
			{
				lIndice = i;
				bResult = TRUE;
			};
		};

		return bResult;
	};

public :
	CButtonToolbar()
	{
		m_bTop			= FALSE;
		m_bMouseIn		= FALSE;
		m_bButtonDown	= FALSE;
		m_pSink			= nullptr;
	};

	virtual ~CButtonToolbar()
	{
		for (LONG i = 0;i<m_vButtons.size();i++)
			m_vButtons[i].Free();
	};

	void	SetSink(CButtonToolbarSink * pSink)
	{
		m_pSink = pSink;
	};

	virtual Image*	GetImage();
	virtual Image*	GetTooltipImage(CRect & rcTooltip);

	virtual	BOOL	GetRect(CRect & rcToolbar)
	{
		rcToolbar = m_rcToolbar;
		rcToolbar.OffsetRect(m_ptTopLeft);
		return !m_rcToolbar.IsRectEmpty();
	};

	void			AddCheck(DWORD dwID, UINT nBitmapUp, UINT nBitmapDown, UINT nBitmapHot, UINT nBitmapDisabled, UINT nBitmapMask, LPCTSTR szTooltip = nullptr)
	{
		CMaskedButton			mb(dwID);

		mb.Init(nBitmapUp, nBitmapDown, nBitmapHot, nBitmapDisabled, nBitmapMask);
		mb.m_Type = MBT_CHECKBOX;
		if (szTooltip)
			mb.SetTooltip(szTooltip);
		m_vButtons.push_back(mb);

		UpdateRect();
	};
	void			AddButton(DWORD dwID, UINT nBitmapUp, UINT nBitmapDown, UINT nBitmapHot, UINT nBitmapDisabled, UINT nBitmapMask, LPCTSTR szTooltip = nullptr)
	{
		CMaskedButton			mb(dwID);

		mb.Init(nBitmapUp, nBitmapDown, nBitmapHot, nBitmapDisabled, nBitmapMask);
		mb.m_Type = MBT_BUTTON;
		if (szTooltip)
			mb.SetTooltip(szTooltip);
		m_vButtons.push_back(mb);

		UpdateRect();
	};

	void			Enable(DWORD dwID, BOOL bEnable = TRUE)
	{
		BOOL		bFound = FALSE;

		for (LONG i = 0;i<m_vButtons.size() && !bFound;i++)
		{
			if (dwID == m_vButtons[i].m_dwID)
			{
				bFound = TRUE;
				if (bEnable)
					m_vButtons[i].m_Status = MBS_UP;
				else
					m_vButtons[i].m_Status = MBS_DISABLED;
			};
		};
	};

	void			Check(DWORD dwID, BOOL bCheck = TRUE)
	{
		BOOL		bFound = FALSE;

		for (LONG i = 0;i<m_vButtons.size() && !bFound;i++)
		{
			if (dwID == m_vButtons[i].m_dwID)
			{
				bFound = TRUE;
				if (m_vButtons[i].m_Type == MBT_CHECKBOX)
				{
					if (bCheck)
						m_vButtons[i].m_Status = MBS_DOWN;
					else
						m_vButtons[i].m_Status = MBS_UP;
				};
			};
		};
	};

	BOOL			IsChecked(DWORD dwID)
	{
		BOOL		bResult = FALSE;
		BOOL		bFound = FALSE;

		for (LONG i = 0;i<m_vButtons.size() && !bFound;i++)
		{
			if (dwID == m_vButtons[i].m_dwID)
			{
				bFound = TRUE;
				if (m_vButtons[i].m_Type == MBT_CHECKBOX)
					bResult = m_vButtons[i].m_Status == MBS_DOWN;
			};
		};

		return bResult;
	};

	virtual BOOL	OnMouseEnter(LONG lX, LONG lY);
	virtual BOOL	OnMouseMove(LONG lX, LONG lY);
	virtual BOOL	OnMouseLeave();
	virtual	BOOL	OnLButtonDown(LONG lX, LONG lY);
	virtual	BOOL	OnLButtonUp(LONG lX, LONG lY);
	virtual	BOOL	OnRButtonDown(LONG lX, LONG lY);
};

/* ------------------------------------------------------------------- */

