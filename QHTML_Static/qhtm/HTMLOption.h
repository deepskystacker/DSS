/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLOption.h
Owner:	russf@gipsysoft.com
Purpose:
----------------------------------------------------------------------*/
#ifndef HTMLOPTION_H
#define HTMLOPTION_H

class CHTMLOption
//
//	List box or combo item
{
public:
	CHTMLOption();
	~CHTMLOption();

	StringClass m_strValue;
	StringClass m_strLabel;
	StringClass m_strText;
	StringClass m_strID;

	bool m_bSelected;

#ifdef _DEBUG
	virtual void Dump() const;
#endif	//	_DEBUG

private:
	CHTMLOption( const CHTMLOption &);
	CHTMLOption& operator =( const CHTMLOption &);
};


#endif //HTMLOPTION_H