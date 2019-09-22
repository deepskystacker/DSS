//------------------------------------------------------------------------------
// ControlPos.cpp
//
//	CControlPos
//	Position controls on a form's resize
//
//		Copyright (c) 2000 Paul Wendt
//
//		VERSION#	DATE			NAME	DESCRIPTION OF CHANGE
//		--------	----------	----	---------------------
//		1.01  	07/11/2000	PRW	Original creation.
//
#include "StdAfx.h"
#include "ControlPos.h"
#include <vector>

//------------------------------------------------------------------------------
// CControlPos::CControlPos
//
//	default constructor
//
//	Access: public
//
//	Args:
//		CWnd* pParent				=	pointer to parent window
//
//	Return:
//		none
//
CControlPos::CControlPos(CWnd* pParent /* = NULL */)
{
	m_pParent = pParent;
	UpdateParentSize();

	m_nOldParentHeight = 0;
	m_nOldParentWidth = 0;

	SetNegativeMoves(FALSE);

	ResetControls();
}

//------------------------------------------------------------------------------
// CControlPos::~CControlPos
//
//	default destructor -- It deletes all controls.
//
//	Access: public
//
//	Args:
//		none
//
//	Return:
//		none
//
CControlPos::~CControlPos()
{
	ResetControls();
}

//------------------------------------------------------------------------------
// CControlPos::SetParent
//
//	This sets the parent window. It should be called from a CWnd's
//    post-constructor function, like OnInitdialog or InitialUpdate.
//
//	Access: public
//
//	Args:
//		CWnd* pParent	=	parent window
//
//	Return:
//		none
//
void CControlPos::SetParent(CWnd* pParent)
{
	CRect rcParentOriginalSize;

	m_pParent = pParent;

	m_pParent->GetClientRect(rcParentOriginalSize);
	m_nOriginalParentWidth = rcParentOriginalSize.right;
	m_nOriginalParentHeight = rcParentOriginalSize.bottom;

	UpdateParentSize();
}

//------------------------------------------------------------------------------
// CControlPos::AddControl
//
//	This adds a control to the internal list of controls in CControlPos.
//
//	Access: public
//
//	Args:
//		CWnd* pControl				=	pointer to the control to be added
//		const DWORD& dwStyle		=  how the window should be moved -- see #define's
//                               in the header file
//
//	Return:
//		BOOL 	=	TRUE if the control was added successfully, FALSE otherwise
//
BOOL CControlPos::AddControl(CWnd* pControl, const DWORD& dwStyle /* = CP_MOVE_HORIZONTAL */)
{
	BOOL fReturnValue = TRUE;

	if (pControl && m_pParent)
	{
		LPCONTROLDATA pstControl = new CONTROLDATA;
		pstControl->hControl = pControl->GetSafeHwnd();
		pstControl->dwStyle = dwStyle;
		m_awndControls.Add(((CObject*)pstControl));
	}
	else
	{
		fReturnValue = FALSE;
	}

	return (fReturnValue);
}

//------------------------------------------------------------------------------
// CControlPos::AddControl
//
//	This adds a control the internal list of controls in CControlPos.
//
//	Access: public
//
//	Args:
//		const UINT& unId			=	ID of the control to add
//		const DWORD& dwStyle		=	how the window should be moved -- see #define's
//                               in the header file
//
//	Return:
//		BOOL 	=	TRUE if the control was added successfully, FALSE otherwise
//
BOOL CControlPos::AddControl(const UINT& unId, const DWORD& dwStyle /* = CP_MOVE_HORIZONTAL */)
{
	CWnd* pControl;

	if (m_pParent)
	{
		pControl = m_pParent->GetDlgItem(unId);
		return (AddControl(pControl, dwStyle));
	}
	else
	{
		return (FALSE);
	}
}

//------------------------------------------------------------------------------
// CControlPos::RemoveControl
//
//	If a client ever wants to remove a control programmatically, this
//    function will do it.
//
//	Access: public
//
//	Args:
//		CWnd* pControl	=	pointer of the window who should be removed from
//								the internal control list [ie: will not be repositioned]
//
//	Return:
//		BOOL 	=	TRUE if the control was found [and deleted], FALSE otherwise
//
BOOL CControlPos::RemoveControl(CWnd* pControl)
{
	BOOL fReturnValue = FALSE;

	for (int i = 0; i < m_awndControls.GetSize(); i++)
	{
		LPCONTROLDATA pstControl = ((LPCONTROLDATA)m_awndControls.GetAt(i));

		if (pstControl->hControl == pControl->GetSafeHwnd())
		{
			m_awndControls.RemoveAt(i);
			delete pstControl;
			fReturnValue = TRUE;
			break;
		}
	}

	return (fReturnValue);
}

