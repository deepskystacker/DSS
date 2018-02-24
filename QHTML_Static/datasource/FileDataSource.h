/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	FileDataSource.h
Owner:	russf@gipsysoft.com
Purpose:	Data source object that is based upon a file.
----------------------------------------------------------------------*/
#ifndef FILEDATASOURCE_H
#define FILEDATASOURCE_H

#ifndef _INC_STDIO
	#include <stdio.h>
#endif	//	_INC_STDIO

#ifndef DATASOURCEABC_H
	#include "DataSourceABC.h"
#endif	//	DATASOURCEABC_H

class CFileDataSource : public CDataSourceABC
{
public:
	CFileDataSource();
	virtual ~CFileDataSource();
	
	bool Open( LPCTSTR pcszFilename );
	virtual bool ReadBytes( BYTE *pbBuffer, UINT uCount );
	virtual bool SetRelativePos( int n);
	virtual bool Reset();
	virtual int GetSize() const;
	virtual int GetCurrentPos() const;

private:
	FILE *m_pFile;
};

#endif //FILEDATASOURCE_H