#ifndef _HISTOGRAM_H__
#define _HISTOGRAM_H__

#include <math.h>

/* ------------------------------------------------------------------- */

inline double LinearAdjust(double fValue)
{
	ASSERT(fValue >= 0 && fValue <= 1);
	return fValue;
};

inline double CubeRootAdjust(double fValue)
{
	ASSERT(fValue >= 0 && fValue <= 1);
	fValue = pow(fValue, 1/3.0);
	ASSERT(fValue >= 0 && fValue <= 1);
	return fValue;
};

inline double SquareRootAdjust(double fValue)
{
	ASSERT(fValue >= 0 && fValue <= 1);
	fValue = pow(fValue, 1/2.0);
	ASSERT(fValue >= 0 && fValue <= 1);
	return fValue;
};

inline double LogAdjust(double fValue)
{
	ASSERT(fValue >= 0 && fValue <= 1);
	fValue = log(fValue * 1.7 + 1);
	ASSERT(fValue >= 0 && fValue <= 1);
	return fValue;
};

inline double LogLogAdjust(double fValue)
{
	ASSERT(fValue >= 0 && fValue <= 1);
	fValue = log(log(fValue * 1.7 + 1)*1.7+1);
	ASSERT(fValue >= 0 && fValue <= 1);
	return fValue;
};

inline double LogSquareRootAdjust(double fValue)
{
	ASSERT(fValue >= 0 && fValue <= 1);
	fValue = log(pow(fValue, 1/2.0)*1.7+1);
	ASSERT(fValue >= 0 && fValue <= 1);
	return fValue;
};

inline double AsinHAdjust(double fValue)
{
	ASSERT(fValue >= 0 && fValue <= 1);
	fValue *= 1.15;
	fValue = log(fValue + sqrt(fValue * fValue + 1));
	ASSERT(fValue >= 0 && fValue <= 1);
	return fValue;
};

typedef enum HISTOADJUSTTYPE
{
	HAT_LINEAR			= 1,
	HAT_CUBEROOT		= 2,
	HAT_SQUAREROOT		= 3,
	HAT_LOG				= 4,
	HAT_LOGLOG			= 5,
	HAT_LOGSQUAREROOT	= 6,
	HAT_ASINH			= 7
}HISTOADJUSTTYPE;


inline void	HistoAdjustTypeText(HISTOADJUSTTYPE hat, CString & strText)
{
	switch (hat)
	{
	case HAT_LINEAR	:
		strText.LoadString(IDS_HAT_LINEAR);
		break;
	case HAT_CUBEROOT :
		strText.LoadString(IDS_HAT_CUBEROOT);
		break;
	case HAT_SQUAREROOT :
		strText.LoadString(IDS_HAT_SQUAREROOT);
		break;
	case HAT_LOG :
		strText.LoadString(IDS_HAT_LOG);
		break;
	case HAT_LOGLOG :
		strText.LoadString(IDS_HAT_LOGLOG);
		break;
	case HAT_LOGSQUAREROOT :
		strText.LoadString(IDS_HAT_LOGSQUAREROOT);
		break;
	case HAT_ASINH :
		strText.LoadString(IDS_HAT_ASINH);
		break;
	};
};

class CHistogramAdjust
{
private :
	double					m_fMin;
	double					m_fMax;
	double					m_fShift;

	double					m_fOrgMin;
	double					m_fOrgMax;

	double					m_fUsedMin;
	double					m_fUsedMax;

	HISTOADJUSTTYPE			m_HAT;

private :
	void	CopyFrom(const CHistogramAdjust & ha)
	{
		m_fMin		= ha.m_fMin;
		m_fMax		= ha.m_fMax;
		m_fShift	= ha.m_fShift;

		m_fOrgMin	= ha.m_fOrgMin;
		m_fOrgMax	= ha.m_fOrgMax;

		m_fUsedMin	= ha.m_fUsedMin;
		m_fUsedMax	= ha.m_fUsedMax;

		m_HAT		= ha.m_HAT;
	};

