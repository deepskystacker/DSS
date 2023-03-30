#pragma once

#include "BitmapExt.h"

/* ------------------------------------------------------------------- */

const	double	LIBTEMP_UNKNOWN		= -300.0;

class CLibraryEntry
{
public :
	PICTURETYPE					m_Type;
	double						m_fTemperature;
	std::vector<CString>		m_vFiles;
	mutable CBitmapInfo			m_BitmapInfo;

private:
	void	CopyFrom(const CLibraryEntry & right)
	{
		m_Type			= right.m_Type;
		m_fTemperature	= right.m_fTemperature;
		m_vFiles		= right.m_vFiles;
		m_BitmapInfo	= right.m_BitmapInfo;
	};

public:
	CLibraryEntry()
	{
        m_fTemperature = 0;
	};

	CLibraryEntry(PICTURETYPE Type, const std::vector<CString> & vFiles, double fTemp=LIBTEMP_UNKNOWN)
		: m_Type(Type), m_vFiles(vFiles), m_fTemperature(fTemp)
	{
	};

	~CLibraryEntry()
	{
	};

	CLibraryEntry(const CLibraryEntry & right)
	{
		CopyFrom(right);
	};

	CLibraryEntry & operator = (const CLibraryEntry & right)
	{
		CopyFrom(right);
		return (*this);
	};

	bool operator == (const CLibraryEntry & right) const
	{
		bool				bResult = m_vFiles.size() == right.m_vFiles.size();

		for (int i = 0;i<m_vFiles.size() && bResult;i++)
		{
			bResult = !m_vFiles[i].CompareNoCase(right.m_vFiles[i]);
		};

		return bResult;
	};

	bool operator < (const CLibraryEntry & right) const
	{
		return true;
	};
};

typedef std::vector<CLibraryEntry>		LIBRARYENTRYVECTOR;
typedef LIBRARYENTRYVECTOR::iterator	LIBRARYENTRYITERATOR;

/* ------------------------------------------------------------------- */

class CLibrary
{
public :
	LIBRARYENTRYVECTOR					m_vEntries;

public :
	CLibrary()
	{
	};

	virtual ~CLibrary()
	{
	};

	void	LoadLibrary();
	void	SaveLibrary();

	bool	IsInLibrary(const CLibraryEntry & le);
	bool	IsInLibrary(const std::vector<CString> & vFiles);

	bool	AddToLibrary(const CLibraryEntry & le);
	bool	AddToLibrary(PICTURETYPE Type, const std::vector<CString> & vFiles, double fTemperature = LIBTEMP_UNKNOWN);

	bool	RemoveFromLibrary(const CLibraryEntry & le);
	bool	RemoveFromLibrary(const std::vector<CString> & vFiles);
};

