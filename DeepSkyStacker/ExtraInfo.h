#pragma once

typedef enum tagEXTRAINFOTYPE
{
	EIT_UNKNOWN = 0,
	EIT_STRING = 1,
	EIT_LONG = 2,
	EIT_DOUBLE = 3
} EXTRAINFOTYPE;

class CExtraInfo
{
public:
	EXTRAINFOTYPE m_Type;
	QString m_strName;
	QString m_strValue;
	QString m_strComment;
	int m_lValue;
	double m_fValue;
	bool m_bPropagate;

private:
	void	CopyFrom(const CExtraInfo& ei)
	{
		m_Type = ei.m_Type;
		m_strName = ei.m_strName;
		m_strValue = ei.m_strValue;
		m_lValue = ei.m_lValue;
		m_fValue = ei.m_fValue;
		m_strComment = ei.m_strComment;
		m_bPropagate = ei.m_bPropagate;
	};

public:
	CExtraInfo()
	{
		m_bPropagate = false;
		m_Type = EIT_UNKNOWN;
		m_lValue = 0;
		m_fValue = 0;
	};
	CExtraInfo(const CExtraInfo& ei)
	{
		CopyFrom(ei);
	};
	~CExtraInfo() {};

	CExtraInfo& operator = (const CExtraInfo& ei)
	{
		CopyFrom(ei);
		return (*this);
	};
};

typedef std::vector<CExtraInfo> EXTRAINFOVECTOR;