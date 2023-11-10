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

bool CBilinearParameters::GetNextParameter(QString& strParameters, double& fValue) const
{
	if (strParameters.isEmpty())
		return false;

	QString strValue(strParameters);
	qsizetype nPos = strParameters.indexOf(",");
	if (nPos >= 0)
		strValue = strParameters.mid(0, nPos);
	fValue = strValue.toFloat();

	strParameters = strParameters.mid(nPos+1, std::max(0, (int)(strParameters.length() - strValue.length() - 1)));
	return true;
}

void CBilinearParameters::ToText(QString& strText) const
{
	if (Type == TT_NONE)
	{
		strText = QString("None(%.20g,%.20g)").arg(fXWidth, 0, 'f', 2).arg(fYWidth, 0, 'g', 20);
	}
	else if (Type == TT_BICUBIC)
	{
		strText = QString("Bicubic(%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15,%16,%17,%18,%19,%20,%21,%22,%23,%24,%25,%26,%27,%28,%29,%30,%31,%32,%33,%34)")
			.arg(a0, 0, 'g', 20)
			.arg(a1, 0, 'g', 20)
			.arg(a2, 0, 'g', 20)
			.arg(a3, 0, 'g', 20)
			.arg(a4, 0, 'g', 20)
			.arg(a5, 0, 'g', 20)
			.arg(a6, 0, 'g', 20)
			.arg(a7, 0, 'g', 20)
			.arg(a8, 0, 'g', 20)
			.arg(a9, 0, 'g', 20)
			.arg(a10, 0, 'g', 20)
			.arg(a11, 0, 'g', 20)
			.arg(a12, 0, 'g', 20)
			.arg(a13, 0, 'g', 20)
			.arg(a14, 0, 'g', 20)
			.arg(a15, 0, 'g', 20)
			.arg(b0, 0, 'g', 20)
			.arg(b1, 0, 'g', 20)
			.arg(b2, 0, 'g', 20)
			.arg(b3, 0, 'g', 20)
			.arg(b4, 0, 'g', 20)
			.arg(b5, 0, 'g', 20)
			.arg(b6, 0, 'g', 20)
			.arg(b7, 0, 'g', 20)
			.arg(b8, 0, 'g', 20)
			.arg(b9, 0, 'g', 20)
			.arg(b10, 0, 'g', 20)
			.arg(b11, 0, 'g', 20)
			.arg(b12, 0, 'g', 20)
			.arg(b13, 0, 'g', 20)
			.arg(b14, 0, 'g', 20)
			.arg(b15, 0, 'g', 20)
			.arg(fXWidth, 0, 'g', 20)
			.arg(fYWidth, 0, 'g', 20);
	}
	else if (Type == TT_BISQUARED)
	{
		strText = QString("Bisquared(%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15,%16,%17,%18,%19,%20)")
			.arg(a0, 0, 'g', 20)
			.arg(a1, 0, 'g', 20)
			.arg(a2, 0, 'g', 20)
			.arg(a3, 0, 'g', 20)
			.arg(a4, 0, 'g', 20)
			.arg(a5, 0, 'g', 20)
			.arg(a6, 0, 'g', 20)
			.arg(a7, 0, 'g', 20)
			.arg(a8, 0, 'g', 20)
			.arg(b0, 0, 'g', 20)
			.arg(b1, 0, 'g', 20)
			.arg(b2, 0, 'g', 20)
			.arg(b3, 0, 'g', 20)
			.arg(b4, 0, 'g', 20)
			.arg(b5, 0, 'g', 20)
			.arg(b6, 0, 'g', 20)
			.arg(b7, 0, 'g', 20)
			.arg(b8, 0, 'g', 20)
			.arg(fXWidth, 0, 'g', 20)
			.arg(fYWidth, 0, 'g', 20);
	}
	else
	{
		strText = QString("Bilinear(%1,%2,%3,%4,%5,%6,%7,%8,%9,%10)")
			.arg(a0, 0, 'g', 20)
			.arg(a1, 0, 'g', 20)
			.arg(a2, 0, 'g', 20)
			.arg(a3, 0, 'g', 20)
			.arg(b0, 0, 'g', 20)
			.arg(b1, 0, 'g', 20)
			.arg(b2, 0, 'g', 20)
			.arg(b3, 0, 'g', 20)
			.arg(fXWidth, 0, 'g', 20)
			.arg(fYWidth, 0, 'g', 20);
	};
}

bool CBilinearParameters::FromText(const QString& szText)
{
	qsizetype nPos1 = szText.indexOf("(");
	qsizetype nPos2 = szText.indexOf(")");
	if (!(nPos1 >= 0 && nPos2 > nPos1))
		return false;

	const QString strType(szText.left(nPos1));
	QString strParameters(szText.mid(nPos1 + 1, nPos2 - nPos1 - 1));
	if (strType.compare("None", Qt::CaseInsensitive) == 0)
	{
		Clear();
		Type = TT_NONE;
		return (GetNextParameter(strParameters, fXWidth) && GetNextParameter(strParameters, fYWidth));
	}
	else if (strType.compare("Bilinear", Qt::CaseInsensitive) == 0)
	{
		Type = TT_BILINEAR;
		return (GetNextParameter(strParameters, a0) &&
				GetNextParameter(strParameters, a1) &&
				GetNextParameter(strParameters, a2) &&
				GetNextParameter(strParameters, a3) &&
				GetNextParameter(strParameters, b0) &&
				GetNextParameter(strParameters, b1) &&
				GetNextParameter(strParameters, b2) &&
				GetNextParameter(strParameters, b3) &&
				GetNextParameter(strParameters, fXWidth) &&
				GetNextParameter(strParameters, fYWidth));
	}
	else if (strType.compare("Bisquared", Qt::CaseInsensitive) == 0)
	{
		Type = TT_BISQUARED;
		return (GetNextParameter(strParameters, a0) &&
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
				GetNextParameter(strParameters, fYWidth));
	}
	else if (strType.compare("Bicubic", Qt::CaseInsensitive) == 0)
	{
		Type = TT_BICUBIC;
		return (GetNextParameter(strParameters, a0) &&
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
				GetNextParameter(strParameters, fYWidth));
	}
	return true;
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