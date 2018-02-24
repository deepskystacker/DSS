/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	WindowText.h
Owner:	russf@gipsysoft.com
Purpose:	Simple class to handle getting the window text into a string
----------------------------------------------------------------------*/
#ifndef WINDOWTEXT_H
#define WINDOWTEXT_H

class CWindowText
{
public:
	inline explicit CWindowText( HWND hwnd );
	inline ~CWindowText() { if( m_pszText ) delete[] m_pszText; }
	inline operator LPTSTR() const { return m_pszText; }
	inline int GetLength() const { return m_nLength; }

private:
	CWindowText();					//	Not implemented
	CWindowText( const CWindowText &rhs );
	CWindowText& operator =( const CWindowText &rhs );

	LPTSTR m_pszText;
	int m_nLength;
};


inline CWindowText::CWindowText( HWND hwnd )
	: m_pszText( NULL )
{
	ASSERT_VALID_HWND( hwnd );
	m_nLength = GetWindowTextLength( hwnd );
	if( m_nLength )
	{
		m_pszText = new TCHAR[ m_nLength + 1];
		if( m_pszText )
		{
			VAPI( GetWindowText( hwnd, m_pszText, m_nLength + 1 ) );
		}
	}
}

#endif //WINDOWTEXT_H
