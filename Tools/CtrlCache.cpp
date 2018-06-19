#include "stdafx.h"
#include "CtrlCache.h"
const CRect CCtrlCache::gDefRect(0, 0, 0, 0);
const CPoint CCtrlCache::gDefPoint(0, 0);
void CCtrlCache::AddToCtrlCache(const int id)
{
	m_mapCtrlSizes[id] = gDefRect;
	m_mapCtrlOffsets[id] = gDefPoint;
}

void CCtrlCache::UpdateCtrlCache()
{
	if (m_pParent == nullptr)
		return;

	CWnd* pCtrl = nullptr;
	CRect rect;
	for (auto& entry : m_mapCtrlSizes)
	{
		pCtrl = m_pParent->GetDlgItem(entry.first);
		if (pCtrl)
		{
			pCtrl->GetWindowRect(&rect);
			entry.second = rect;
		}
	}
	for (auto& entry : m_mapCtrlOffsets)
	{
		pCtrl = m_pParent->GetDlgItem(entry.first);
		if (pCtrl)
		{
			pCtrl->GetWindowRect(&rect);
			pCtrl->GetParent()->ScreenToClient(rect);
			entry.second = rect.TopLeft();
		}
	}
}

void CCtrlCache::InvalidateCtrls() const
{
	if (m_pParent == nullptr)
		return;

	for (auto& entry : m_mapCtrlSizes)
	{
		CWnd* pCtrl = m_pParent->GetDlgItem(entry.first);
		if (pCtrl)
			pCtrl->Invalidate();
	}
}

const CRect& CCtrlCache::GetCtrlSize(const int id) const
{
	const auto iter = m_mapCtrlSizes.find(id);
	if (iter == m_mapCtrlSizes.end())
		return CCtrlCache::gDefRect;
	return iter->second;
}

const CPoint& CCtrlCache::GetCtrlOffset(const int id) const
{
	const auto iter = m_mapCtrlOffsets.find(id);
	if (iter == m_mapCtrlOffsets.end())
		return CCtrlCache::gDefPoint;
	return iter->second;
}


void CCtrlCache::MoveCtrlVert(const int nId, const int nYDelta)
{
	if (m_pParent == nullptr)
		return;

	CWnd* pWnd = m_pParent->GetDlgItem(nId);
	if (pWnd == nullptr)
		return;

	CRect pos;
	pWnd->GetWindowRect(&pos);
	pos.top += nYDelta;
	pos.bottom += nYDelta;
	pWnd->GetParent()->ScreenToClient(pos);
	pWnd->MoveWindow(pos);
}

void CCtrlCache::MoveCtrlHoriz(const int nId, const int nXDelta)
{
	if (m_pParent == nullptr)
		return;

	CWnd* pWnd = m_pParent->GetDlgItem(nId);
	if (pWnd == nullptr)
		return;

	CRect pos;
	pWnd->GetWindowRect(&pos);
	pos.left += nXDelta;
	pos.right += nXDelta;
	pWnd->GetParent()->ScreenToClient(pos);
	pWnd->MoveWindow(pos);
}

void CCtrlCache::SizeCtrlVert(const int nId, const int nHeight)
{
	if (m_pParent == nullptr)
		return;

	CWnd* pWnd = m_pParent->GetDlgItem(nId);
	if (pWnd == nullptr)
		return;

	CRect pos;
	pWnd->GetWindowRect(&pos);
	pos.bottom = pos.top + nHeight;
	pWnd->GetParent()->ScreenToClient(pos);
	pWnd->MoveWindow(pos);
}

void CCtrlCache::SizeCtrlHoriz(const int nId, const int nWidth)
{
	if (m_pParent == nullptr)
		return;

	CWnd* pWnd = m_pParent->GetDlgItem(nId);
	if (pWnd == nullptr)
		return;

	CRect pos;
	pWnd->GetWindowRect(&pos);
	pos.right = pos.left + nWidth;
	pWnd->GetParent()->ScreenToClient(pos);
	pWnd->MoveWindow(pos);
}


