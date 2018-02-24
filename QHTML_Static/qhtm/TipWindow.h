/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	TipWindow.h
Owner:	russf@gipsysoft.com
Purpose:	Tool tips window
----------------------------------------------------------------------*/
#ifndef TIPWINDOW_H
#define TIPWINDOW_H

#ifndef HTMLSECTION_H
	#include "HTMLSection.h"
#endif //#ifndef HTMLSECTION_H

#ifndef DEFAULTS_H
	#include "defaults.h"
#endif //#ifndef DEFAULTS_H

#ifdef QHTM_ALLOW_TOOLTIPS
class CTipWindow : public CParentSection
{
public:
	CTipWindow( CSectionABC *pParent, LPCTSTR pcszTip, WinHelper::CPoint &pt, UINT uCharacterSet );

	void OnWindowDestroyed();
	void OnLayout( const WinHelper::CRect &rc );
	virtual void OnMouseMove( const WinHelper::CPoint &pt );

	static DWORD	LastTipCreated() { return m_nLastTipCreated; }

	//
	//	Register the tip window class
	static BOOL Register( HINSTANCE hInst );

	void DestroyWindow();

private:
	static LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

	CTipWindow( const CTipWindow &);
	CTipWindow& operator =( const CTipWindow &);
	virtual ~CTipWindow();
	bool UpdateCursor();

	//	MUST BE FIRST
	CDefaults m_defaults;

	CHTMLSection	m_htmlSection;

	static DWORD	m_nLastTipCreated;
	StringClass	m_strTip;

	HWND m_hwnd;
};

#endif	//	QHTM_ALLOW_TOOLTIPS

#endif //TIPWINDOW_H