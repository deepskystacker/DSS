/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	SimpleString.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef QHTM_TYPES_H
#include "QHTM_Types.h"
#endif	//	QHTM_TYPES_H

#ifndef SIMPLESTRING_H
#define SIMPLESTRING_H

#ifndef TEXTABC_H
	#include "TextABC.h"
#endif	//	TEXTABC_H


class CSimpleString : public CTextABC
{
public:
	CSimpleString();
	/*lint -e1931 */
	CSimpleString( LPCTSTR pcszText );
	/*lint +e1931 */
	CSimpleString( LPCTSTR pcszText, size_t uLength );
	CSimpleString( const CSimpleString &rhs );
	CSimpleString &operator = ( const CSimpleString &rhs );

	void Add( LPCTSTR pcszText, size_t uLength );
	void Set( LPCTSTR pcszText, size_t uLength );
	CSimpleString &operator += ( const CSimpleString &rhs );
	CSimpleString &operator += ( LPCTSTR pcszText );

	void TrimBoth();

	virtual ~CSimpleString();

	void Delete( size_t uIndex, size_t uCount );

	void Empty();

	int Compare( const CTextABC & ) const;
	int CompareNoCase( const CTextABC & ) const;

	virtual size_t GetLength() const;
	virtual LPCTSTR GetData() const;

	void Replace( UINT uFind, UINT uReplace );

private:
	ArrayOfChar m_arrText;

private:
};

#endif //SIMPLESTRING_H