/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	LoadTextFile.cpp
Owner:	russf@gipsysoft.com
Purpose:	Load a text file.
----------------------------------------------------------------------*/
#include "stdafx.h"

extern bool LoadTextFile( LPCTSTR pcszFilename, LPTSTR &pszBuffer );

#ifdef UNDER_CE
static bool IsTextUnicode( CONST VOID* pBuffer, int cb, LPINT lpi )
{
	const char *pBuf = (const char *)pBuffer;
	for( int u = 0; u < cb; u++ )
	{
		if( pBuf[ u ] == '\000' )
		{
			return true;
		}
	}
	return false;
}
#endif	//	UNDER_CE

bool LoadTextFile( LPCTSTR pcszFilename, LPTSTR &pszBuffer, UINT &uLength )
{
	bool bRetVal = false;
	HANDLE hFile = ::CreateFile( pcszFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( hFile != INVALID_HANDLE_VALUE )
	{
		uLength = ::GetFileSize( hFile, NULL );
		if( uLength )
		{
			LPSTR pszFile = new char [ uLength + 1 ];
			DWORD dwRead;
			if( ::ReadFile( hFile, pszFile, uLength, &dwRead, NULL ) )
			{
				pszFile[ uLength ] = _T('\000');

				pszBuffer = (LPTSTR)pszFile;

#ifdef _UNICODE
				if( !IsTextUnicode( pszFile, uLength, NULL ) )
				{
					LPWSTR pszFile2 = new WCHAR [ uLength * sizeof( TCHAR ) + 1 ];
					MultiByteToWideChar(CP_ACP, 0, pszFile, uLength, pszFile2, uLength * sizeof( TCHAR ) );
					pszBuffer = pszFile2;
					delete pszFile;
				}
#else	//	UNICODE

				if( IsTextUnicode( pszFile, uLength, NULL ) )
				{
					LPSTR pszFile2 = new char [ uLength * sizeof( TCHAR ) + 1 ];
					WideCharToMultiByte( CP_UTF8, 0, (WCHAR*)pszFile, uLength, pszFile2, uLength * sizeof( TCHAR ), NULL, NULL );

					pszBuffer = pszFile2;
					delete pszFile;
				}

#endif	//	_UNICODE
				bRetVal = true;
			}
			else
			{
				delete[] pszFile;
			}
		}

		::CloseHandle( hFile );
	}
	return bRetVal;
}