//------------------------------------------------------------------------------
// CControlPos::RemoveControl
//
//	If a client ever wants to remove a control programmatically, this
//    function will do it.
//
//	Access: public
//
//	Args:
//		const UINT& unId  =  ID of the control that should be removed from the
//                         internal control list [ie: will not be repositioned]
//
//	Return:
//		BOOL 	=	TRUE if the control was found [and deleted], FALSE otherwise
//
BOOL CControlPos::RemoveControl(const UINT& unId)
{
	CWnd* pControl;

	if (m_pParent)
	{
		pControl = m_pParent->GetDlgItem(unId);
		return (RemoveControl(pControl));
	}
	else
	{
		return (FALSE);
	}
}

//------------------------------------------------------------------------------
// CControlPos::ResetControls
//
//	This function removes all controls from the CControlPos object
//
//	Access: public
//
//	Args:
//		none
//
//	Return:
//		none
//
void CControlPos::ResetControls(void)
{
	while (m_awndControls.GetSize() > 0)
	{
		int   nHighIdx = m_awndControls.GetUpperBound();
		LPCONTROLDATA pstControl = ((LPCONTROLDATA)m_awndControls.GetAt(nHighIdx));
		if (pstControl)
		{
			m_awndControls.RemoveAt(nHighIdx);
			delete pstControl;
		}
	}
}

//------------------------------------------------------------------------------
// CControlPos::MoveControls
//
//	This function takes care of moving all controls that have been added to
//    the object [see AddControl].  This function should be called from the
//    WM_SIZE handler-function [typically OnSize].
//
//	Access: public
//
//	Args:
//		none
//
//	Return:
//		none
//
void CControlPos::MoveControls(void)
{
	if (m_pParent)
	{
		//--------------------------------------------------------------------
		// for each control that has been added to our object, we want to
		// check its style and move it based off of the parent control's
		// movements.
		// the thing to keep in mind is that when you resize a window, you
		// can resize by more than one pixel at a time. this is important
		// when, for example, you start with a width smaller than the
		// original width and you finish with a width larger than the
		// original width. you know that you want to move the control, but
		// by how much? that is why so many if's and calculations are made
		//
		std::vector<CRect>			vRects;

		for (int i = 0; i < m_awndControls.GetSize(); i++)
		{
			LPCONTROLDATA pstControl = ((LPCONTROLDATA)m_awndControls.GetAt(i));
			CRect rcParentBounds;
			CRect rcBounds;
			CWnd* pControl = m_pParent->FromHandle(pstControl->hControl);

			pControl->GetWindowRect(rcBounds);
			m_pParent->GetClientRect(rcParentBounds);

			if ((pstControl->dwStyle & (CP_RESIZE_VERTICAL)) == (CP_RESIZE_VERTICAL))
			{
				if (!m_fNegativeMoves)
				{
					if (rcParentBounds.bottom > m_nOriginalParentHeight)
					{
						if (m_nOriginalParentHeight <= m_nOldParentHeight)
						{
							rcBounds.bottom += rcParentBounds.bottom - m_nOldParentHeight;
						}
						else
						{
							rcBounds.bottom += rcParentBounds.bottom - m_nOriginalParentHeight;
						}
					}
					else
					{
						if (m_nOldParentHeight > m_nOriginalParentHeight)
						{
							rcBounds.bottom += m_nOriginalParentHeight - m_nOldParentHeight;
						}
					}
				}
				else
				{
					rcBounds.bottom += rcParentBounds.bottom - m_nOldParentHeight;
				}
			}

			if ((pstControl->dwStyle & (CP_RESIZE_HORIZONTAL)) == (CP_RESIZE_HORIZONTAL))
			{
				if (!m_fNegativeMoves)
				{
					if (rcParentBounds.right > m_nOriginalParentWidth)
					{
						if (m_nOriginalParentWidth <= m_nOldParentWidth)
						{
							rcBounds.right += rcParentBounds.right - m_nOldParentWidth;
						}
						else
						{
							rcBounds.right += rcParentBounds.right - m_nOriginalParentWidth;
						}
					}
					else
					{
						if (m_nOldParentWidth > m_nOriginalParentWidth)
						{
							rcBounds.right += m_nOriginalParentWidth - m_nOldParentWidth;
						}
					}
				}
				else
				{
					rcBounds.right += rcParentBounds.right - m_nOldParentWidth;
				}
			}

			if ((pstControl->dwStyle & (CP_MOVE_VERTICAL)) == (CP_MOVE_VERTICAL))
			{
				if (!m_fNegativeMoves)
				{
					if (rcParentBounds.bottom > m_nOriginalParentHeight)
					{
						if (m_nOriginalParentHeight <= m_nOldParentHeight)
						{
							rcBounds.bottom += rcParentBounds.bottom - m_nOldParentHeight;
							rcBounds.top += rcParentBounds.bottom - m_nOldParentHeight;
						}
						else
						{
							rcBounds.bottom += rcParentBounds.bottom - m_nOriginalParentHeight;
							rcBounds.top += rcParentBounds.bottom - m_nOriginalParentHeight;
						}
					}
					else
					{
						if (m_nOldParentHeight > m_nOriginalParentHeight)
						{
							rcBounds.bottom += m_nOriginalParentHeight - m_nOldParentHeight;
							rcBounds.top += m_nOriginalParentHeight - m_nOldParentHeight;
						}
					}
				}
				else
				{
					rcBounds.bottom += rcParentBounds.bottom - m_nOldParentHeight;
					rcBounds.top += rcParentBounds.bottom - m_nOldParentHeight;
				}
			}

			if ((pstControl->dwStyle & (CP_MOVE_HORIZONTAL)) == (CP_MOVE_HORIZONTAL))
			{
				if (!m_fNegativeMoves)
				{
					if (rcParentBounds.right > m_nOriginalParentWidth)
					{
						if (m_nOriginalParentWidth <= m_nOldParentWidth)
						{
							rcBounds.right += rcParentBounds.right - m_nOldParentWidth;
							rcBounds.left += rcParentBounds.right - m_nOldParentWidth;
						}
						else
						{
							rcBounds.right += rcParentBounds.right - m_nOriginalParentWidth;
							rcBounds.left += rcParentBounds.right - m_nOriginalParentWidth;
						}
					}
					else
					{
						if (m_nOldParentWidth > m_nOriginalParentWidth)
						{
							rcBounds.right += m_nOriginalParentWidth - m_nOldParentWidth;
							rcBounds.left += m_nOriginalParentWidth - m_nOldParentWidth;
						}
					}
				}
				else
				{
					rcBounds.right += rcParentBounds.right - m_nOldParentWidth;
					rcBounds.left += rcParentBounds.right - m_nOldParentWidth;
				}
			}

			m_pParent->ScreenToClient(rcBounds);
			vRects.push_back(rcBounds);
			//pControl->MoveWindow(rcBounds);
		}

		BOOL				bEnableVertical   = TRUE,
							bEnableHorizontal = TRUE;

		for (int i = 0;i<vRects.size();i++)
		{
			if (vRects[i].Width() < 1)
				bEnableHorizontal = FALSE;
			if (vRects[i].Height() < 1)
				bEnableVertical = FALSE;
		};

		for (int i = 0; i < m_awndControls.GetSize(); i++)
		{
			LPCONTROLDATA pstControl = ((LPCONTROLDATA)m_awndControls.GetAt(i));
			CWnd* pControl = m_pParent->FromHandle(pstControl->hControl);
			CRect			rcBounds = vRects[i];
			CRect			rcOrgBounds;

			pControl->GetWindowRect(&rcOrgBounds);
			m_pParent->ScreenToClient(&rcOrgBounds);
			if (!bEnableVertical)
			{
				rcBounds.top = rcOrgBounds.top;
				rcBounds.bottom = rcOrgBounds.bottom;
			};
			if (!bEnableHorizontal)
			{
				rcBounds.left = rcOrgBounds.left;
				rcBounds.right = rcOrgBounds.right;
			};

			pControl->MoveWindow(rcBounds);
		};

		UpdateParentSize();
	}
}

