#pragma once
#include "cfa.h"

class CDSLR
{
public:
	QString m_strName;
	CFATYPE m_CFAType;

private:
	void CopyFrom(const CDSLR& cd)
	{
		m_strName = cd.m_strName;
		m_CFAType = cd.m_CFAType;
	};

public:
	CDSLR(QString name, CFATYPE CFAType) :
		m_strName(name),
		m_CFAType(CFAType)
	{
	};

	CDSLR(const CDSLR& cd)
	{
		CopyFrom(cd);
	};

	virtual ~CDSLR()
	{
	};

	const CDSLR& operator = (const CDSLR& cd)
	{
		CopyFrom(cd);
		return *this;
	};
};
