/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	HTMLSection.h
Owner:	russf@gipsysoft.com
Purpose:	Main container section for the basic HTML control.

	This class basically wraps all functionality for the HTML that is
	not really "window" specific. For example; we also use this class
	for printing, rendering to HDCs and bitmaps.

----------------------------------------------------------------------*/
#ifndef HTMLSECTION_H
#define HTMLSECTION_H

#ifndef SCROLLCONTAINER_H
	#include "ScrollContainer.h"
#endif	//	SCROLLCONTAINER_H

#ifndef HTMLSECTIONLINK_H
	#include "HTMLSectionLink.h"
#endif // HTMLSECTIONLINK_H

#ifndef SIMPLESTRING_H
	#include "SimpleString.h"
#endif	//	SIMPLESTRING_H

#ifndef QHTM_H
	#include "QHTM.h"
#endif	//	QHTM_H

class CDefaults;
class CHTMLForm;

class CHTMLSection : public CScrollContainer
{
public:
	explicit CHTMLSection( CDefaults *pDefaults );
	virtual ~CHTMLSection();

	virtual void OnLayout( const WinHelper::CRect &rc, GS::CDrawContext &dc );
	virtual void OnDraw( GS::CDrawContext &dc );
	virtual bool OnNotify( const CSectionABC *pChild, const int nEvent );

	//	Set the HTML content for this section
	void SetHTML( LPCTSTR pcszHTMLText, size_t nLength, LPCTSTR pcszPathToFile );
	bool SetHTML( HINSTANCE hInst, LPCTSTR pcszName );
	bool SetHTMLFile( LPCTSTR pcszFilename );
	
	//	Add HTML to the current document
	void AddHTML( LPCTSTR pcsz );

	//	Set the default margins for the control.
	void SetDefaultMargins( LPCRECT lprect );
	void GetDefaultMargins( LPRECT lprect ) const;

	//	Set the default resource handle used
	void SetDefaultResourceHandle( HINSTANCE hInstance );

	//	Load the HTML from a resource.
	void LoadFromResource( UINT uID );

	//	Accessor for the title
	const StringClass & GetTitle() const;

	//
	//	Set the image callback for this control
	void SetImageCallback( funcQHTMImageCallback pfuncQHTMImageCallback );

	//
	//	Set the callback for plain bitmaps
	void SetBitmapCallback( funcQHTMBitmapCallback pfuncBitmapCallback );

	//
	//	Set the callback for QHTM to acquire any resources it needs
	void SetResourceCallback( funcQHTMResourceCallback pfuncQHTMResourceCallback );

	//
	//	Set the callback for forms
	void SetFormCallback( funcQHTMFORMCallback pfuncQHTMFORMCallback );

	//	Setthe general event callback
	void SetEventCallback( funcQHTMEventCallback pfuncQHTMEventCallback );

	//	Goto a named link.
	void GotoLink( CHTMLSectionLink *pLink );
	void GotoLink( LPCTSTR pcszLinkTarget );

	//	Force a form to be subitted...
	bool SubmitForm( LPCTSTR pcszFormName, LPCTSTR pcszSubmitValue );

	//	Reset a form
	bool ResetForm( LPCTSTR pcszFormName );

	inline const WinHelper::CSize &GetSize() const { return sizeHTML; }
	inline int GetHeight() const { return sizeHTML.cy; }

	//	Enable/disable tooltips
	void EnableTooltips( bool bEnable ) const;
	bool IsTooltipsEnabled() const;

	//	Pagination Methods
	size_t Paginate(const WinHelper::CRect& rcPage);	// Returns number of pages
	WinHelper::CRect GetPageRect(UINT nPage) const;

	//
	//	Kludge for preasured tables
	void ResetMeasuringKludge();

	//	Set the zoom level used.
	void SetZoomLevel( UINT uZoomLevel );

	//	Get the zoom level used.
	inline int GetZoomLevel() const { return m_uZoomLevel; }

	//	Destroy the document and all child sections
	void DestroyDocument();

	//
	//	Add a hyperlink and get back a controlling section for it.
	CHTMLSectionLink* AddLink( LPCTSTR pcszLinkTarget, LPCTSTR pcszLinkTitle, LPCTSTR pcszID, COLORREF crLink, COLORREF crHover, bool bTitleSet );

	//
	//	Add an object that is able to have the focus set to it.
	void AddFocusObject( CFocusControlABC *pObject );

