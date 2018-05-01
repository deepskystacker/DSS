#ifndef _BEZIERADJUST_H__
#define _BEZIERADJUST_H__

/* ------------------------------------------------------------------- */

#ifndef PI
#define PI 3.141592654
#endif

class CBezierCurvePoint
{
public :
	double				x,
						y;

private :
	void	CopyFrom(const CBezierCurvePoint & bcp)
	{
		x = bcp.x;
		y = bcp.y;
	};

public :
	CBezierCurvePoint(double nx = 0, double ny = 0) 
	{
		x = nx;
		y = ny;
	};

	CBezierCurvePoint(const CBezierCurvePoint & bcp)
	{
		CopyFrom(bcp);
	};

	virtual ~CBezierCurvePoint() {};

	CBezierCurvePoint & operator = (const CBezierCurvePoint & bcp)
	{
		CopyFrom(bcp);
		return *this;
	};

	bool operator < (const CBezierCurvePoint & bcp) const
	{
		return x < bcp.x;
	}
};

typedef std::vector<CBezierCurvePoint>		BEZIERCURVEPOINTVECTOR;
typedef BEZIERCURVEPOINTVECTOR::iterator	BEZIERCURVEPOINTITERATOR;

class CBezierAdjust
{
public :
	BEZIERCURVEPOINTVECTOR	m_vPoints;

	double				m_fDarknessAngle,
						m_fDarknessPower,
						m_fMidtone,
						m_fMidtoneAngle,
						m_fHighlightAngle,
						m_fHighlightPower;
	double				m_fSaturationShift;

private :
	void CopyFrom(const CBezierAdjust & ba)
	{
		m_fDarknessAngle = ba.m_fDarknessAngle;
		m_fDarknessPower = ba.m_fDarknessPower;
		m_fMidtone		 = ba.m_fMidtone;
		m_fMidtoneAngle	 = ba.m_fMidtoneAngle;
		m_fHighlightAngle= ba.m_fHighlightAngle;
		m_fHighlightPower= ba.m_fHighlightPower;
		m_fSaturationShift= ba.m_fSaturationShift;

		m_vPoints		 = ba.m_vPoints;
	};

	void AddBezierPoints(double x1, double y1, double vx1, double vy1, double x2, double y2, double vx2, double vy2)
	{
		for (double t = 0;t<=1.0; t+= 0.01)
		{
			CBezierCurvePoint	pt;

			pt.x = (double)x1*pow((1-t), 3) + 3*vx1*pow((1-t), 2)*t + 3*vx2*(1-t)*t*t + x2*t*t*t;
			pt.y = (double)y1*pow((1-t), 3) + 3*vy1*pow((1-t), 2)*t + 3*vy2*(1-t)*t*t + y2*t*t*t;
			
			m_vPoints.push_back(pt);
		};
	};

