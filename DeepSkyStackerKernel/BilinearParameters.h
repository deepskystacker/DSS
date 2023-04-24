#pragma once

typedef enum TRANSFORMATIONTYPE
{
	TT_LINEAR = 0,
	TT_BILINEAR = 1,
	TT_BISQUARED = 2,
	TT_BICUBIC = 3,
	TT_NONE = 4,
	TT_LAST = 5
} TRANSFORMATIONTYPE;

class QPointF;
class CBilinearParameters
{
public:
	TRANSFORMATIONTYPE		Type;
	double					a0, a1, a2, a3;
	double					a4, a5, a6, a7, a8;
	double					a9, a10, a11, a12, a13, a14, a15;
	double					b0, b1, b2, b3;
	double					b4, b5, b6, b7, b8;
	double					b9, b10, b11, b12, b13, b14, b15;

	double					fXWidth, fYWidth;

private:
	bool GetNextParameter(QString& strParameters, double& fValue) const;

public:
	CBilinearParameters()
	{
		Clear();
	}

	CBilinearParameters(const CBilinearParameters& bp) = default;

	CBilinearParameters& operator=(const CBilinearParameters& bp) = default;

	void Clear() noexcept;
	void ToText(QString& strText) const;
	bool FromText(const QString& szText);
	QPointF transform(const QPointF& pt) const noexcept;
	double Angle(int lWidth) const noexcept;
	void Offsets(double& dX, double& dY) const noexcept;
	void Footprint(QPointF& pt1, QPointF& pt2, QPointF& pt3, QPointF& pt4) const noexcept;
};
