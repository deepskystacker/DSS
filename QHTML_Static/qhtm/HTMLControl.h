/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLControl.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef HTMLCONTROL_H
#define HTMLCONTROL_H

#ifdef QHTM_WINDOWS_CONTROL

class CHTMLControl : public CHTMLParagraphObject			//txt
//
//	Text block.
//	Has some text.
{
public:
	explicit CHTMLControl( const HTMLFontDef * pFont );

#ifdef _DEBUG
	virtual void Dump() const;
#endif	//	_DEBUG

	virtual void AddDisplayElements( class CHTMLSectionCreator *psc );

	void SetControlClass( const CTextABC &str );
	void SetStyle( const CTextABC &str );
	void SetStyleEx( const CTextABC &str );
	void SetID( const CTextABC &str );

	void SetWidth( const CTextABC &str );
	void SetHeight( const CTextABC &str );

	enum SizeType { knPixels, knPoints, knDLUs, knPercent };

	SizeType m_WidthType, m_HeightType;
	UINT m_uWidth, m_uHeight;

	UINT m_uStyle;
	UINT m_uStyleEx;
	UINT m_uID;
	StringClass m_strControlClass;
	const HTMLFontDef * m_pFont;

private:
	CHTMLControl();
	CHTMLControl( const CHTMLControl &);
	CHTMLControl& operator =( const CHTMLControl &);
};

#endif	//	QHTM_WINDOWS_CONTROL

#endif //HTMLCONTROL_H