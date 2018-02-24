/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	resstring.h
Owner:	russf@gipsysoft.com
Purpose:	Simple class to load a string from a resource table
----------------------------------------------------------------------*/
#ifndef RESSTRING_H
#define RESSTRING_H

class CResString
//
//	Simple class that loads a string from the resources. It deferrs loading of the
//	string until the string is actually asked for via the conversion operator.
{
public:
	explicit CResString( UINT uStringID );
	virtual ~CResString();

	int GetLength() const;
	//	Give you access to the string
	operator LPCTSTR () const;
	operator LPTSTR ();

private:
	CResString();
	UINT m_uStringID;
	mutable LPTSTR m_pcszString;
	mutable int m_nLength;

	static HINSTANCE g_instStrings;
	static HANDLE g_hHeap;
};


#endif //RESSTRING_H