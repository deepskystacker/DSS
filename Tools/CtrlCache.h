#pragma once
#include <map>

class CWnd;
class CCtrlCache
{
public:
	CCtrlCache(CWnd* pParent) : m_pParent(pParent)
	{}
	virtual ~CCtrlCache() {}

	void AddToCtrlCache(const int id);
	void UpdateCtrlCache();
	void InvalidateCtrls() const;
	const CRect& GetCtrlSize(const int id) const;
	const CPoint& GetCtrlOffset(const int id) const;

	void MoveCtrlHoriz(const int nId, const int nXDelta);
	void MoveCtrlVert(const int nId, const int nYDelta);
	void SizeCtrlHoriz(const int nId, const int nWidth);
	void SizeCtrlVert(const int nId, const int nHeight);

protected:
	CWnd * m_pParent;
	std::map<const int, CRect> m_mapCtrlSizes;
	std::map<const int, CPoint> m_mapCtrlOffsets;

protected:
	static const CRect gDefRect;
	static const CPoint gDefPoint;
};