	double	AdjustValue(double fValue) const
	{
		switch (m_HAT)
		{
		case HAT_CUBEROOT :
			return CubeRootAdjust(fValue);
			break;
		case HAT_SQUAREROOT :
			return SquareRootAdjust(fValue);
			break;
		case HAT_LOG :
			return LogAdjust(fValue);
			break;
		case HAT_LOGLOG	:
			return LogLogAdjust(fValue);
			break;
		case HAT_LOGSQUAREROOT :
			return LogSquareRootAdjust(fValue);
			break;
		case HAT_ASINH :
			return AsinHAdjust(fValue);
			break;
		case HAT_LINEAR	:
		default :
			return LinearAdjust(fValue);
			break;
		};
	};

	double	ExtractValue(LPCTSTR szString, LPCTSTR szVariable)
	{
		double				fValue = 0.0;
		CString				strString = szString;
		CString				strVariable = szVariable;
		int					nPos, nStart, nEnd;
		CString				strValue;

		strVariable += "=";
		nPos = strString.Find(strVariable, 0);
		if (nPos >= 0)
		{
			nStart = nPos + strVariable.GetLength();
			nEnd = strString.Find(_T(";"), nStart);
			if (nEnd < 0)
				nEnd = strString.Find(_T("}"), nStart);
			if (nEnd > nStart)
			{
				strValue = strString.Mid(nStart, nEnd-nStart);
				fValue = _ttof(strValue);
			};
		};

		return fValue;
	};

public :
	CHistogramAdjust()
	{
		m_HAT = HAT_LOGSQUAREROOT;
		SetOrgValues(0.0, 65535.0);
	};

	virtual ~CHistogramAdjust() {};

	void	Reset()
	{
		m_HAT = HAT_LOGSQUAREROOT;
		SetOrgValues(0.0, 65535.0);
		SetNewValues(0.0, 65535.0, 0.0);
	};

	CHistogramAdjust(const CHistogramAdjust & ha)
	{
		CopyFrom(ha);
	};

	CHistogramAdjust & operator = (const CHistogramAdjust & ha)
	{
		CopyFrom(ha);
		return (*this);
	};

	void	SetOrgValues(double fMin, double fMax)
	{
		m_fOrgMin = fMin;
		m_fOrgMax = fMax;
		m_fUsedMin = (m_fOrgMax-m_fOrgMin) * 0.05;
		m_fUsedMax = m_fOrgMax - m_fUsedMin;
	};

	void	SetNewValues(double fMin, double fMax, double fShift)
	{
		m_fMin   = fMin;
		m_fMax	 = fMax;
		m_fShift = fShift;
	};

	void	SetAdjustMethod(HISTOADJUSTTYPE hat)
	{
		m_HAT = hat;
	};

	HISTOADJUSTTYPE GetAdjustMethod() const
	{
		return m_HAT;
	};

	double	GetMin() const
	{
		return m_fMin;
	};

	double	GetMax() const
	{
		return m_fMax;
	};

	double	GetShift() const
	{
		return m_fShift;
	};

	double	Adjust(double fValue) const
	{
		double		fResult;

		if (fValue < m_fMin)
			fResult = m_fOrgMin + fValue/max(1, m_fMin - m_fOrgMin) * (m_fUsedMin - m_fOrgMin);
		else if (fValue > m_fMax)
			fResult = m_fOrgMax - (fValue - m_fMax)/max(1, m_fOrgMax - m_fMax) * (m_fOrgMax - m_fUsedMax);
		else 
			fResult = m_fUsedMin + AdjustValue((fValue-m_fMin)/max(1, (m_fMax - m_fMin)))*(m_fUsedMax - m_fUsedMin);

		// Then shift the value
		fResult = fResult + (m_fUsedMax - m_fUsedMin)*m_fShift;
		if (fResult < m_fOrgMin)
			fResult = m_fOrgMin + fValue/max(1, m_fMin - m_fOrgMin) * (m_fUsedMin - m_fOrgMin);
		else if (fResult > m_fOrgMax)
			fResult = m_fOrgMax - (fValue - m_fMax)/max(1, m_fOrgMax - m_fMax) * (m_fOrgMax - m_fUsedMax);

		return fResult;
	};

