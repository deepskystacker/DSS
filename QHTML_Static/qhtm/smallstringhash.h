/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	SmallStringHash.h
Owner:	rich@woodbridgeinternalmed.com
Purpose:	Hash method for CSmallString
----------------------------------------------------------------------*/
#ifndef SMALLSTRINGHASH_H
#define SMALLSTRINGHASH_H

namespace Container
{
	inline UINT HashIt( const StringClass& s)
	{
		UINT uHash = 0;
		LPCTSTR pcszText = s;
		if (!pcszText)
			return 0;
		while( *pcszText )
		{
			uHash = uHash << 1 ^ toupper( *pcszText++ );
		}
		return uHash;
	}

	inline bool ElementsTheSame( const StringClass& lhs, const StringClass& rhs )
	{
		return !lhs.CompareNoCase(rhs);
	}

}

#endif
