#pragma once
#include "dss_settings.h"
#include "ControlPos.h"
#include "ProcessSettingsSheet.h"
#include "RGBTab.h"
#include "LuminanceTab.h"
#include "SaturationTab.h"
#include "ImageSinks.h"
#include "commonresource.h"

/* ------------------------------------------------------------------- */

class QWidget;

class CValuedRect
{
public :
	CRect					m_rc;
	double					m_fScore;

private :
	void	CopyFrom(const CValuedRect & vr)
	{
		m_rc = vr.m_rc;
		m_fScore = vr.m_fScore;
	};

public :
	CValuedRect()
	{
		m_fScore = 0.0;
	};

	CValuedRect(const CValuedRect & vr)
	{
		CopyFrom(vr);
	};

	virtual ~CValuedRect() {};

	CValuedRect & operator = (const CValuedRect & vr)
	{
		CopyFrom(vr);
		return (*this);
	};

	bool operator < (const CValuedRect & vr)
	{
		return m_fScore < vr.m_fScore;
	};
};

class CProcessRect
{
private :
	int						m_lWidth;
	int						m_lHeight;
	int						m_lSize;
	std::vector<CValuedRect>	m_vRects;
	std::vector<bool>			m_vProcessed;
	bool						m_bToProcess;
	CRect						m_rcToProcess;

private :
	bool IsProcessRectOk()
	{
		bool			bResult = false;

		bResult = (m_rcToProcess.left >= 0) && (m_rcToProcess.left < m_lWidth) &&
				  (m_rcToProcess.right >= 0) && (m_rcToProcess.right < m_lWidth) &&
				  (m_rcToProcess.top >= 0) && (m_rcToProcess.top < m_lHeight) &&
				  (m_rcToProcess.bottom >= 0) && (m_rcToProcess.bottom < m_lHeight) &&
				  (m_rcToProcess.left < m_rcToProcess.right) &&
				  (m_rcToProcess.top < m_rcToProcess.bottom);

		return bResult;
	};

public :
	CProcessRect()
	{
		m_bToProcess = false;
		m_rcToProcess.left = 0;
		m_rcToProcess.top = 0;
		m_rcToProcess.right = 0;
		m_rcToProcess.bottom = 0;
		m_rcToProcess.SetRectEmpty();
        m_lWidth = 0;
        m_lHeight = 0;
        m_lSize = 0;
	};
	virtual ~CProcessRect() {};

	void	Init(int lWidth, int lHeight, int lRectSize)
	{
		int			i, j;

		m_lWidth	= lWidth;
		m_lHeight	= lHeight;
		m_lSize		= lRectSize;

		m_vRects.clear();
		m_vProcessed.clear();
		for (i = 0;i<m_lWidth;i+=lRectSize)
		{
			for (j = 0;j<m_lHeight;j+=lRectSize)
			{
				CValuedRect	rcCell;

				rcCell.m_rc.left = i;
				rcCell.m_rc.right = min(i+m_lSize, m_lWidth);
				rcCell.m_rc.top = j;
				rcCell.m_rc.bottom = min(j+m_lSize, m_lHeight);

				rcCell.m_fScore = fabs(((i+m_lSize/2.0) - m_lWidth/2.0)/(double)m_lWidth) + fabs(((j + m_lSize/2) - m_lHeight/2.0)/(double)m_lHeight);

				m_vRects.push_back(rcCell);
				m_vProcessed.push_back(false);
			};
		};
		std::sort(m_vRects.begin(), m_vRects.end());
	};

	void	SetProcessRect(const CRect & rc)
	{
		m_rcToProcess = rc;
	};

	void	Reset()
	{
		for (int i = 0;i<m_vProcessed.size();i++)
			m_vProcessed[i] = false;
		m_bToProcess = true;
	};

