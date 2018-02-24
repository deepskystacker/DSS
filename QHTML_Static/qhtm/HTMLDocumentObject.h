/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLDocumentObject.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef HTMLDOCUMENTOBJECT_H
#define HTMLDOCUMENTOBJECT_H

class CHTMLDocumentObject	//	docobj
{
public:
	enum Type { knNone, knParagraph };

	inline Type GetType() const { return m_type; }

	virtual ~CHTMLDocumentObject();
#ifdef _DEBUG
	virtual void Dump() const = 0;
#endif	//	_DEBUG

protected:
	explicit CHTMLDocumentObject( Type type );

private:
	Type m_type;

private:
	CHTMLDocumentObject();
	CHTMLDocumentObject( const CHTMLDocumentObject &);
	CHTMLDocumentObject& operator =( const CHTMLDocumentObject &);
};

#endif //HTMLDOCUMENTOBJECT_H