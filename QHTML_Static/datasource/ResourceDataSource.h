/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	ResourceDataSource.h
Owner:	russf@gipsysoft.com
Purpose:	A data source based on a Windows resource.
----------------------------------------------------------------------*/
#ifndef RESOURCEDATASOURCE_H
#define RESOURCEDATASOURCE_H

#ifndef BUFFERDATASOURCE_H
	#include "BufferDataSource.h"
#endif	//	BUFFERDATASOURCE_H

class CResourceDataSource : public CBufferDataSource
{
public:
	CResourceDataSource();
	virtual ~CResourceDataSource();

	bool Open( HINSTANCE hInst, LPCTSTR pcszName, LPCTSTR pcszResourceType );
	
	virtual HINSTANCE GetResourceHandle() const
	{
		return m_hInst;
	}

private:
  HRSRC   m_hRsrc;
  HGLOBAL m_hGlobal;
	HINSTANCE m_hInst;

private:
	CResourceDataSource( const CResourceDataSource & );
	CResourceDataSource& operator = ( const CResourceDataSource & );
};

#endif //RESOURCEDATASOURCE_H