	bool	GetNextUnProcessedRect(CRect & rcCell)
	{
		bool		bResult = false;

		if (m_bToProcess)
		{
			if (!m_rcToProcess.IsRectEmpty() && IsProcessRectOk())
			{
				if (!m_vProcessed[0])
				{
					rcCell = m_rcToProcess;
					m_vProcessed[0] = true;
					bResult = true;
				};
			}
			else
			{
				for (int i = 0;i<m_vProcessed.size() && !bResult;i++)
				{
					if (!m_vProcessed[i])
					{
						bResult = true;
						rcCell  = m_vRects[i].m_rc;
						m_vProcessed[i] = true;
					};
				};
			};

			m_bToProcess = bResult;
		};

		return bResult;
	};

	float GetPercentageComplete() const
	{
		if (m_vProcessed.size() == 0)
			return 100.0f;

		float fPercentage = 0.0f;
		const float fDelta = 100.0f / static_cast<float>(m_vProcessed.size());

		// The iteration loop here could be corrupted by a call to Init() on a different thread.
		// To make totally thread safe this should really have a mutex lock associated with it.
		for (bool bState : m_vProcessed)
			fPercentage += bState ? fDelta : 0.0f;

		return fPercentage;
	}
};

/* ------------------------------------------------------------------- */

typedef std::list<CDSSSetting>			PROCESSPARAMLIST;
typedef PROCESSPARAMLIST::iterator		PROCESSPARAMITERATOR;

class CProcessParamsList
{
public :
	PROCESSPARAMLIST		m_lParams;
	int					m_lCurrent;

public :
	CProcessParamsList()
	{
		m_lCurrent = -1;
    };
	virtual ~CProcessParamsList()
	{
	};

	int size()
	{
		return (int)m_lParams.size();
	};

	int current()
	{
		return m_lCurrent;
	};

	void clear()
	{
		m_lParams.clear();
		m_lCurrent = -1;
	};

	bool	MoveForward()
	{
		bool			bResult = false;

		if (m_lCurrent+1<size())
		{
			m_lCurrent++;
			bResult = true;
		};
		return bResult;
	};
	bool	MoveBackward()
	{
		bool			bResult = false;

		if ((m_lCurrent-1 >= 0) && (size() > 0))
		{
			m_lCurrent--;
			bResult = true;
		};
		return bResult;
	};

	bool IsBackwardAvailable()
	{
		return (m_lCurrent-1>=0);
	};

	bool IsForwardAvailable()
	{
		return (m_lCurrent+1<size());
	};
	bool	GetCurrentParams(CDSSSetting & pp)
	{
		return GetParams(m_lCurrent, pp);
	};

	bool	GetParams(int lIndice, CDSSSetting & pp)
	{
		bool					bResult = false;
		PROCESSPARAMITERATOR    it;
		//bool					bFound = false;

		if (!(lIndice >= 0) && (lIndice < size()))
			return false;

		for (it = m_lParams.begin(); it != m_lParams.end() && lIndice>0; it++, lIndice--);
		if (it != m_lParams.end())
		{
			pp = (*it);
			bResult = true;
		};

		return bResult;
	};

	bool	AddParams(const CDSSSetting & pp)
	{
		bool						bResult = false;

		if ((m_lCurrent >=0) && (m_lCurrent < size()-1))
		{
			PROCESSPARAMITERATOR	it;
			int					lIndice = m_lCurrent+1;

			for (it = m_lParams.begin(); it != m_lParams.end() && lIndice>0; it++, lIndice--);
			
			m_lParams.erase(it, m_lParams.end());
		}
		else if (m_lCurrent == -1)
			m_lParams.clear();

		m_lParams.push_back(pp);

		m_lCurrent = size()-1;

		bResult = true;

		return bResult;
	};

};

/////////////////////////////////////////////////////////////////////////////
// CProcessingDlg dialog

class CProcessingDlg : public CDialog
{
private :
	CControlPos				m_ControlPos;
	CDSSSetting				m_ProcessParams;
	CProcessRect			m_ToProcess;

