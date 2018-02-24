/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLBreak.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef HTMLBREAK_H
#define HTMLBREAK_H

#ifndef HTMLPARAGRAPHOBJECT_H
	#include "HTMLParagraphObject.h"
#endif	//	HTMLPARAGRAPHOBJECT_H

class CHTMLBreak: public CHTMLParagraphObject
{
public:
	CHTMLBreak()
		: CHTMLParagraphObject( CHTMLParagraphObject::knNone )
		, m_clear( knNone )
	{}

	enum Clear { knNone, knLeft, knRight, knAll, knMax };

	virtual void AddDisplayElements( class CHTMLSectionCreator *psc );

	Clear m_clear;

#ifdef _DEBUG
	virtual void Dump() const;
#endif	//	_DEBUG

private:
	CHTMLBreak( const CHTMLBreak &);
	CHTMLBreak& operator =( const CHTMLBreak &);
};

#endif //HTMLBREAK_H