//------------------------------------------------------------------------------
// CControlPos::SetNegativeMoves
//
//	This sets the NegativeMoves boolean parameter of the object. When the
//    parent window becomes smaller than it started, setting this to FALSE
//    will not allow controls to be moved; the parent size may change, but
//    it'll just force the controls to go off of the
//    This parameter defaults to FALSE on object creation.
//
//	Access: public
//
//	Args:
//		const BOOL& fNegativeMoves /* = TRUE */	=	value to set
//
//	Return:
//		none
//
void CControlPos::SetNegativeMoves(const BOOL& fNegativeMoves /* = TRUE */)
{
	m_fNegativeMoves = fNegativeMoves;
}

//------------------------------------------------------------------------------
// CControlPos::GetNegativeMoves
//
//	This function returns whether or not negative moves are enabled.
//
//	Access: public
//
//	Args:
//		none
//
//	Return:
//		BOOL 	=	TRUE if negative moves are enabled, FALSE otherwise
//
BOOL CControlPos::GetNegativeMoves(void) const
{
	return (m_fNegativeMoves);
}

//------------------------------------------------------------------------------
// CControlPos::UpdateParentSize
//
//	Since CControlPos keeps track of the parent's size, it gets updated
//    every time it tells us to size the controls. We keep track so we know
//    how much it changed from the last WM_SIZE message.
//
//	Access: protected
//
//	Args:
//		none
//
//	Return:
//		none
//
void CControlPos::UpdateParentSize(void)
{
	if (m_pParent)
	{
		CRect rcBounds;
		m_pParent->GetClientRect(rcBounds);

		m_nOldParentWidth = rcBounds.Width();
		m_nOldParentHeight = rcBounds.Height();
	}
}