	BOOL	Load(FILE * hFile)
	{
		fread(&m_fMin, sizeof(m_fMin), 1, hFile);
		fread(&m_fMax, sizeof(m_fMax), 1, hFile);
		fread(&m_fShift, sizeof(m_fShift), 1, hFile);
		fread(&m_fOrgMin, sizeof(m_fOrgMin), 1, hFile);
		fread(&m_fOrgMax, sizeof(m_fOrgMax), 1, hFile);
		fread(&m_fUsedMin, sizeof(m_fUsedMin), 1, hFile);
		fread(&m_fUsedMax, sizeof(m_fUsedMax), 1, hFile);
		fread(&m_HAT, sizeof(m_HAT), 1, hFile);
		return TRUE;
	};

	BOOL	Save(FILE * hFile) const
	{
		fwrite(&m_fMin, sizeof(m_fMin), 1, hFile);
		fwrite(&m_fMax, sizeof(m_fMax), 1, hFile);
		fwrite(&m_fShift, sizeof(m_fShift), 1, hFile);
		fwrite(&m_fOrgMin, sizeof(m_fOrgMin), 1, hFile);
		fwrite(&m_fOrgMax, sizeof(m_fOrgMax), 1, hFile);
		fwrite(&m_fUsedMin, sizeof(m_fUsedMin), 1, hFile);
		fwrite(&m_fUsedMax, sizeof(m_fUsedMax), 1, hFile);
		fwrite(&m_HAT, sizeof(m_HAT), 1, hFile);
		return TRUE;
	};

	void	ToText(CString & strParameters) const
	{
		strParameters.Format(_T("Min=%2.f;Max=%.2f;Shift=%.2f;MinOrg=%.2f;MaxOrg=%2.f;MinUsed=%.2f;MaxUsed=%.2f;HAT=%ld;"),
							m_fMin, m_fMax, m_fShift, m_fOrgMin, m_fOrgMax, m_fUsedMin, m_fUsedMax, (WORD)m_HAT);
	};

	void	FromText(LPCTSTR szParameters)
	{
		m_fMin = ExtractValue(szParameters, _T("Min"));
		m_fMax = ExtractValue(szParameters, _T("Max"));
		m_fShift = ExtractValue(szParameters, _T("Shift"));
		m_fOrgMin = ExtractValue(szParameters, _T("MinOrg"));
		m_fOrgMax = ExtractValue(szParameters, _T("MaxOrg"));
		m_fUsedMin = ExtractValue(szParameters, _T("MinUsed"));
		m_fUsedMax = ExtractValue(szParameters, _T("MaxUsed"));

		LONG				lValue;

		lValue = ExtractValue(szParameters, _T("HAT"));
		m_HAT = (HISTOADJUSTTYPE)lValue;
	};
};

/* ------------------------------------------------------------------- */

class CRGBHistogramAdjust
{
private :
	CHistogramAdjust		m_RedAdjust;
	CHistogramAdjust		m_GreenAdjust;
	CHistogramAdjust		m_BlueAdjust;

	void	CopyFrom(const CRGBHistogramAdjust & ha)
	{
		m_RedAdjust		= ha.m_RedAdjust;
		m_GreenAdjust	= ha.m_GreenAdjust;
		m_BlueAdjust	= ha.m_BlueAdjust;
	};

	void	ExtractParameters(LPCTSTR szParameters, LPCSTR szSub, CHistogramAdjust & ha)
	{
		CString				strParameters = szParameters;
		CString				strSub = szSub;
		int					nPos, nStart, nEnd;
		CString				strSubParameters;

		strSub += "{";

		nPos = strParameters.Find(strSub);
		if (nPos >= 0)
		{
			nStart = nPos + strSub.GetLength();
			nEnd = strParameters.Find(_T("}"), nStart);
			if (nEnd > nStart)
			{
				strSubParameters = strParameters.Mid(nStart, nEnd-nStart);
				ha.FromText(strSubParameters);
			};
		};

	};

public :
	CRGBHistogramAdjust() {	};
	virtual ~CRGBHistogramAdjust() {};

	void	Reset()
	{
		m_RedAdjust.Reset();
		m_GreenAdjust.Reset();
		m_BlueAdjust.Reset();
	};

	CRGBHistogramAdjust(const CRGBHistogramAdjust & ha)
	{
		CopyFrom(ha);
	};

	CRGBHistogramAdjust & operator = (const CRGBHistogramAdjust & ha)
	{
		CopyFrom(ha);
		return (*this);
	};

