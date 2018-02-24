/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	ParentSection.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef PARENTSECTION_H
#define PARENTSECTION_H

#ifndef SECTIONABC_H
	#include "SectionABC.h"
#endif	//	SECTIONABC_H

class CParentSection: public CSectionABC
{
public:
	explicit CParentSection();
	virtual ~CParentSection();

	//	handle a draw event.
	virtual void OnDraw( GS::CDrawContext &dc );

	//	Remove a section from the list of sections
	void RemoveAllSections( bool bRemoveKeepSectionToo );


	//	Add a section to the list of sections this section contains.
	void AddSection( CSectionABC *pSect );

	//	Get the number of children the section has
	inline size_t GetSectionCount() const { return m_arrSections.GetSize(); }

	//	Get at a section by index
	inline CSectionABC *GetSectionAt( size_t nIndex ) const { return m_arrSections[ nIndex ]; }

	//	Tell our children about our transparent state
	virtual void Transparent( bool bTransparent );
	virtual CSectionABC * FindSectionFromPoint( const WinHelper::CPoint &pt ) const;

	//
	//	Given an ID locate the element
	CSectionABC *GetKeeperItemByID( UINT uID );

protected:
	typedef Container::CArray< CSectionABC *> CSectionList; 

	CSectionList m_arrSections;
	CSectionList m_arrSectionsKeepers;

private:


private:
	CParentSection( const CParentSection & );
	CParentSection& operator =( const CParentSection & );
};

#endif //PARENTSECTION_H