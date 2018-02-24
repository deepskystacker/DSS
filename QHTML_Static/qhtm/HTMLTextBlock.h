/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLTextBlock.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef HTMLTEXTBLOCK_H
#define HTMLTEXTBLOCK_H

class CHTMLTextBlock : public CHTMLParagraphObject			//txt
//
//	Text block.
//	Has some text.
{
public:
	CHTMLTextBlock( const CTextABC &strText, const HTMLFontDef * pFont, CColor crFore, CColor crBack, bool bPreformatted );

	virtual bool IsEmpty() const;
#ifdef _DEBUG
	virtual void Dump() const;
#endif	//	_DEBUG
	virtual void AddDisplayElements( class CHTMLSectionCreator *psc );

	StringClass m_strText;
	StringClass m_strTip;
	StringClass m_strActivationTarget;

	const HTMLFontDef * m_pFont;
	CColor m_crFore;
	bool m_bPreformatted;
	CColor m_crBack;

private:
	CHTMLTextBlock();
	CHTMLTextBlock( const CHTMLTextBlock &);
	CHTMLTextBlock& operator =( const CHTMLTextBlock &);
};

#endif //HTMLTEXTBLOCK_H