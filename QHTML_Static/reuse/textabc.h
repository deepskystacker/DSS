/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	TextABC.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef TEXTABC_H
#define TEXTABC_H

class CTextABC
{
public:
	operator LPCTSTR () const;

	virtual size_t GetLength() const = 0;

	virtual LPCTSTR GetData() const = 0;

	virtual ~CTextABC() {}
};


inline CTextABC::operator LPCTSTR () const
{
	return GetData();
}

#endif //TEXTABC_H