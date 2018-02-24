/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLTextArea.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef HTMLTEXTAREA_H
#define HTMLTEXTAREA_H

#ifndef HTMLFORMINPUT_H
	#include "HTMLFormInput.h"
#endif	//	HTMLFORMINPUT_H

#ifndef HTMLFONTDEF_H
	#include "HTMLFontDef.h"
#endif	//	HTMLFONTDEF_H


class CHTMLTextArea: public CHTMLFormInput
{
public:
	explicit CHTMLTextArea( const HTMLFontDef * pFont );

#ifdef _DEBUG
	virtual void Dump() const;
#endif	//	_DEBUG

private:
	CHTMLTextArea();
	CHTMLTextArea( const CHTMLTextArea &);
	CHTMLTextArea& operator =( const CHTMLTextArea &);
};

#endif //HTMLTEXTAREA_H