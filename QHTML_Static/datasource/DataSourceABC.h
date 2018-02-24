/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	DataSourceABC.h
Owner:	russf@gipsysoft.com
Purpose:	Data source ABC class.
----------------------------------------------------------------------*/
#ifndef DATASOURCEABC_H
#define DATASOURCEABC_H

#ifndef DEBUGHLP_H
	#include <DebugHlp/DebugHlp.h>
#endif	//	DEBUGHLP_H

class CDataSourceABC
{
public:
	//	Read some data from the stream
	virtual bool ReadBytes( BYTE *pbBuffer, UINT uCount ) = 0;
	inline BYTE ReadByte() { BYTE b; VERIFY( ReadBytes( &b, 1 ) ); return b; }

	//	Set the positionof the stream relative to the current position
	//	n can be positive or negative
	virtual bool SetRelativePos( int n ) = 0;

	// Get the current position in the data stream
	virtual int GetCurrentPos() const = 0;
	
	//	Reset the position to the begining
	virtual bool Reset() = 0;

	//	Get the size, in bytes, of this stream.
	virtual int GetSize() const = 0;

	virtual HINSTANCE GetResourceHandle() const
	{
		return 0;
	}

	//	Read a long in MSB format
	bool MSBReadLong( long &l );
};

#endif //DATASOURCEABC_H