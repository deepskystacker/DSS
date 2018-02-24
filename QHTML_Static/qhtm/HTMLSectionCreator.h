/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLSectionCreator.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef HTMLSECTIONCREATOR_H
#define HTMLSECTIONCREATOR_H


#ifndef STACK_H
	#include <reuse/Stack.h>
#endif	//	STACK_H

#ifndef HTMLPARSE_H
	#include "htmlparse.h"
#endif	//	HTMLPARSE_H

#ifndef HTMLSECTION_H
	#include "HTMLSection.h"
#endif	//	HTMLSECTION_H

#ifndef HTMLSECTIONABC_H
	#include "HTMLSectionABC.h"
#endif	//	HTMLSECTIONABC_H

// Constants used to indicate special conditions to CHTMLSectionCreator
// Valid only when m_bMeasuring is true
const int knFindMaximumWidth = 32767;

class CQHTMImageABC;
class CHTMLSection;
class CSectionABC;
class CHTMLSectionLink;

namespace GS
{
	class CDrawContext;
};


class CHTMLSectionCreator
{
public:
	CHTMLSectionCreator( CHTMLSection *psect, GS::CDrawContext &dc, int nTop, int nLeft, int nRight, COLORREF crBack, bool bMeasuring, int nZoomLevel, CHTMLSectionLink* pCurrentLink, CHTMLSectionCreator *psc );
	virtual ~CHTMLSectionCreator();

	//	Get the height of the sections and therefore the height
	WinHelper::CSize GetSize() const { return WinHelper::CSize( m_nWidth, m_nYPos ); }

	//	Add a new paragraph
	void NewParagraph( int nSpaceAbove, int nSpaceBelow, CStyle::Align alg = CStyle::algEmpty );

	//	Add a document
	void AddDocument( CHTMLDocument *pDocument );

	//
	//	Add a section to our list
	void AddSection( CHTMLSectionABC *pSect )
	{
			m_psect->AddSection( pSect );
		if( m_pCurrentLink )
			pSect->SetAsLink( m_pCurrentLink );
	}

	CHTMLSectionLink* GetCurrentLink() const { return m_pCurrentLink; }

	//
	//	Gain access to the underlying HTML display
	CHTMLSection *GetHTMLSection() const { return m_psect; }

	//
	//	Y-Position control
	void AddYPos( int nToAdd ) { m_nYPos += nToAdd; }
	void SetCurrentYPos( int nYPos ) { m_nYPos = nYPos; }
	int GetCurrentYPos() const { return m_nYPos; }

	//
	//	X-Position control
	void SetCurrentXPos( int nXPos ) { m_nXPos = nXPos; }
	int GetCurrentXPos() const { return m_nXPos; }

	//
	//	X-Position control
	void SetNextYPos( int nYPos ) { m_nNextYPos = nYPos; }
	int GetNextYPos() const { return m_nNextYPos; }
	
	//
	//	Access the current drawing context
	/*lint -e1536 */
	GS::CDrawContext &GetDC() const { return m_dc; }
	/*lint +e1536 */


	//
	//	Access the current margins.
	inline int GetLeftMargin() const { return m_nLeftMargin; }
	inline int GetRightMargin() const { return m_nRightMargin; }

	//
	//	Move the Y position to be below the respective margins
	void SkipLeftMargin();
	void SkipRightMargin();

	//
	//	Add a new left margin, complete with when the margin expires
	inline void AddNewLeftMargin( int nMargin, int nYExpiry )
	{
		MarginStackItem & msi = m_stkLeftMargin.Add();
		m_nLeftMargin = msi.nMargin = nMargin;
		msi.nYExpiry = nYExpiry;
	}

	//
	//	Add a new right margin, complete with when the margin expires
	inline void AddNewRightMargin( int nMargin, int nYExpiry )
	{
		MarginStackItem & msi = m_stkRightMargin.Add();
		m_nRightMargin = msi.nMargin = nMargin;
		msi.nYExpiry = nYExpiry;
	}

	//
	//	Get the current display width - taking into accounr the current margins
	inline int GetCurrentWidth() const { return m_nRightMargin - m_nLeftMargin; }

	//
	//	Action a carriage return
	void CarriageReturn( bool bSetNextY );

	//
	//	Add a baseline for the current item
	void AddBaseline( int nBaseline );

	//
	//	Update the passed item if a hyperlink is active
	void UpdateIfLink( CHTMLSectionABC *pSection )
	{
		if( m_pCurrentLink )
			pSection->SetAsLink( m_pCurrentLink );
	}

	//
	//	Are we doing it for real or are we just measuring?
	bool IsMeasuring() const { return m_bMeasuring; }

	COLORREF GetBackgroundColor() const { return m_crBack; }

	int GetZoomLevel() const { return m_nZoomLevel; }

	BYTE GetCurrentCharSet() const { return m_cCharSet; }

	size_t GetCurrentShapeID() const;

	int GetDefaultRightMargin() const { return m_nDefaultRightMargin; }
	int GetDefaultLeftMargin() const { return m_nDefaultLeftMargin; }

	void AddBreak() { m_psect->AddBreakSection( GetCurrentShapeID() );}

	void SetCurrentLink( CHTMLSectionLink* pCurrentLink ) { m_pCurrentLink = pCurrentLink; }

	const GS::FontDef * GetDrawingFont( const HTMLFontDef &def );


private:
	void Finished();
	void AdjustShapeBaselinesAndHorizontalAlignment();
	inline int GetBaseline( size_t nShape ) const;


	CHTMLSection *m_psect;
	/*lint -e1725 */
	GS::CDrawContext &m_dc;
	/*lint +e1725 */

	//	The left and right margins passed into this object, what we default to.
	int m_nDefaultLeftMargin, m_nDefaultRightMargin;

	int m_nTop;

	//	Our current margins
	int m_nLeftMargin, m_nRightMargin;
	
	//	The current position when drawing our objects.
	int m_nYPos, m_nXPos;


	//	This flag is set in the ctor when we are determining the extents
	//	of a document. It prevents the re-alignment of objects based
	//	on artificial margins.
	bool m_bMeasuring;

	int m_nPreviousParaSpaceBelow;
	CStyle::Align m_algCurrentPargraph;

	//
	//	Margin Stacks. Used to alter the margins for a set number of vertical pixels
	struct MarginStackItem	//	msi
	{
		int nMargin;
		int nYExpiry;
	};
	StackClass<MarginStackItem> m_stkLeftMargin;
	StackClass<MarginStackItem> m_stkRightMargin;

	size_t m_nFirstShapeOnLine;
	int m_nNextYPos;

	//	Map of shape IDs (-1) to shape baselines.
	MapClass<size_t, int>	m_mapBaseline;

	int m_nLowestBaseline;

	//
	//	Original line width
	int m_nLineWidth;

	//
	//	Running tally of the width of the document
	int m_nWidth;

	UINT m_nZoomLevel;
	BYTE m_cCharSet;
	COLORREF m_crBack;

	//
	//	The current link that will be associated with new display elements
	CHTMLSectionLink* m_pCurrentLink;

	CHTMLSectionCreator *m_pscParent;

	Container::CMap< HTMLFontDef, GS::FontDef * > m_mapFonts;

private:
	CHTMLSectionCreator();
	CHTMLSectionCreator( const CHTMLSectionCreator& );
	CHTMLSectionCreator& operator =( const CHTMLSectionCreator& );
};

#endif //HTMLSECTIONCREATOR_H