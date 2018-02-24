/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLParagraphObject.h
Owner:	russf@gipsysoft.com
Purpose:	Paragraph object!
----------------------------------------------------------------------*/
#ifndef HTMLPARAGRAPHOBJECT_H
#define HTMLPARAGRAPHOBJECT_H

class CHTMLAnchor;
class CHTMLParagraph;

class CHTMLParagraphObject				//	obj
{
public:
	virtual ~CHTMLParagraphObject();
	enum Type { knNone, knAnchor };

	inline Type GetType() const { return m_type; }


	virtual bool IsEmpty() const { return false; }
	virtual void ResetMeasuringKludge() { }
	CHTMLAnchor*	m_pAnchor;

	virtual void AddDisplayElements( class CHTMLSectionCreator *psc );

	inline void SetElementID( LPCTSTR pcszElementID ) {  m_strElementID = pcszElementID; }

#ifdef _DEBUG
	virtual void Dump() const = 0;
#endif	//	_DEBUG
protected:

	explicit CHTMLParagraphObject( Type type );

	inline void SetParagraph( CHTMLParagraph *pPara ) { m_pPara = pPara; }

	StringClass m_strElementID;

private:
	CHTMLParagraph *m_pPara;
	Type m_type;

private:
	CHTMLParagraphObject();
	CHTMLParagraphObject( const CHTMLParagraphObject &);
	CHTMLParagraphObject& operator =( const CHTMLParagraphObject &);
	friend class CHTMLParagraph;
};


#endif //HTMLPARAGRAPHOBJECT_H