	void Init()
	{
		CBezierCurvePoint	pt1, pt2, pt3, pt4;
		double				lSize = 1.0;

		m_vPoints.clear();

		// Set the first 4 control points of the bezier curve
		pt1.x = 0;			
		pt1.y = 0;
		pt2.x = lSize * cos(m_fDarknessAngle * PI /180.0) * m_fDarknessPower * m_fMidtone / 100.0 / 100.0;
		pt2.y = sin(m_fDarknessAngle * PI /180.0) * lSize * m_fDarknessPower * m_fMidtone /100.0 / 100.0;

		pt3.x = lSize * m_fMidtone / 100.0;
		pt3.y = lSize* m_fMidtone / 100.0;
		pt4 = pt3;
		pt4.x -= sin(m_fMidtoneAngle*PI/180.0) * lSize * m_fDarknessPower * m_fMidtone /100.0 / 100.0;
		pt4.y -= cos(m_fMidtoneAngle*PI/180.0) * lSize * m_fDarknessPower * m_fMidtone /100.0 / 100.0;

		AddBezierPoints(pt1.x, pt1.y, pt2.x, pt2.y, pt3.x, pt3.y, pt4.x, pt4.y);

		// Set the next 4 control points of the bezier curve
		pt1 = pt3;
		pt2.x = pt1.x + sin(m_fMidtoneAngle*PI/180.0) * lSize * m_fHighlightPower /100.0;
		pt2.y = pt1.y + cos(m_fMidtoneAngle*PI/180.0) * lSize * m_fHighlightPower /100.0;;
		pt3.x = lSize;
		pt3.y = lSize;
		pt4 = pt3;
		pt4.x -= cos(m_fHighlightAngle * PI /180.0) * lSize * m_fHighlightPower * (100.0 - m_fMidtone)/ 100.0 / 100.0;
		pt4.y -= sin(m_fHighlightAngle * PI /180.0) * lSize * m_fHighlightPower * (100.0 - m_fMidtone) /100.0 / 100.0;

		AddBezierPoints(pt1.x, pt1.y, pt2.x, pt2.y, pt3.x, pt3.y, pt4.x, pt4.y);

		std::sort(m_vPoints.begin(), m_vPoints.end());
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
	CBezierAdjust() 
	{
		Reset();
	};
	CBezierAdjust(const CBezierAdjust & ba)
	{
		CopyFrom(ba);
	};

	virtual ~CBezierAdjust() {};
	CBezierAdjust & operator = (const CBezierAdjust & ba)
	{
		CopyFrom(ba);
		return *this;
	};


	void	Reset(BOOL bNeutral = FALSE)
	{
		if (bNeutral)
		{
			m_fDarknessAngle = 0;
			m_fDarknessPower = 0;
			m_fMidtone		 = 50;
			m_fMidtoneAngle	 = 45;
			m_fHighlightAngle= 0;
			m_fHighlightPower= 0;
			m_fSaturationShift = 0;
		}
		else
		{
			m_fDarknessAngle = 0;
			m_fDarknessPower = 80;
			m_fMidtone		 = 33;
			m_fMidtoneAngle	 = 10;
			m_fHighlightAngle= 0;
			m_fHighlightPower= 50;
			m_fSaturationShift = 0;
		};
		Clear();
	};

	void	Clear()
	{
		m_vPoints.clear();
		Init();
	};

	double	GetValue(double x)
	{
		double			fResult = x;

		if (!m_vPoints.size())
			Init();

		BEZIERCURVEPOINTITERATOR	it;

		it = std::lower_bound(m_vPoints.begin(), m_vPoints.end(), CBezierCurvePoint(x, 0));
		if (it != m_vPoints.end())
			fResult = it->y;

		return fResult;
	};

	double	AdjustSaturation(double S)
	{
		double			fResult = S;

		if (m_fSaturationShift > 0)
			fResult = pow(S, 1.0/(m_fSaturationShift/10.0));
		else if (m_fSaturationShift < 0)
			fResult = pow(S, -m_fSaturationShift/10.0);

		return fResult;
	};

	BOOL	Load(FILE * hFile)
	{
		fread(&m_fDarknessAngle, sizeof(m_fDarknessAngle), 1, hFile);
		fread(&m_fDarknessPower, sizeof(m_fDarknessPower), 1, hFile);
		fread(&m_fMidtone, sizeof(m_fMidtone), 1, hFile);
		fread(&m_fMidtoneAngle, sizeof(m_fMidtoneAngle), 1, hFile);
		fread(&m_fHighlightAngle, sizeof(m_fHighlightAngle), 1, hFile);
		fread(&m_fHighlightPower, sizeof(m_fHighlightPower), 1, hFile);
		fread(&m_fSaturationShift, sizeof(m_fSaturationShift), 1, hFile);

		Init();
		return TRUE;
	};

	BOOL	Save(FILE * hFile)
	{
		fwrite(&m_fDarknessAngle, sizeof(m_fDarknessAngle), 1, hFile);
		fwrite(&m_fDarknessPower, sizeof(m_fDarknessPower), 1, hFile);
		fwrite(&m_fMidtone, sizeof(m_fMidtone), 1, hFile);
		fwrite(&m_fMidtoneAngle, sizeof(m_fMidtoneAngle), 1, hFile);
		fwrite(&m_fHighlightAngle, sizeof(m_fHighlightAngle), 1, hFile);
		fwrite(&m_fHighlightPower, sizeof(m_fHighlightPower), 1, hFile);
		fwrite(&m_fSaturationShift, sizeof(m_fSaturationShift), 1, hFile);

		return TRUE;
	};

	void	ToText(CString & strParameters)
	{
		strParameters.Format(_T("Bezier{DA=%.2f;DP=%.2f;MA=%.2f;MP=%.2f;HA=%.2f;HP=%.2f;SS=%.2f;}"), 
							 m_fDarknessAngle, m_fDarknessPower, m_fMidtoneAngle, m_fMidtone,
							 m_fHighlightAngle, m_fHighlightPower, m_fSaturationShift);
	};

	void	FromText(LPCTSTR szParameters)
	{
		CString			strParameters = szParameters;

		m_fDarknessAngle	= ExtractValue(szParameters, _T("DA"));
		m_fDarknessPower	= ExtractValue(szParameters, _T("DP"));
		m_fMidtoneAngle		= ExtractValue(szParameters, _T("MA"));
		m_fMidtone			= ExtractValue(szParameters, _T("MP"));
		m_fHighlightAngle	= ExtractValue(szParameters, _T("HA"));
		m_fHighlightPower	= ExtractValue(szParameters, _T("HP"));
		m_fSaturationShift	= ExtractValue(szParameters, _T("SS"));
	};
};

/* ------------------------------------------------------------------- */

#endif // _BEZIERADJUST_H__