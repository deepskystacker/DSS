#include "stdafx.h"
#include "BilinearParameters.h"

void CBilinearParameters::Clear() noexcept
{
	Type = TT_BILINEAR;
	a0 = a1 = a2 = a3 = a4 = a5 = a6 = a7 = a8 = 0.0;
	a9 = a10 = a11 = a12 = a13 = a14 = a15 = 0.0;
	b0 = b1 = b2 = b3 = b4 = b5 = b6 = b7 = b8 = 0.0;
	b9 = b10 = b11 = b12 = b13 = b14 = b15 = 0.0;
	a1 = 1.0;	// to have x' = x
	b2 = 1.0;	// to have y' = y

	fXWidth = fYWidth = 1.0;
}

bool CBilinearParameters::GetNextParameter(CString& strParameters, double& fValue) const
{
	bool			bResult = false;
	int				nPos;
	CString			strValue;

	if (strParameters.GetLength())
	{
		nPos = strParameters.Find(_T(","));
		if (nPos >= 0)
			strValue = strParameters.Left(nPos);
		else
			strValue = strParameters;

		fValue = _ttof((LPCTSTR)strValue);	// Change _ttof to _ttof for Unicode
		strParameters = strParameters.Right(std::max(0, strParameters.GetLength() - strValue.GetLength() - 1));
		bResult = true;
	};

	return bResult;
}

void CBilinearParameters::ToText(CString& strText) const
{
	if (Type == TT_NONE)
	{
		strText.Format(_T("None(%.20g,%.20g)"), fXWidth, fYWidth);
	}
	else if (Type == TT_BICUBIC)
	{
		CString			strText1;

		strText1.Format(_T("Bicubic(%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,"),
			a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15);
		strText = strText1;

		strText1.Format(_T("%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,"),
			b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11, b12, b13, b14, b15);
		strText += strText1;
		strText1.Format(_T("%.20g,%.20g)"),
			fXWidth, fYWidth);
		strText += strText1;
	}
	else if (Type == TT_BISQUARED)
	{
		CString			strText1;

		strText1.Format(_T("Bisquared(%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,"),
			a0, a1, a2, a3, a4, a5, a6, a7, a8);
		strText = strText1;

		strText1.Format(_T("%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,%.20g)"),
			b0, b1, b2, b3, b4, b5, b6, b7, b8,
			fXWidth, fYWidth);
		strText += strText1;
	}
	else
	{
		strText.Format(_T("Bilinear(%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,%.20g,%.20g)"),
			a0, a1, a2, a3,
			b0, b1, b2, b3,
			fXWidth, fYWidth);
	};
}

