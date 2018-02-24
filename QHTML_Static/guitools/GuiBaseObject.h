/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	GuiBaseObject.h
Owner:	russf@gipsysoft.com
Purpose:

	Gui base object is a simple object that manages child objects

----------------------------------------------------------------------*/
#ifndef GUIBASEOBJECT_H
#define GUIBASEOBJECT_H

#ifndef WINHELPER_H
	#include <reuse/winhelper.h>
#endif	//	WINHELPER_H

#ifndef DRAWCONTEXT_H
	#include "DrawContext.h"
#endif	//	DRAWCONTEXT_H

#ifndef ARRAY_CONTAINER_H
	#include <array.h>
#endif	//	ARRAY_CONTAINER_H

namespace GS
{

	class CGuiBaseObject : public WinHelper::CRect		//	gbo
	{
	public:
		CGuiBaseObject( CGuiBaseObject *pgboParent );
		
		virtual ~CGuiBaseObject();
		//
		//	Drawing...
		virtual void OnDraw( CDrawContext &dc );

		//
		//	Layout anything needed
		virtual void OnLayout( CDrawContext &dc );

		virtual void OnMouseLeftDown( const WinHelper::CPoint &pt );
		virtual void OnMouseLeftUp( const WinHelper::CPoint & );
		virtual void OnMouseLeftDoubleclick( const WinHelper::CPoint & );
		virtual void OnMouseCancelLeftMouse();
		virtual void OnMouseWheel( int nDelta );

		void Visible( bool bVisible )
			{ m_bVisible = bVisible; }
		bool IsVisible() const { return m_bVisible; }

		//
		//	Is the client relative point within this object
		inline virtual bool IsPointInSection( const WinHelper::CPoint &pt ) const
			{ return PtInRect( pt ); }



		CGuiBaseObject * FindObjectFromPoint( const WinHelper::CPoint &pt, bool bIncludeThis ) const;

		virtual void SetCursor();

		//
		//	Mouse entering and leaving our area...
		virtual void OnMouseLeave();
		virtual void OnMouseEnter();
		inline bool IsMouseInSection() const { return m_bMouseInSection; }



		class CTimer
		{
		public:
			explicit CTimer( CGuiBaseObject *pOwner )
				: m_pOwner( pOwner )
				, m_uTimer( 0 )
			{
			}
			~CTimer()
			{
				Stop();
			}

			bool IsSet() const { return m_uTimer ? true: false; }

			bool operator == ( const UINT uTimer ) { return m_uTimer == uTimer; }

			void Stop()
			{
				if( m_uTimer )
				{
					m_pOwner->StopTimer( m_uTimer );
					m_uTimer = 0;
				}
				ASSERT( !m_uTimer );
			}

			void Start( UINT uDuration )
			{
				Stop();
				m_uTimer = m_pOwner->StartTimer( m_pOwner, uDuration );
				ASSERT( m_uTimer );
			}
		private:
			UINT m_uTimer;
			CGuiBaseObject *m_pOwner;
		};

		//
		//	Timer fired
		virtual void OnTimer( UINT uTimerID );

		virtual HWND GetSafeHwnd() const;

	protected:
		//
		//	Update a rectangle, could be *this
		virtual void ForceRedraw( const WinHelper::CRect &rc );

		//
		//	Returns 0 if it fails, otherwise it returns a timer ID
		virtual UINT StartTimer( CGuiBaseObject *pgbo, UINT uDuration );
		//
		//	Stop a timer you started
		virtual void StopTimer( UINT uTimerID );

		void AddChild( CGuiBaseObject * );
		void RemoveChild( CGuiBaseObject * );

		bool m_bVisible;

	private:
		friend class CTimer;
		bool m_bMouseDown;
		bool m_bMouseInSection;

		Container::CArray< CGuiBaseObject * > m_arrChild;
		CGuiBaseObject *m_pgboParent;
	};
};

#endif //GUIBASEOBJECT_H