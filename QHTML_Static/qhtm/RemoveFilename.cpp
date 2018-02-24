/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	RemoveFilename.cpp
Owner:	russf@gipsysoft.com

Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include <Shlwapi.h>

#ifndef UNDER_CE
#pragma comment( lib, "shlwapi.lib" )
#endif	//	UNDER_CE

void RemoveFilename( LPTSTR pcszFilename )
{
#ifdef UNDER_CE
	wchar_t* pos = ::wcsrchr( pcszFilename, L'\\' );
	if( pos )
	{
		*pos = '\000';
	}
#else		//	UNDER_CE
	PathRemoveFileSpec( pcszFilename );
#endif	//	UNDER_CE
}