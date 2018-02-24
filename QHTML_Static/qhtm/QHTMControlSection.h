/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	QHTMControlSection.h
Owner:	russf@gipsysoft.com
Purpose:	QHTM control.
----------------------------------------------------------------------*/
#ifndef QHTMCONTROLSECTION_H
#define QHTMCONTROLSECTION_H

#ifndef HTMLSECTION_H
	#include "HTMLSection.h"
#endif //#ifndef HTMLSECTION_H

#ifndef DEFAULTS_H
	#include "Defaults.h"
#endif	//	DEFAULTS_H

class CQHTMControlSection : public CParentSection
{
public:
	CQHTMControlSection( HWND hwnd );
	virtual ~CQHTMControlSection();
	virtual void OnLayout( const WinHelper::CRect &rc );
	virtual bool OnNotify( const CSectionABC *pChild, const int nEvent );

	void SetText( LPCTSTR pcszText );
	
	//	Called when the HTML is required to be loaded from the resources.
	bool LoadFromResource( HINSTANCE hInst, LPCTSTR pcszName );

	//	Set the default resource handle used
	void SetDefaultResourceHandle( HINSTANCE hInstance );

	//	Called when the HTML is required to be loaded from a file.
	bool LoadFromFile( LPCTSTR pcszFilename );

	//	Force a form to be submitted
	bool SubmitForm( LPCTSTR pcszFormName, LPCTSTR pcszSubmitValue );

	//	Reset a form 
	bool ResetForm( LPCTSTR pcszFormName );

	//	Get a HRGN for a link
	HRGN GetLinkRegion( LPCTSTR pcszLinkID ) const;

	void SetLParam( LPARAM lParam );

	void OnExecuteHyperlink( CHTMLSectionLink *pHtmlLink );
	void OnVScroll( int nScrollCode );
	void OnHScroll( int nScrollCode );
	void OnMouseWheel( int nDelta );

	//
	//	Get the size of the HTML
	const WinHelper::CSize &GetSize() const { return m_htmlSection.GetSize(); }

	//	Set an option
	int SetOption( UINT uOptionIndex, LPARAM lParam );

	//	Get an option
	LPARAM GetOption( UINT uOptionIndex, LPARAM lParam );

	//	Force the control to jump to a link
	void GotoLink( LPCTSTR pcszLinkName );

	//
	//	Get the length of the HTML title
	size_t GetTitleLength() const;

	//
	//	Get the HTML title
	size_t GetTitle( size_t uBufferLength, LPTSTR pszBuffer ) const;

	bool IsLayingOut() const { return m_bLayingOut; }

	void SelectNearestLink( const POINT &pt );

	//
	//	Set the font to use
	void SetFont( HFONT hFont );

	//
	//	Update the cursor
	bool UpdateCursor();


	//	Get/Set the scroll position of the control.
	void SetScrollPos( int n )
	{
		const WinHelper::CSize size( m_htmlSection.GetSize() );
		if( n > size.cy - Height() )
		{
			n = size.cy - Height();
		}

		if( n == -1 )
		{
			n = size.cy - Height();
		}

		::SetScrollPos( m_hwnd, SB_VERT, n, TRUE );
		m_htmlSection.SetPos( n );
		InvalidateRect( m_hwnd, NULL, TRUE );

		UpdateCursor();
	}
	int GetScrollPos( ) { return m_htmlSection.GetScrollPos(); }
	virtual bool ShouldShowTips() const;

	//
	//	Add HTMl to the currenrt document
	void OnAddHTML( LPCTSTR pcsz, UINT uValue );


	static LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

	bool GetBackgroundColours( HDC hdc, HBRUSH &hbr ) const;

	inline HWND GetHwnd() const { return m_hwnd; }

	virtual void ForceRedraw( const WinHelper::CRect &rc );
	virtual void DrawNow();

	//
	//	Are we at the begining or end of the control for tabbing around?
	//	Return true if there are more controls to be tabbed onto.
	bool CanTabForward() const;
	bool CanTabBackward() const;

	bool SelectPreviousInGroup();
	bool SelectNextInGroup();

	//
	//	Return true if we have something we can tab onto. false otherwise
	bool HasControls() const;

	//
	//	Someone wants us to have the focus!
	void SetFocus( bool bFocused );

	//
	//	Set the focus to either the first or the last control
	void SetFocusControl( bool bFirst );

	//
	//	Return true if there is a link at the point. If there is fill out the lpInfo structure
	BOOL GetLinkFromPoint( LPPOINT lpPoint, LPQHTM_LINK_INFO lpInfo ) const;

	//
	//	Get the contained HTML
	const StringClass &GetText() const { return m_htmlSection.GetText(); }

private:
	virtual void OnTimer( int nTimerID );
	virtual int RegisterTimerEvent( CSectionABC *pSect, int nInterval );
	virtual void UnregisterTimerEvent( const int nTimerEventID );
	virtual void OnMouseMove( const WinHelper::CPoint &pt );
	virtual void OnDestroy();
	virtual UINT OnParentEvent( const CSectionABC * pChild, const Event nEventType );

	//
	//	Internal use - when we have recieved some new HTML
	void OnNewHTMLTextSet();

	//
	//	Keyboard handling
	bool TabToNextControl();
	bool TabToPreviousControl();

	void ActivateFocusedItem();

	//
	//	!!!! Needs to be first due to construction order !!!!
	CDefaults m_defaults;

	void LayoutHTML();
	void ResetScrollPos();

	class CHTMLSection2 : public CHTMLSection
	{
	public:
		explicit CHTMLSection2( CQHTMControlSection *psectParent, CDefaults *pDefaults ) : CHTMLSection( pDefaults ), m_psectParent( psectParent ) { psectParent->AddSection( this ); }

		virtual void OnExecuteHyperlink( CHTMLSectionLink *pHtmlLink )
		{
			static_cast<CQHTMControlSection *>( GetParent() )->OnExecuteHyperlink( pHtmlLink );
		}

		virtual bool GetBackgroundColours( HDC hdc, HBRUSH &hbr ) const
		{
			return m_psectParent->GetBackgroundColours( hdc, hbr );
		}

	private:
		CQHTMControlSection *m_psectParent;
		CHTMLSection2();
		CHTMLSection2( const CHTMLSection2 &);
		CHTMLSection2& operator =( const CHTMLSection2 &);
	};

	CHTMLSection2	m_htmlSection;

	bool m_bEnableTooltips;
	bool m_bAlwaysShowTips;
	bool m_bLayingOut;
	bool m_bUseColorStatic;
	bool m_bEnableScrollbars;

	HWND	m_hwnd;
	int m_nMouseMoveTimerID;

	Container::CMap<int, CSectionABC *> m_mapTimerEvents;
	int m_nNextTimerID;

	CSectionABC *m_pCaptureSection;

	bool m_bHadTABSTOP;
	bool m_bShowHorizontal;
	bool m_bShowVertical;

private:
	CQHTMControlSection( const CQHTMControlSection &);
	CQHTMControlSection &operator =( const CQHTMControlSection &);
};


#endif //QHTMCONTROLSECTION_H