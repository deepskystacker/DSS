/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
File:	sectionabc.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef SECTIONABC_H
#define SECTIONABC_H

#ifndef GUITOOLS_H
	#include <guitools/guitools.h>
#endif	//	GUITOOLS_H

#ifndef CURSOR_H
	#include "cursor.h"
#endif	//	CURSOR_H

#ifndef SIMPLESTRING_H
	#include "SimpleString.h"
#endif	//	SIMPLESTRING_H

class CParentSection;

class CSectionABC : public WinHelper::CRect
{
public:
	explicit CSectionABC();
	/*lint -e1510*/
	virtual ~CSectionABC();
	/*lint +e1510*/

	//	Layout the child sections.
	virtual void OnLayout( const WinHelper::CRect &rc );

	//	handle a draw event.
	virtual void OnDraw( GS::CDrawContext &dc );

	//	Handle a mouse move event
	virtual void OnMouseMove( const WinHelper::CPoint &pt );

	//	Handle a left mouse button down event
	virtual void OnMouseLeftDown( const WinHelper::CPoint &pt );
	virtual void OnMouseRightDown( const WinHelper::CPoint & );

	//	Handle a left mouse up message, only gets sent if a left down was sent
	virtual void OnMouseLeftUp( const WinHelper::CPoint &pt );
	virtual void OnMouseRightUp( const WinHelper::CPoint & );

	//	Cancel any mouse downs (left, middle or right)
	virtual void OnStopMouseDown( );


	enum Event
		{
			knEventNone,
			knEventMouseEnter = 1,
			knEventMouseLeave = 2
		};

	//	Events, like mouse enter/leave, pass up to reach a parent or until there is no parent.
	virtual UINT OnParentEvent( const CSectionABC * pChild, const Event nEventType );

	//	Cancel any mouse capture
	virtual void OnStopMouseCapture( );

	//	Handle the changing mouse curor event, default does a find.
	virtual bool OnSetMouseCursor( const WinHelper::CPoint & );

	//	Called just before the section is to be destroyed.
	virtual void OnDestroy();

	//	Handle the mouse enter and leave events, usually highlight on/off
	virtual void OnMouseEnter();
	virtual void OnMouseLeave();

	//	Mouse wheel event...

	//	Set the mouse status
	inline void SetMouseInSection(bool bMouseIn) { m_bMouseInSection = bMouseIn; }

	//	Called when the mouse wheel is rolled.
	virtual void OnMouseWheel( int nDelta );

	//	Recieve timer events
	virtual void OnTimer( int nTimerID );

	//	set the cursor given a point
	virtual void SetCursor( const WinHelper::CPoint &pt );

	//	Called when a child control event happens
	virtual bool OnNotify( const CSectionABC *pChild, const int nEvent );

	//	Notify the parent of a change/event
	bool NotifyParent( int nEvent, const CSectionABC *pChild = NULL );

	//	Force a redraw of the section, or just part of it
	void ForceRedraw();
	virtual void ForceRedraw( const WinHelper::CRect &rc );
	virtual void DrawNow();

	//	Scroll the section display
	void Scroll( int cx, int cy, const WinHelper::CRect &rc );

	//	Set the transparency of the section. Used when removing items from the exclude regions
	virtual void Transparent( bool bTransparent );
	inline bool IsTransparent() const { return m_bTransparent ;}

	//	Determine if a point is in the section
	inline virtual bool IsPointInSection( const WinHelper::CPoint &pt ) const { return PtInRect( pt ); }

	//	Given a point determine the child that is beneath it
	virtual CSectionABC * FindSectionFromPoint( const WinHelper::CPoint &pt ) const;

	//	Get the parent of this section
	inline CParentSection * GetParent() const { return m_psectParent; }


	//	Set the text for this item's tool tip
	virtual void SetTipText( const StringClass &strTipText );
	virtual class CTipWindow *CreateTipWindow(  LPCTSTR pcszTip, WinHelper::CPoint &pt  );
	virtual bool ShouldShowTips() const;
	
	//	Called prior to a tooltip being displayed, if the return is NULL or an empty string 
	//	then no tooltip is displayed.
	virtual StringClass GetTipText() const;

	void SetKeep( bool bKeep ) { m_bKeep = bKeep; }
	bool IsKeep() const { return m_bKeep; }


	UINT GetID() const { return m_uID; }
	void SetID( UINT uID ) { m_uID = uID; }

	virtual void MoveX( int nOffsetX ) { left += nOffsetX; right += nOffsetX; }
	virtual void MoveY( int nOffsetY ) { top += nOffsetY; bottom += nOffsetY; }
	virtual void MoveXY( int nOffsetX, int nOffsetY ) { Offset( nOffsetX, nOffsetY ); }
	

	//
	//	ToolTip methods
	typedef enum { m_knLMouseButtonDown, m_knNCLMouseButtonDown, m_knMouseWheel } TipKillReason;
	virtual void KillTip( TipKillReason nKillReason );
	static void KillTip();
	void RestTipTimer( bool bReStart );

	//	Return true if this section has a tool tip displayed
	bool SectionHasToolTip() const { return m_pTippedWindow == this; }

	virtual HWND GetHwnd() const;

	void SetParent( CParentSection * psectParent ) { m_psectParent = psectParent; }

	void SetCharacterSet( BYTE cCharSet ) { m_cCharSet = cCharSet; }
	BYTE GetCharacterSet() const { return m_cCharSet; }


	//
	//	Element ID information
	LPCTSTR GetElementID() const { return m_strElementID; }
	void SetElementID( const StringClass &strElelementID )
	{
		m_strElementID = strElelementID;
	}

protected:
	enum { knNullTimerId = -1 };

	virtual int RegisterTimerEvent( CSectionABC *pSect, int nInterval );
	virtual void UnregisterTimerEvent( const int nTimerEventID );

	//	Return true if the mouse is currently in our section
	inline bool IsMouseInSection() const { return m_bMouseInSection; }
	//	Return true if the left mouse button has been pressed but not released
	inline bool IsLeftMouseDown() const { return m_bLeftMouseDown; }

	CCursor m_cursor;

	static class CTipWindow *m_pCurrentTip;
	static class CSectionABC *m_pTippedWindow;

private:
	CSectionABC( const CSectionABC & );
	CSectionABC& operator =( const CSectionABC & );

	CParentSection * m_psectParent;

	int m_nTipTimerID;


	bool m_bMouseInSection;
	bool m_bLeftMouseDown;
	bool m_bTransparent;

	StringClass m_strTipText;
	bool m_bKeep;
	UINT m_uID;
	BYTE m_cCharSet;

	StringClass m_strElementID;
};

#ifndef PARENTSECTION_H
	#include "ParentSection.h"
#endif	//	PARENTSECTION_H

#endif //SECTIONABC_H