bool CBilinearParameters::FromText(LPCTSTR szText)
{
	bool			bResult = false;
	CString			strText = szText;
	int				nPos1,
		nPos2;

	nPos1 = strText.Find(_T("("));
	nPos2 = strText.Find(_T(")"));
	if (nPos1 >= 0 && nPos2 > nPos1)
	{
		CString		strType;
		CString		strParameters;

		strType = strText.Left(nPos1);
		strParameters = strText.Mid(nPos1 + 1, nPos2 - nPos1 - 1);
		if (!strType.CompareNoCase(_T("None")))
		{
			Clear();
			Type = TT_NONE;
			bResult = GetNextParameter(strParameters, fXWidth) &&
				GetNextParameter(strParameters, fYWidth);
		}
		else if (!strType.CompareNoCase(_T("Bilinear")))
		{
			Type = TT_BILINEAR;
			bResult = GetNextParameter(strParameters, a0) &&
				GetNextParameter(strParameters, a1) &&
				GetNextParameter(strParameters, a2) &&
				GetNextParameter(strParameters, a3) &&
				GetNextParameter(strParameters, b0) &&
				GetNextParameter(strParameters, b1) &&
				GetNextParameter(strParameters, b2) &&
				GetNextParameter(strParameters, b3) &&
				GetNextParameter(strParameters, fXWidth) &&
				GetNextParameter(strParameters, fYWidth);
		}
		else if (!strType.CompareNoCase(_T("Bisquared")))
		{
			Type = TT_BISQUARED;
			bResult = GetNextParameter(strParameters, a0) &&
				GetNextParameter(strParameters, a1) &&
				GetNextParameter(strParameters, a2) &&
				GetNextParameter(strParameters, a3) &&
				GetNextParameter(strParameters, a4) &&
				GetNextParameter(strParameters, a5) &&
				GetNextParameter(strParameters, a6) &&
				GetNextParameter(strParameters, a7) &&
				GetNextParameter(strParameters, a8) &&
				GetNextParameter(strParameters, b0) &&
				GetNextParameter(strParameters, b1) &&
				GetNextParameter(strParameters, b2) &&
				GetNextParameter(strParameters, b3) &&
				GetNextParameter(strParameters, b4) &&
				GetNextParameter(strParameters, b5) &&
				GetNextParameter(strParameters, b6) &&
				GetNextParameter(strParameters, b7) &&
				GetNextParameter(strParameters, b8) &&
				GetNextParameter(strParameters, fXWidth) &&
				GetNextParameter(strParameters, fYWidth);
		}
		else if (!strType.CompareNoCase(_T("Bicubic")))
		{
			Type = TT_BICUBIC;
			bResult = GetNextParameter(strParameters, a0) &&
				GetNextParameter(strParameters, a1) &&
				GetNextParameter(strParameters, a2) &&
				GetNextParameter(strParameters, a3) &&
				GetNextParameter(strParameters, a4) &&
				GetNextParameter(strParameters, a5) &&
				GetNextParameter(strParameters, a6) &&
				GetNextParameter(strParameters, a7) &&
				GetNextParameter(strParameters, a8) &&
				GetNextParameter(strParameters, a9) &&
				GetNextParameter(strParameters, a10) &&
				GetNextParameter(strParameters, a11) &&
				GetNextParameter(strParameters, a12) &&
				GetNextParameter(strParameters, a13) &&
				GetNextParameter(strParameters, a14) &&
				GetNextParameter(strParameters, a15) &&
				GetNextParameter(strParameters, b0) &&
				GetNextParameter(strParameters, b1) &&
				GetNextParameter(strParameters, b2) &&
				GetNextParameter(strParameters, b3) &&
				GetNextParameter(strParameters, b4) &&
				GetNextParameter(strParameters, b5) &&
				GetNextParameter(strParameters, b6) &&
				GetNextParameter(strParameters, b7) &&
				GetNextParameter(strParameters, b8) &&
				GetNextParameter(strParameters, b9) &&
				GetNextParameter(strParameters, b10) &&
				GetNextParameter(strParameters, b11) &&
				GetNextParameter(strParameters, b12) &&
				GetNextParameter(strParameters, b13) &&
				GetNextParameter(strParameters, b14) &&
				GetNextParameter(strParameters, b15) &&
				GetNextParameter(strParameters, fXWidth) &&
				GetNextParameter(strParameters, fYWidth);
		};
	};

	return bResult;
}
QPointF CBilinearParameters::transform(const QPointF& pt) const noexcept
{
	QPointF ptResult;
	qreal& x = ptResult.rx();
	qreal& y = ptResult.ry();


	if (Type == TT_BICUBIC)
	{
		qreal			X = pt.x() / fXWidth;
		qreal			X2 = X * X;
		qreal			X3 = X * X * X;
		qreal			Y = pt.y() / fYWidth;
		qreal			Y2 = Y * Y;
		qreal			Y3 = Y * Y * Y;

		x = a0 + a1 * X + a2 * Y + a3 * X * Y
			+ a4 * X2 + a5 * Y2 + a6 * X2 * Y + a7 * X * Y2 + a8 * X2 * Y2
			+ a9 * X3 + a10 * Y3 + a11 * X3 * Y + a12 * X * Y3 + a13 * X3 * Y2 + a14 * X2 * Y3 + a15 * X3 * Y3;
		y = b0 + b1 * X + b2 * Y + b3 * X * Y
			+ b4 * X2 + b5 * Y2 + b6 * X2 * Y + b7 * X * Y2 + b8 * X2 * Y2
			+ b9 * X3 + b10 * Y3 + b11 * X3 * Y + b12 * X * Y3 + b13 * X3 * Y2 + b14 * X2 * Y3 + b15 * X3 * Y3;
	}
	else if (Type == TT_BISQUARED)
	{
		qreal			X = pt.x() / fXWidth;
		qreal			X2 = X * X;
		qreal			Y = pt.y() / fYWidth;
		qreal			Y2 = Y * Y;

		x = a0 + a1 * X + a2 * Y + a3 * X * Y
			+ a4 * X2 + a5 * Y2 + a6 * X2 * Y + a7 * X * Y2 + a8 * X2 * Y2;
		y = b0 + b1 * X + b2 * Y + b3 * X * Y
			+ b4 * X2 + b5 * Y2 + b6 * X2 * Y + b7 * X * Y2 + b8 * X2 * Y2;
	}
	else
	{
		qreal			X = pt.x() / fXWidth;
		qreal			Y = pt.y() / fYWidth;

		x = a0 + a1 * X + a2 * Y + a3 * X * Y;
		y = b0 + b1 * X + b2 * Y + b3 * X * Y;
	};

	x *= fXWidth;
	y *= fYWidth;

	return ptResult;
}

double CBilinearParameters::Angle(int lWidth) const noexcept
{
	double		fAngle;
	QPointF	pt1(0, 0),
		pt2(lWidth, 0);

	pt1 = transform(pt1);
	pt2 = transform(pt2);

	fAngle = atan2(pt2.y() - pt1.y(), pt2.x() - pt1.x());

	return fAngle;
}

void CBilinearParameters::Offsets(double& dX, double& dY) const noexcept
{
	dX = a0 * fXWidth;
	dY = b0 * fYWidth;
}

void CBilinearParameters::Footprint(QPointF& pt1, QPointF& pt2, QPointF& pt3, QPointF& pt4) const noexcept
{
	pt1.rx() = pt1.ry() = 0;
	pt2.rx() = fXWidth;	pt2.ry() = 0;
	pt3.rx() = fXWidth;	pt3.ry() = fYWidth;
	pt4.rx() = 0;		pt4.ry() = fYWidth;

	pt1 = transform(pt1);
	pt2 = transform(pt2);
	pt3 = transform(pt3);
	pt4 = transform(pt4);
}