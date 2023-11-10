#include "stdafx.h"
#include "file.h"

bool GetFileCreationDateTime(LPCTSTR szFileName, FILETIME& FileTime)
{
	bool			bResult = false;
	HANDLE			hFind;
	WIN32_FIND_DATA	FindData;

	hFind = FindFirstFile(szFileName, &FindData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		FileTime = FindData.ftLastWriteTime;
		FindClose(hFind);
		bResult = true;
	};

	return bResult;
}