/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	ParentSection.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "ParentSection.h"

CParentSection::CParentSection()
{

}

CParentSection::~CParentSection()
{
	ASSERT( m_arrSectionsKeepers.GetSize() == 0 );
}


void CParentSection::OnDraw( GS::CDrawContext &dc )
{
	const WinHelper::CRect &rcClip = dc.GetClipRect();
	const size_t uSize = m_arrSections.GetSize();

	for( size_t n = 0; n < uSize; n++ )
	{
		CSectionABC *psect = m_arrSections[ n ];
		if( rcClip.Intersect( *psect ) )
		{
			psect->OnDraw( dc );
		}
	}
}


void CParentSection::Transparent( bool bTransparent )
{
	CSectionABC::Transparent( bTransparent );

	const size_t uSize = m_arrSections.GetSize();

	for( size_t n = 0; n < uSize; n++ )
	{
		m_arrSections[ n ]->Transparent( bTransparent );
	}
}


void CParentSection::AddSection( CSectionABC *pSect )
{
	pSect->SetParent( this );

	m_arrSections.Add( pSect );
}


void CParentSection::RemoveAllSections( bool bRemoveKeepSectionToo )
{
	const size_t uSize = m_arrSections.GetSize();
	if( bRemoveKeepSectionToo )
	{
		for( size_t n = 0; n < uSize; n++ )
		{
			delete m_arrSections[ n ];
		}
	}
	else
	{
		for( size_t n = 0; n < uSize; n++ )
		{
			CSectionABC *pSect = m_arrSections[ n ];
			if( pSect->IsKeep() )
			{
				m_arrSectionsKeepers.Add( pSect );
			}
			else
			{
				delete m_arrSections[ n ];
			}
		}
	}

	m_arrSections.RemoveAll();
}


CSectionABC * CParentSection::FindSectionFromPoint(const WinHelper::CPoint &pt) const
//
//	Given a point return pointer to a section if the pointer is within a section
//	Can return NULL if there is no section at the point.
{
	CSectionABC *pSect = NULL;
	const size_t uSize = m_arrSections.GetSize();
	for( size_t n = 0; n < uSize; n++ )
	{
		CSectionABC *psect = m_arrSections[ n ];
		if( psect->IsPointInSection( pt ) )
		{
			pSect = psect;
			CSectionABC *pSect2 = pSect->FindSectionFromPoint( pt );
			if( pSect2 )
			{
				pSect = pSect2;
			}
			break;
		}
	}
	return pSect;
}


CSectionABC *CParentSection::GetKeeperItemByID( UINT uID )
{
	const size_t uSize = m_arrSectionsKeepers.GetSize();
	for( size_t n = 0; n < uSize; n++ )
	{
		CSectionABC *psect = m_arrSectionsKeepers[ n ];
		if( psect->GetID() == uID )
		{
			m_arrSectionsKeepers.RemoveAt( n );
			return psect;
		}
	}
	return NULL;
}