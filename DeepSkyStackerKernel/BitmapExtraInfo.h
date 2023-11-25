#pragma once
#include "ExtraInfo.h"

class CBitmapExtraInfo
{
public:
	std::vector<ExtraInfo> m_vExtras;

public:
	CBitmapExtraInfo() = default;
	~CBitmapExtraInfo() = default;

	void AddInfo(const ExtraInfo& ei)
	{
		m_vExtras.push_back(ei);
	}

	void AddInfo(const QString& szName, const QString& szValue, const QString& szComment, bool bPropagate = false)
	{
		m_vExtras.emplace_back(ExtraInfo{ .m_Type = ExtraInfo::EIT_STRING, .m_strName = szName, .m_strValue = szValue, .m_strComment = szComment, .m_bPropagate = bPropagate });
	}
	void AddInfo(const QString& szName, int lValue, const QString& szComment, bool bPropagate = false)
	{
		m_vExtras.emplace_back(ExtraInfo{ .m_Type = ExtraInfo::EIT_LONG, .m_strName = szName, .m_strComment = szComment, .m_lValue = lValue, .m_bPropagate = bPropagate });
	}
	void AddInfo(const QString& szName, double fValue, const QString& szComment = nullptr, bool bPropagate = false)
	{
		m_vExtras.emplace_back(ExtraInfo{ .m_Type = ExtraInfo::EIT_DOUBLE, .m_strName = szName, .m_strComment = szComment, .m_fValue = fValue, .m_bPropagate = bPropagate });
	}

	void Clear()
	{
		m_vExtras.clear();
	}
};