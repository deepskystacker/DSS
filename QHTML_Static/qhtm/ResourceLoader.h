/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	ResourceLoader.h
Owner:	russf@gipsysoft.com
Purpose:	class to load resources of any type.
----------------------------------------------------------------------*/
#ifndef RESOURCELOADER_H
#define RESOURCELOADER_H

class CResourceLoader  
{
public:
	explicit CResourceLoader( HINSTANCE hInst = GetModuleHandle( NULL ) );
	virtual ~CResourceLoader();
	inline bool Load( UINT uID, LPCTSTR pcszType ) { return Load( MAKEINTRESOURCE( uID ), pcszType ); }
	
	bool Load( LPCTSTR pcszName, LPCTSTR pcszType );

	LPCVOID GetData() const { return m_pData; }
	int GetSize() const { return m_nSize; }

private:
	LPVOID m_pData;
	int m_nSize;
	HINSTANCE m_hInst;
};

#endif //RESOURCELOADER_H