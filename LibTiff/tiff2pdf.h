// tiff2pdf.h : main header file for the TIFF2PDF application
//

#if !defined(AFX_TIFF2PDF_H__4CEFD1A1_E7E4_4E96_94CB_E56C661F6BBD__INCLUDED_)
#define AFX_TIFF2PDF_H__4CEFD1A1_E7E4_4E96_94CB_E56C661F6BBD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// Tiff2PDF:
// See tiff2pdf.cpp for the implementation of this class
//

class Tiff2PDF : public CWinApp
{
public:
	Tiff2PDF();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Tiff2PDF)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(Tiff2PDF)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIFF2PDF_H__4CEFD1A1_E7E4_4E96_94CB_E56C661F6BBD__INCLUDED_)
