/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	ParseStyles.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef PARSESTYLES_H
#define PARSESTYLES_H

#ifndef STATICSTRING_H
	#include "StaticString.h"
#endif	//	STATICSTRING_H

#ifndef QHTM_TYPES_H
	#include "QHTM_Types.h"
#endif	//	QHTM_TYPES_H
	

	enum Style
	{
			sFontWeight
		, sFontsize
		, sFontFamily

		, sColor
		, sBackgroundColor
		, sBackgroundImage
	};

	class CStylePair
	{
	public:
		Style m_style;
		CStaticString m_strValue;
	};
	typedef ArrayClass< CStylePair > CStyles;

	void ParseStyles( const CStaticString &strStyle, CStyles &styles );


#endif //PARSESTYLES_H