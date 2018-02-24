/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	BufferDataSource.h
Owner:	russf@gipsysoft.com
Purpose:	A general data source from some arbitrary data pointer.
----------------------------------------------------------------------*/
#ifndef BUFFERDATASOURCE_H
#define BUFFERDATASOURCE_H

#ifndef DATASOURCEABC_H
	#include "DataSourceABC.h"
#endif	//	DATASOURCEABC_H

class CBufferDataSource : public CDataSourceABC  
{
public:
	CBufferDataSource();
	virtual ~CBufferDataSource();

	bool Open( BYTE *pbBuffer, UINT uDataLen );

	virtual bool ReadBytes( BYTE *pbBuffer, UINT uCount );
	virtual bool SetRelativePos( int n);
	virtual bool Reset();
	virtual int GetSize() const;
	virtual int GetCurrentPos() const;

protected:
	BYTE *m_pBuffer, *m_pCurrent;
	UINT m_nDataLen;

private:
	CBufferDataSource( const CBufferDataSource & );
	CBufferDataSource& operator = ( const CBufferDataSource & );
};

#endif //BUFFERDATASOURCE_H