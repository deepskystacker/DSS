/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	BufferDataSource.cpp
Owner:	russf@gipsysoft.com
Purpose:	A general data source from some arbitrary data pointer.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "BufferDataSource.h"


CBufferDataSource::CBufferDataSource()
	: m_pBuffer( NULL )
	, m_pCurrent( NULL )
	, m_nDataLen( 0 )
{

}


CBufferDataSource::~CBufferDataSource()
{

}


bool CBufferDataSource::Open( BYTE *pBuffer, UINT nDataLen )
{
	m_pCurrent = m_pBuffer = pBuffer;
	m_nDataLen = nDataLen;
	return true;
}


bool CBufferDataSource::ReadBytes( BYTE *pbBuffer, UINT uCount )
{
	if( m_pCurrent + uCount <= m_pBuffer + m_nDataLen )
	{
		memcpy( pbBuffer, m_pCurrent, uCount );
		m_pCurrent += uCount;
		return true;
	}
	return false;
}


bool CBufferDataSource::SetRelativePos( int n )
{
	if( m_pCurrent + n <= m_pBuffer + m_nDataLen && m_pCurrent + n > m_pBuffer )
	{
		m_pCurrent += n;
		return true;
	}
	return false;
}


bool CBufferDataSource::Reset()
{
	if( m_pBuffer )
	{
		m_pCurrent = m_pBuffer;
		return true;
	}
	return false;
}


int CBufferDataSource::GetSize() const
{
	return m_nDataLen;
}

int CBufferDataSource::GetCurrentPos() const
{
	return (int)(m_pCurrent - m_pBuffer);
}
