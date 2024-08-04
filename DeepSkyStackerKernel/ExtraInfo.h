#pragma once

struct ExtraInfo
{
	enum ExtraInfoType {
		EIT_UNKNOWN = 0,
		EIT_STRING = 1,
		EIT_LONG = 2,
		EIT_DOUBLE = 3
	};

	ExtraInfoType m_Type = EIT_UNKNOWN;
	QString m_strName;
	QString m_strValue;
	QString m_strComment;
	int m_lValue = 0;
	double m_fValue = 0;
	bool m_bPropagate = false;
};
