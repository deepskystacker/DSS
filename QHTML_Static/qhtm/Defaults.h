/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	Defaults.h
Owner:	russf@gipsysoft.com
Purpose:	Defaults for the control.
----------------------------------------------------------------------*/
#ifndef DEFAULTS_H
#define DEFAULTS_H

#ifndef QHTM_H
	#include "qhtm.h"
#endif	//QHTM_H

#ifndef WINHELPER_H
	#include <WinHelper.h>
#endif	//WINHELPER_H

#ifndef SIMPLESTRING_H
	#include "SimpleString.h"
#endif	//	SIMPLESTRING_H

class CDefaults
{
public:
	CDefaults();
	virtual ~CDefaults();

	bool Set( LPQHTM_DEFAULTS lpDefaults );

	//
	//	Set the font used from a HFONT or a LOGFONT
	void SetFont( HFONT hFont );
	void SetFont( LOGFONT &lf );

	StringClass m_strFontName;
	int m_nFontSize;

	//
	//	For lists and quotes
	int m_nIndentSize;
	int m_nIndentSpaceSize;
	int m_nDefinitionListDescriptionIndentSize;

	//
	//	Space after a control in a form
	int m_nHorizontalFormControlGap;

	BYTE m_cCharSet;

	//
	//	Main document colours
	COLORREF m_crBackground;
	COLORREF m_crDefaultForeColour;
	COLORREF m_crLinkColour;
	COLORREF m_crLinkHoverColour;

	//
	//	Tables
	int m_nCellPadding;
	int m_nCellSpacing;
	COLORREF m_crBorderLight;
	COLORREF m_crBorderDark;
	int m_nAlignedTableMargin;	//	When a table is left or right aligned this is the gap that surrounds it

	//
	//	Font name used in preformatted text
	StringClass m_strDefaultPreFontName;

	//	The number of lines before and after a default paragraph
	int m_nParagraphLinesAbove;
	int m_nParagraphLinesBelow;

	//
	//	The margin around images that are aligned
	int m_nImageMargin;

	WinHelper::CRect m_rcMargins;

	int m_nZoomLevel;

	//
	//	User data sent to callback...
	LPARAM m_lParam;

	funcQHTMImageCallback m_funcQHTMImageCallback;
	funcQHTMBitmapCallback m_funcBitmapCallback;
	funcQHTMResourceCallback m_funcQHTMResourceCallback;
	funcQHTMFORMCallback m_funcQHTMFORMCallback;
	funcQHTMEventCallback m_funcQHTMEventCallback;
};

extern CDefaults g_defaults;

#endif //DEFAULTS_H