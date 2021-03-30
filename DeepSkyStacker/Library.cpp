#include <stdafx.h>
#include "Library.h"

/* ------------------------------------------------------------------- */

void	CLibrary::LoadLibrary()
{
};

/* ------------------------------------------------------------------- */

void	CLibrary::SaveLibrary()
{
};

/* ------------------------------------------------------------------- */

bool	CLibrary::IsInLibrary(const CLibraryEntry & le)
{
	bool				bResult = false;

	for (int i = 0;i<m_vEntries.size() && !bResult;i++)
	{
		if (le == m_vEntries[i])
			bResult = true;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool	CLibrary::IsInLibrary(const std::vector<CString> & vFiles)
{
	CLibraryEntry		le(PICTURETYPE_UNKNOWN, vFiles);

	return IsInLibrary(le);
};

/* ------------------------------------------------------------------- */

bool	CLibrary::AddToLibrary(const CLibraryEntry & le)
{
	bool				bResult = false;
	CLibraryEntry		le1 = le;

	if (le.m_vFiles.size())
	{
		std::sort(le1.m_vFiles.begin(), le1.m_vFiles.end());
		if (IsInLibrary(le1))
			RemoveFromLibrary(le1);

		m_vEntries.push_back(le1);
		bResult = true;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool	CLibrary::AddToLibrary(PICTURETYPE Type, const std::vector<CString> & vFiles, double fTemperature)
{
	CLibraryEntry		le(Type, vFiles, fTemperature);

	return AddToLibrary(le);
};

/* ------------------------------------------------------------------- */

bool	CLibrary::RemoveFromLibrary(const CLibraryEntry & le)
{
	bool				bResult = false;

	return bResult;
};

/* ------------------------------------------------------------------- */

bool	CLibrary::RemoveFromLibrary(const std::vector<CString> & vFiles)
{
	CLibraryEntry		le(PICTURETYPE_UNKNOWN, vFiles);

	return RemoveFromLibrary(le);
};

/* ------------------------------------------------------------------- */