	//
	//	Add a named section, this is for internal hyperlinks.
	void AddNamedSection(LPCTSTR name, const WinHelper::CPoint& point);

	//
	//	Add a section ID where a possible page break could occurr.
	void AddBreakSection( size_t i );

	//
	//	Select the link nearest to the point *or* the currently focused link.
	void SelectNearestLink( const POINT &pt );

	//
	//	Keyboard handling
	bool TabToNextControl();
	bool TabToPreviousControl();


	bool SelectPreviousInGroup();
	bool SelectNextInGroup();

	//
	//	Return true if we have something we can tab onto. false otherwise
	bool HasControls() const;

	//
	//	Link activation
	void ActivateLink();

	//
	//	Are we at the begining or end of the control for tabbing around?
	bool CanTabForward() const;
	bool CanTabBackward() const;

	//
	//	We got the focus!
	void SetFocus( bool bFocused );

	//
	//	Set the focus to either the first or the last control
	void SetFocusControl( bool bFirst );

	//
	//	Set the focus to a specific item
	void SetCurrentFocus( CFocusControlABC *pFocus, bool bScrollIntoView );

	//
	//	Have we got the focus?
	inline bool IsFocused() const { return m_bFocused; }

	//
	//	Have we got a focus section
	inline bool HasFocusSection() const { return m_pFocus != NULL; }


	//
	//	Submit a form!
	void SubmitForm( CHTMLForm *pForm, LPCTSTR pcszSubmitValue );

	//
	//	Return the found section link, or null otherwise
	const CHTMLSectionLink *GetLinkFromSection( const CSectionABC *pSect ) const;

	//
	//	Get a HRGN from a link
	HRGN GetLinkRegion( LPCTSTR pcszLinkID ) const;

	//
	//	Get the contained HTML
	const StringClass &GetText() const { return m_strHTML; }

	//
	//	Line height	(last used font height)
	int GetLineHeight() const { return m_nLineHeight; }

	//
	//	Set the focus to, and activate, a named section
	void SetFocusAndActivate( LPCTSTR pcszNameSection );

private:
	//
	//	Get the background for us
	virtual bool GetBackgroundColours( HDC , HBRUSH & ) const { return false;}
	virtual void OnExecuteHyperlink( CHTMLSectionLink *pHtmlLink );
	virtual void OnTimer( int nTimerID );


	//
	//	Called when a new document has been loaded
	void OnLoadedDocument();

	//
	//	Stop our background image timer. Only used when we have a background image of course.
	void KillBackgroundImageTimer();

	size_t FindOurControl( CFocusControlABC *pFocusControl ) const;
	CFocusControlABC *FindSelectedFromGroup( CFocusControlABC *pFocusNext );

	//
	//	Our main HTML document - can be NULL.
	class CHTMLDocument *m_pDocument;


	//	Just remove the child sections
	void RemoveAllChildSections( bool bRemoveKeepSectionToo );

	//	Layout a document to the display given a width, returns the document height
	WinHelper::CSize LayoutDocument( GS::CDrawContext &dc, CHTMLDocument *m_pDocument, int nYPos, int nLeft, int nRight );

	//
	//	The size of the contained HTML in pixels.
	WinHelper::CSize sizeHTML;


	//
	//	Store a list of links in this Section. Each link will have a list
	//	of sections it contains.
	ArrayClass<CHTMLSectionLink*> m_arrLinks;


	//	Create a map of names in this document. The position saved is 
	//	The current position.
	MapClass<StringClass, WinHelper::CPoint> m_mapNames;

	//	Create a list of sections (by index) before which a page break may occur
	ArrayClass< size_t > m_arrBreakSections;

	//	Pagination data
	ArrayClass< WinHelper::CRect > m_arrPageRects;

	UINT m_uZoomLevel;

	//
	//	Document title
	StringClass m_strTitle;

	//
	//	Defaults for this object
	CDefaults *m_pDefaults;

	//
	//	Background image stuff
	int m_nBackgroundTimerID;
	UINT  m_nBackgroundFrame;


	//
	//	Focus control stuff
	CFocusControlABC *m_pFocus;
	ArrayClass< CFocusControlABC * > m_arrFocus;
	bool m_bFocused;
	int m_nLineHeight;

	StringClass m_strHTML;

	HINSTANCE m_hDefaultInstance;


private:
	CHTMLSection();
	CHTMLSection( const CHTMLSection &);
	CHTMLSection& operator =( const CHTMLSection &);
};


#endif //HTMLSECTION_H