	void	Adjust(double & fRed, double & fGreen, double & fBlue) const
	{
		fRed	= m_RedAdjust.Adjust(fRed);
		fGreen	= m_GreenAdjust.Adjust(fGreen);
		fBlue	= m_BlueAdjust.Adjust(fBlue);
	};

	CHistogramAdjust & GetRedAdjust() 
	{
		return m_RedAdjust;
	};

	CHistogramAdjust & GetGreenAdjust() 
	{
		return m_GreenAdjust;
	};

	CHistogramAdjust & GetBlueAdjust() 
	{
		return m_BlueAdjust;
	};

	BOOL	Load(FILE * hFile)
	{
		return m_RedAdjust.Load(hFile) && m_GreenAdjust.Load(hFile) && m_BlueAdjust.Load(hFile);
	};
	BOOL	Save(FILE * hFile) const
	{
		return m_RedAdjust.Save(hFile) && m_GreenAdjust.Save(hFile) && m_BlueAdjust.Save(hFile);
	};

	void	ToText(CString & strParameters) const
	{
		CString				strRedParameters;
		CString				strGreenParameters;
		CString				strBlueParameters;

		m_RedAdjust.ToText(strRedParameters);
		m_GreenAdjust.ToText(strGreenParameters);
		m_BlueAdjust.ToText(strBlueParameters);

		strParameters.Format(_T("RedAdjust{%s}GreenAdjust{%s}BlueAdjust{%s}"), 
					strRedParameters, 
					strGreenParameters, 
					strBlueParameters);
	};

	void	FromText(LPCTSTR szParameters)
	{
		CString				strRedParameters;
		CString				strGreenParameters;
		CString				strBlueParameters;

		ExtractParameters(szParameters, "RedAdjust", m_RedAdjust);
		ExtractParameters(szParameters, "GreenAdjust", m_GreenAdjust);
		ExtractParameters(szParameters, "BlueAdjust", m_BlueAdjust);
	};
};

/* ------------------------------------------------------------------- */

class CHistogram 
{
private :
	std::vector<DWORD>		m_vValues;
	LONG					m_lMax;
	double					m_fAbsMax;
	double					m_fMax;
	double					m_fMin;
	double					m_fStep;
	double					m_fSum;
	double					m_fPowSum;
	LONG					m_lNrValues;
	BOOL					m_bInitOk;

public :
	CHistogram() 
	{
		m_fSum		= 0;
		m_fPowSum	= 0;
		m_lNrValues = 0;
		m_lMax		= 0;
		m_fMax		= 0;
		m_fMin		= -1;
		m_bInitOk	= FALSE;
	};

	virtual ~CHistogram() {};

	void	Init()
	{
		LONG		lNrValues = 0;

		m_bInitOk = FALSE;
		Clear();
		lNrValues = (LONG)(m_fAbsMax/m_fStep+1);

		m_vValues.resize(lNrValues);

		m_bInitOk = TRUE;
	};

	void	Clear()
	{
		m_vValues.clear();
		m_fSum		= 0;
		m_fPowSum	= 0;
		m_lNrValues = 0;
		m_lMax		= 0;
		m_fMax		= 0;
		m_fMin		= -1;

		if (m_bInitOk)
		{
			LONG		lNrValues;

			lNrValues = (LONG)(m_fAbsMax/m_fStep+1);
			m_vValues.resize(lNrValues);
		};
	};

	void	SetSize(double fMax, double fStep)
	{
		m_fAbsMax = fMax;
		m_fStep	  = fStep;

		Init();
	};

	void	SetSize(double fMax, LONG lNrValues)
	{
		m_fAbsMax	= fMax;
		m_fStep		= fMax/(lNrValues-1);

		Init();
	};

	LONG	GetSize()
	{
		return (LONG)m_vValues.size();
	};

	void	AddValue(double fValue, LONG lNrValues = 1)
	{
		LONG		lNrStep;

		lNrStep = (LONG)(fValue/m_fStep);

		if (lNrStep < m_vValues.size())
		{
			m_vValues[lNrStep]+=lNrValues;
			m_lNrValues+=lNrValues;
			m_fPowSum += (fValue*fValue)*lNrValues;
			m_fSum	  += fValue * lNrValues;
			m_lMax	  = max(m_lMax, m_vValues[lNrStep]);

			m_fMax = max(m_fMax, fValue);
			if (m_fMin < 0)
				m_fMin = fValue;
			else
				m_fMin = min(m_fMin, fValue);
		};
	};

