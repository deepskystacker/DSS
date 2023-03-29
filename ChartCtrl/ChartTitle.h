/*
 *
 *	ChartTitle.h
 *
 *	Written by Cédric Moonen (cedric_moonen@hotmail.com)
 *
 *
 *
 *	This code may be used for any non-commercial and commercial purposes in a compiled form.
 *	The code may be redistributed as long as it remains unmodified and providing that the 
 *	author name and this disclaimer remain intact. The sources can be modified WITH the author 
 *	consent only.
 *	
 *	This code is provided without any garanties. I cannot be held responsible for the damage or
 *	the loss of time it causes. Use it at your own risks
 *
 *	An e-mail to notify me that you are using this code is appreciated also.
 *
 *
 */

#if !defined(AFX_CHARTTITLE_H__49972787_6D28_4F81_A12F_420947456913__INCLUDED_)
#define AFX_CHARTTITLE_H__49972787_6D28_4F81_A12F_420947456913__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ChartObject.h"

class CChartTitle : public CChartObject  
{
	friend CChartCtrl;

public:
	size_t GetStringCount() const;
	std::string GetString(size_t Index) const;
	void AddString(std::string const& NewString);
	void RemoveAll();

	void SetFont(int iPointSize, std::string const& strFaceName)
	{
		m_iFontSize = iPointSize;
		m_strFontName = strFaceName;
	}

	CChartTitle(CChartCtrl* pParent);
	virtual ~CChartTitle();

private:
	CSize GetSize(CDC* pDC);
	void Draw(CDC *pDC);

	std::vector<std::string> m_StringArray;

	std::string m_strFontName;
	int         m_iFontSize;
};

#endif // !defined(AFX_CHARTTITLE_H__49972787_6D28_4F81_A12F_420947456913__INCLUDED_)