	CString					m_strCurrentFile;

	CProcessSettingsSheet	m_Settings;
	CRGBTab					m_tabRGB;
	CLuminanceTab			m_tabLuminance;
	CSaturationTab			m_tabSaturation;
	CProcessParamsList		m_lProcessParams;
	bool					m_bDirty;

	CSelectRectSink			m_SelectRectSink;
	QWidget* parentWidget;

// Construction
public:
	CProcessingDlg(CWnd* pParent = nullptr);   // standard constructor
	void setParent(QWidget* p) { parentWidget = p; };

	bool	SaveOnClose();

// Dialog Data
	//{{AFX_DATA(CProcessingDlg)
	enum { IDD = IDD_PROCESSING };
	CLabel				m_Info;
	CStatic				m_SettingsRect;
	CStatic				m_OriginalHistogramStatic;
	CStatic				m_PictureStatic;
	//}}AFX_DATA
	CWndImage			m_Picture;
	CWndImage			m_OriginalHistogram;
	CProgressCtrl		m_ProcessingProgress;

	double				m_fGradientOffset;
	double				m_fGradientRange;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcessingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private :
	bool	AskToSave();
	void	ProcessAndShow(bool bSaveUndo = true);

	void	ResetSliders();
	void	DrawHistoBar(Graphics * pGraphics, int lNrReds, int lNrGreens, int lNrBlues, int X, int lHeight);
	void	ShowHistogram(CWndImage & wndImage, CRGBHistogram & Histogram, bool bLog = false);
	void	ShowOriginalHistogram(bool bLog = false);

	void	DrawGaussCurves(Graphics * pGraphics, CRGBHistogram & Histogram, int lWidth, int lHeight);

	void	UpdateHistogramAdjust();
	void	DrawBezierCurve(Graphics * pGraphics, int lWidth, int lHeight);

	void	UpdateControls();
	void	UpdateControlsFromParams();
	void	UpdateMonochromeControls();

	void	UpdateInfos();

// Implementation
public:
	void	UpdateBezierCurve();
	void	OnUndo();
	void	OnRedo();
	void	OnSettings();

	void	CopyPictureToClipboard();
	bool	SavePictureToFile();
	void	CreateStarMask();

	void	LoadFile(LPCTSTR szFileName);
	void	SetCurrentFileName(LPCTSTR szFileName)
	{
		m_strCurrentFile = szFileName;
		UpdateInfos();
	};

	// Generated message map functions
	//{{AFX_MSG(CProcessingDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLoaddsi();
	afx_msg void OnProcess();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnReset();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	afx_msg void OnNotifyRedChangeSelPeg(NMHDR * pNotifyStruct, LRESULT *result);
	afx_msg void OnNotifyRedPegMove(NMHDR * pNotifyStruct, LRESULT *result);
	afx_msg void OnNotifyRedPegMoved(NMHDR * pNotifyStruct, LRESULT *result);
	afx_msg void OnNotifyGreenChangeSelPeg(NMHDR * pNotifyStruct, LRESULT *result);
	afx_msg void OnNotifyGreenPegMove(NMHDR * pNotifyStruct, LRESULT *result);
	afx_msg void OnNotifyGreenPegMoved(NMHDR * pNotifyStruct, LRESULT *result);
	afx_msg void OnNotifyBlueChangeSelPeg(NMHDR * pNotifyStruct, LRESULT *result);
	afx_msg void OnNotifyBluePegMove(NMHDR * pNotifyStruct, LRESULT *result);
	afx_msg void OnNotifyBluePegMoved(NMHDR * pNotifyStruct, LRESULT *result);

	afx_msg LRESULT OnInitNewPicture(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()
};

inline CProcessingDlg * GetParentProcessingDlg(CWnd * pWnd)
{
	pWnd = pWnd->GetParent()->GetParent();
	CProcessingDlg *	pResult = dynamic_cast<CProcessingDlg *>(pWnd);

	return pResult;
};