	void	AddValues(const CHistogram & Histogram)
	{
		for (LONG i = 0;i<Histogram.m_vValues.size();i++)
		{
			if (Histogram.m_vValues[i])
				AddValue(i*m_fStep, Histogram.m_vValues[i]);
		};
	};

	LONG	GetNrValues()
	{
		return (LONG)m_vValues.size();
	};

	LONG	GetValue(double fValue)
	{
		return m_vValues[(LONG)(fValue/m_fStep)];
	};

	LONG	GetValue(LONG lValue)
	{
		return m_vValues[lValue];
	};

	double	GetComponentValue(LONG lIndice)
	{
		return (double)lIndice * m_fStep;
	};

	double	GetAverage()
	{
		double		fResult = 0;

		if (m_lNrValues)
			fResult = m_fSum/m_lNrValues;

		return fResult;
	};

	double	GetMin()
	{
		return m_fMin;
	};

	double	GetMax()
	{
		return m_fMax;
	};

	double	GetStdDeviation()
	{
		double		fResult = 0;

		if (m_lNrValues)
			fResult = sqrt(m_fPowSum/m_lNrValues - pow(m_fSum/m_lNrValues, 2));

		return fResult;
	};

	double	GetMedian()
	{
		double		fResult = 0;

		if (m_lNrValues)
		{
			LONG		lCount = 0;
			LONG		i = 0;

			while ((lCount + m_vValues[i]) <= m_lNrValues/2)
			{
				lCount += m_vValues[i];
				i++;
			};
			// The median is i
			fResult = i*m_fStep;
		};

		return fResult;
	};

	LONG	GetMaximumNrValues()
	{
		return m_lMax;
	};
};

/* ------------------------------------------------------------------- */

class CRGBHistogram
{
private :
	CHistogram				m_RedHisto;
	CHistogram				m_GreenHisto;
	CHistogram				m_BlueHisto;

public :
	CRGBHistogram() {};
	virtual ~CRGBHistogram() {};

	void	Clear()
	{
		m_RedHisto.Clear();
		m_GreenHisto.Clear();
		m_BlueHisto.Clear();
	};

	BOOL	IsInitialized()
	{
		return m_RedHisto.GetSize() && m_GreenHisto.GetSize() && m_BlueHisto.GetSize();
	};

	LONG	GetSize()
	{
		return m_RedHisto.GetSize();
	};

	void	SetSize(double fMax, double fStep)
	{
		m_RedHisto.SetSize(fMax, fStep);
		m_GreenHisto.SetSize(fMax, fStep);
		m_BlueHisto.SetSize(fMax, fStep);
	};

	void	SetSize(double fMax, LONG lNrValues)
	{
		m_RedHisto.SetSize(fMax, lNrValues);
		m_GreenHisto.SetSize(fMax, lNrValues);
		m_BlueHisto.SetSize(fMax, lNrValues);
	};

	void	AddValues(double fRed, double fGreen, double fBlue)
	{
		m_RedHisto.AddValue(fRed);
		m_GreenHisto.AddValue(fGreen);
		m_BlueHisto.AddValue(fBlue);
	};

	void	AddValues(const CRGBHistogram & RGBHistogram)
	{
		m_RedHisto.AddValues(RGBHistogram.m_RedHisto);
		m_GreenHisto.AddValues(RGBHistogram.m_GreenHisto);
		m_BlueHisto.AddValues(RGBHistogram.m_BlueHisto);
	};

	void	GetValues(LONG lValue, LONG & lNrReds, LONG & lNrGreens, LONG & lNrBlues)
	{
		lNrReds		= m_RedHisto.GetValue(lValue);
		lNrGreens	= m_GreenHisto.GetValue(lValue);
		lNrBlues	= m_BlueHisto.GetValue(lValue); 
	};

	CHistogram & GetRedHistogram()
	{
		return m_RedHisto;
	};

	CHistogram & GetGreenHistogram()
	{
		return m_GreenHisto;
	};

	CHistogram & GetBlueHistogram()
	{
		return m_BlueHisto;
	};
};

/* ------------------------------------------------------------------- */

#endif // _HISTOGRAM_H__