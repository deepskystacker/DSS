#pragma once
#include "ExtraInfo.h"

class CBitmapExtraInfo
{
public:
	EXTRAINFOVECTOR m_vExtras;

public:
	CBitmapExtraInfo() {};
	~CBitmapExtraInfo() {};

	void AddInfo(const CExtraInfo& ei)
	{
		m_vExtras.push_back(ei);
	};

	void AddInfo(const QString& szName, const QString& szValue, const QString& szComment, bool bPropagate = false)
	{
		CExtraInfo		ei;

		ei.m_Type = EIT_STRING;
		ei.m_strName = szName;
		ei.m_strComment = szComment;
		ei.m_strValue = szValue;
		ei.m_bPropagate = bPropagate;
		m_vExtras.push_back(ei);
	};
	void AddInfo(const QString& szName, int lValue, const QString& szComment, bool bPropagate = false)
	{
		CExtraInfo		ei;

		ei.m_Type = EIT_LONG;
		ei.m_strName = szName;
		ei.m_lValue = lValue;
		ei.m_strComment = szComment;
		ei.m_bPropagate = bPropagate;
		m_vExtras.push_back(ei);
	};
	void AddInfo(const QString& szName, double fValue, const QString& szComment = nullptr, bool bPropagate = false)
	{
		CExtraInfo		ei;

		ei.m_Type = EIT_DOUBLE;
		ei.m_strName = szName;
		ei.m_fValue = fValue;
		ei.m_strComment = szComment;
		ei.m_bPropagate = bPropagate;
		m_vExtras.push_back(ei);
	};

	void Clear()
	{
		m_vExtras.clear();
	};
};